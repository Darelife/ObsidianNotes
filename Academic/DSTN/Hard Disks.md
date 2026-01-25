# LBA, CHS

## Controller 

Responsible for monitoring the health. If suppose CHS (0,0,8) is corrupted, it will map it to (0,1,0) or something else. 

> [!info] Something about defragmentation, and disk checker

There are reserved blocks, to map to...like, (0,1,0) will be reserved, and there won't be any LBA assigned to those. 

## Queuing (Also called buffering)

Allows the disk to reorder the I/O ops, so that the read and write's are executed in an order optimized for the layout of the disk. Like, if we want to access LBA's 10,15,11, and 14. In which order should I perform the IO ops??

> [!info] From the file system's point of view, the performance is measured till the data goes to the buffer, not in the actual disk.

## Interface and Protocol

### SATA (Serial Advanced Technology Attachment)

Cheap, low perf, and high capacity.
Not for servers. Slow speed.

### SAS (Serial Attached SCSI)

Has 2 processors (for fault tolerance, and multiple channels)
Dual Ported

### Fibre Channel

Same command set like SCSI, and queuing like SAS

### Nearline SAS (NL-SAS)

Hybrid of SAS and SATA. Better than SATA, but can't transfer data at a lightning speed. SAS Interface, and the SAS protocol, but the RPM of a SATA drive

# Speed

## Drive Speed

Usually, 5400, 7200, 10000, and 15000 RPM. SATA can't handle higher speeds. 

## Drive Seek Time

Time taken to move the read/write heads to the correct position.  Usually in the order of ms. Compute happens in micro/nano. Thrashing = a lot of seeks. Usually average seek time is the time to move it from the circumference to the center by 3, instead of 2....(calculus) integral of (x-y) dy, from 0 to x. (X = radius)

## Rotational Latency

Time it takes for the platter to make half the rotation. For 15000 rpm, average is (60/15000)/2 = 2 ms. So basically, if it's random accessing, we'll spend 7ms (seek time) + 2ms....so like, almost 10ms just to reach.


> [!info] IO Time = Seek Time + Rotational Latency + Data Transfer Time

Assume 100MBps as the Transfer Rate. So, for 512 KB

Transfer Rate 
- Maximum Transfer Rate (MTR)
- Sustained Transfer Rate (STR) : Much more realistic, and smaller

## Track Skew

Basically, since my disk is rotating, once we read 10 (suppose 10 tracks in a cycle), then, we'll have to switch to 11, which will take some time. So, we'll have to ensure that we have 11, not directly below 10, but a bit ahead. Skewing is done by manufacturers. 

```c
T_sector = (60/RPM)/number of sectors
Track Skew = ciel(T_head_speed/T_sector)

IO Time = Seek Time + Rotational Latency + Data transfer time + Controller overhead + Queuing Delay.

IO Rate = Transfer Size/IO Time

IOPS = number of reads and writes per seconds (depends on conditions)
```

> [!Question] Seek Time is 3.5 ms. 15k RPM, time for 1 r/w op = order of microseconds (can be ignored) IOPS = ?
> 
> IOPS = 1000/(3.5+2)

> [!question] 2 hard disks, (300gb vs 1tb) (15000 vs 7200), seek = (4ms vs 9ms), Max Transfer = (125MB/s vs 105)

There were more qs...check the slides

# Disk Scheduling

## Disk Scheduling Algorithms

1. FCFS (FIFO)
	1. Simple to implement, and no starving
	2. But it doesn't optimize the seek time (Time to change tracks)
2. SSTF (Shortest Seek Time First)
	1. Average Response Time Decreases
	2. Throughput increases
	3. If our batch size is larger, it will cause starvation
3. SCAN/Elevator
	1. Head moves back and forth across the disk, servicing reqs in order across the tracks.
	2. Advantage
		1. Avg Response Time decreases
		2. Throughput increases
		3. Low variance of response time
		4. No Starvation
	3. Goes till the ends......sweeps through the ends basically.  Eg: 10,29,30,40,50,60,70
		1. If it starts at 30, and was moving right, it will do, 30,40,50,60,70,29,10
	4. Disadvantage : Favours the  middle tracks
4. C-SCAN
	1. Scans from Inner to Outer, and then, instantly goes back to Inner, without scanning...repeat.
	2. Advantage
		1. Avg Response Time dec
		2. Throughput inc
		3. Low variance of res time
		4. no starvation
		5. provides more uniform wait time compared to scan
		6. no favouring of the middle tracks
5. F-SCAN (Freeze Scan)
	1. Uses 2 sub queues
	2. Freezes the queue to be serviced when its doing a scan.
	3. New inc reqs are added to the 2nd queue.
	4. Avoids starvation of far away requests, by delaying the servicing of late arriving (but nearer) reqs
6. LOOK
	1. Similar to SCAN. Doesn't go till the end, just till the last reqs
	2. Prevents the extra delay which occurred due to unnecessary traversal to the end of the disk.
7. C-LOOK
	1. Just like, C-SCAN to SCAN, but with LOOK instead
8. SPTF (Shortest Positioning Time First / Shortest Access Time First)
	1. Just looking at seek time isn't enough.

## How are these Algorithms Implemented?

Previously OS would do it, but now, the drive controllers can control it.
OS would pick a batch size (like 16), and then would follow the nearest block first (NBF), which follows the request with the nearest LBAs.
I/O merging is also done by the OS.

The disk uses its internal knowledge of the head position, and the detailed track layout info to service the requests, and would follow the best possible scheduling algorithm (like, SPTF).

### Linux I/O Schedulers

1. None
	1. Doesn't reorder the reqs
2. MQ Deadline (Multiple Queue Deadline)
	1. Uses 3 Queues
		1. Sorted - Tries Sequential (Has both Read and Write)
		2. Read FIFO
		3. Write FIFO
	2. Reads have a 500ms deadline, and Reads have a 5s deadline.
	3. Reqs are issued from the sorted queue, unless a read from the head of a read or write FIFO expires. Reads are preferred over Write
	4. Fixes starvation issues. Good all rounder, with fairly low CPU Overhead
3. Kyber
	1. Designed for fast multiqueue devices, and is relatively simple.
	2. Has 2 queues
		1. Synchronous Requests (Eg: Blocked Reads) (A lot of db things, where we need to order the reqs)
		2. Async Reqs (Eg: Writes) (The OS will do the ordering, and all)
	3. There are strict limits on the number of request operations sent to the queues.
	4. This limits the time waiting for requests to be dispatched provifing quick completion time for requests that are high priority.
4. BFQ (Budget Fair Queuing)
	1. Fair sharing based on the number of sectors requested rather than a time slice. Has a thing like MLFQ (low priority gets more time, when it runs)
	2. Complex I/O scheduler with high per operation overhead.
		1. Not ideal for devices with slow CPU or high throughput I/O devices.
	3. Advantageous when loading large applications

### Which Linux I/O Scheduler to use?

- SSD/NVMe
	- Little to no difference in throughput between mq-deadline/none/bfq schedulers. (it's preferred to use none nowadays though)
- HDD
	- mq-deadline is shown to be better
	- if we have larger applications, bfq

# System Calls

## Creating a file
```c
#include <fcntl.h>
int open(const char *pathname, int flags, mode_t mode);

int fd = open("foo", O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
```

fd is a file descriptor. It's an integer, which is private per process, and is used to read/write to the file.

Each process has a simple array of files in its struct
`struct file *ofile[NOFILE]`

O_SYNC? O_DIRECT?

- O_Sync : No return until the hdd actually writes it (The write() call does not return until the data (and required metadata) is flushed to stable storage.) (We'll get the actual hdd throughput when we use O_SYNC)

- O_Direct : Won't save it in the kernel space (Data bypasses the kernel page cache and is transferred directly between user space and the storage device.) It will return when it saves in the hard disk buffer too (sync won't do it...sync will wait until it actually does the IO)

## Reading a file
```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
```

read upto count bytes from the file descriptor fd, into the buffer starting at buf.

On success, the number of bytes read is returned, and the file position is advanced by this number.
On error, -1 is returned.

## Writing to a file
```c
#include <unistd.h>
ssize_t write(int fd, void *buf, size_t count);
```

A successful write may transfer fewer than count bytes.
1. Insufficient space on the disk device
2. A blocked write to a socket, pipe, or similar was interrupted by a signal handler.
3. The caller can make another write call to transfer the remaining bytes.
4. It will either transfer further bytes, or may result in errors (disk full)


## Reading and Writing not sequentially

```c
#include <unistd.h>
off_t lseek(int fd, off_t offset, int whence);
```

Used to reposition the offset basically for the fd.

if whence == 
1. SEEK_SET, offset = offset
2. SEEK_CUR, offset = current offset + offset
3. SEEK_END, offset = file_end + offset 

SEEK_END + |x| = adds some blank space, till |x| after the end, and then, puts the pointer there in the end. Like, the file size doesn't immediately change, but a hole is created later, if we write.

we can also do SEEK_END + (-x)....but SEEK_END-x >=0...if it's < 0, it will cause an error `EINVAL`.

SEEK_HOLE : The first empty point (\0)
SEEK_DATA : The first data entry

## The structs

```c
struct file {
	enum { FD_NONE, FD_PIPE, FD_INODE } type;
	int ref; // reference count...indegree
	char readable;
	char writable;
	struct pipe *pipe;
	struct inode *ip;
	uint off;
};

struct pipe {
	struct spinlock lock;
	char data[PIPESIZE];
	uint nread; // number of bytes read
	uint nwrite; // number of bytes written
	int readopen; // read fd is still open
	int writeopen; // write fd is still open
};
```



Process File Descriptor Table \[ File descriptor ] -> open file table pointer (struct file \* is the type)
Open file table (has the struct file's) -> pointer to the inode in the memory

## open file table

```c
struct {
	struct spinlock lock;
	struct file file[NFILE];
} ftable;
```


each process has something like this,

```c
struct proc {
	...
	struct file *ofile[NOFILE];
	...
}
```

So, for a process,
```
ofile[0]   --> struct file *
ofile[1]   --> struct file *
ofile[2]   --> struct file *
```

## Shared file entries in the OFT

in `fork()`, both have the same file entry.

```c
int dup(int oldfd);
int dup2(int oldfd, int newfd);
```

dup() basically allocates a new FD that refers to the same open file description as the descriptor oldfd.

on success, it returns, 0, otherwise, -1

## Write immediately to disk

fsync()
```c
#include <unistd.h>
int fsync(int fd);
```

flushes all the modified data of the file (modified buffer cache pages) referred to by the fd to the disk device.

The call blocks until the device reports that the transfer is complete.

On success, 0, otherwise -1

if we don't use O_SYNC, then, we'll use a write(), followed immediately by fsync()

## Renaming files

```c
#include <stdio.h>
int rename(const char *oldpath, const char *newpath);
```

atomic operation.

On success, 0, otherwise -1

## Getting file info

stat()
We get the device id, inode number, protection, number of hard links, user id of owner, group id of owner, device if (if special file), total size, block size, number of blocks allocated, time of last access, time of last modification, and time of last status change.

## Creating directories - int mkdir(const char \*pathname, mode_t mode);

Permission = 0777 (in octal)
=111111111 in binary
(user, group, other)
(read, write, execute)

umask - mode is the default permission for creating files
we have to use chmod to alter perms

ls -al = --all (all files), long listing format (all the details)

Also, it makes 2 entries, '.' and '..'
like it hardlinks them properly to the things

continue from page 23/28

## Reading directories (Still sort of confused)
opendir(), readdir(), closedir() 



## rmdir()
int rmdir(const char \*pathname)l;
deletes a dir, which must be empty, except for '.' and '..'

## Hardlink and softlink
stuff....too lazy to type it now
hardlink = same inode (also, if the new path exists, it won't be overwritten)...softlink is a file itself, and points to the file it wants to link to.

hardlink is also a file, but its directory entry points to the same inode, while a softlink literally stores the path to the file, which has the inode in its directory entry.

## Memory Mapping (mmap) need to go through this properly

Maps the memory to a particular process or a file.