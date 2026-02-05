A file system is mostly 3 things.
1. Metadata structures
2. Data block management
3. Directory / Index structures

We mainly use arrays of blocks, and Tree based structures. 

How does the File system map the calls made by a process such as open(), read(), write(), etc onto its structures? -> access methods

Access Methods = the algorithms + steps the file system uses to access and manipulate files.
They answer:
- How do I find a file by name?
- How do I translate file offset → disk block?
- How do I allocate blocks?
- How do I read/write those blocks efficiently?
- What metadata do I need to update?

## Overall Organization

1. Divide the disk into blocks.
	- Simple file system has only 1 block size (Eg: 4KB)
	- In a small disk, 64 blocks = 256 KB
2. Most of the blocks should be used to store user data (data region)...eg out 64, reserve 56 as data blocks.
3. Reserve space on the disk for the inode table...eg 5 blocks for inodes. Inodes aren't usually large. Eg: 128 or 256 bytes. 
	- Assuming 256 bytes per inode, a 4KB block can hold 16 inodes. Our file system contains 80 total inodes (5\*16 = 80)
		- Represents the maximum number of files we can have in our filesystem. The same filesystem on a larger disk, could simply allocate a larger inode table, and thus accommodate more files.
4. We also reserve some space for allocation data structures that store which inodes / datablocks are free.
	- Free List : points to the first free block, which then points to the next free block, and so on.
	- Bitmap: Each bit is used to indicate whether the corresponding object/block is free (0) or in use (1).
	- We need one for the data region (data bitmap), and another for the inode table (inode bitmap)
5. The first block is reserved to contain information of the file system - Superblock
	- Stores the number of inodes (80 here), and data blocks(56 here)
	- Where the inode table begins (index 3)
	- Magic Number to denote the file system type (In this case, vsfs - very simple file system)


Below is a **clean, side-by-side comparison** of the three ratios you’re asking about.

**Interpretation of ratios (FS : Device)**

* **1 : 8** → FS block **512 B**, device sector **4 KB**
* **1 : 1** → FS block **4 KB**, device sector **4 KB**
* **8 : 1** → FS block **32 KB**, device sector **4 KB**

---

# FS block size vs Device sector size

| Aspect                  | **1 : 8** (512 B FS / 4 KB device) | **1 : 1** (4 KB / 4 KB) | **8 : 1** (32 KB / 4 KB) |
| ----------------------- | ---------------------------------- | ----------------------- | ------------------------ |
| Alignment               | ❌ Misaligned                       | ✅ Perfect               | ✅ Aligned (multiple)     |
| Smallest physical I/O   | 4 KB                               | 4 KB                    | 32 KB (logical)          |
| Read-Modify-Write       | ❌ Frequent                         | ✅ None                  | ❌ On partial writes      |
| Random write latency    | ❌ Worst                            | ✅ Best                  | ⚠️ Moderate              |
| Sequential throughput   | ⚠️ OK                              | ✅ Excellent             | ✅ Excellent              |
| Small-file space usage  | ✅ Best                             | ⚠️ Moderate             | ❌ Worst                  |
| Internal fragmentation  | ✅ Minimal                          | ⚠️ Moderate             | ❌ High                   |
| Metadata overhead       | ❌ Very high                        | ✅ Low                   | ✅ Very low               |
| Page-cache efficiency   | ❌ Poor                             | ✅ Best                  | ⚠️ Mixed                 |
| SSD write amplification | ❌ Very high                        | ✅ Lowest                | ⚠️ Medium                |
| HDD seek efficiency     | ❌ Poor                             | ✅ Good                  | ✅ Very good              |
| fsck / recovery time    | ❌ Slow                             | ✅ Normal                | ✅ Fast                   |
| Typical real usage      | ❌ Obsolete                         | ✅ Standard              | ⚠️ Niche                 |

---

## 🔍 What each ratio is *good* for

### **1 : 8 (512 B FS / 4 KB device)**

**Pros**

* Excellent space efficiency for tiny files
* Fine-grained allocation

**Cons**

* Heavy read-modify-write
* Terrible for SSDs
* High metadata and CPU overhead

🚫 **Almost never used today**

---

### **1 : 1 (4 KB FS / 4 KB device)**

**Pros**

* Perfect alignment across CPU, OS, FS, and storage
* Best overall performance
* Lowest write amplification
* Balanced for all workloads

**Cons**

* Some wasted space for small files

✅ **Default choice for almost all systems**

---

### **8 : 1 (32 KB FS / 4 KB device)**

**Pros**

* Very high sequential throughput
* Fewer metadata lookups
* Great for large, streaming files

**Cons**

* Large internal fragmentation
* Poor small-file efficiency
* Partial writes cause RMW

⚠️ **Used in specialized workloads**

* Media storage
* Backups
* Data lakes
* Some DB / log-structured designs

---

## 🧠 Mental model (easy to remember)

```
Too small  → RMW + overhead
Just right → alignment heaven
Too large  → fragmentation + partial-write pain
```

---

## 🏁 Final takeaway

| Ratio     | Verdict                                     |
| --------- | ------------------------------------------- |
| **1 : 8** | ❌ Avoid                                     |
| **1 : 1** | ⭐ Optimal default                           |
| **8 : 1** | ⚠️ Use only if workload is large-file heavy |

If you want, next we can:

* plug in **real numbers** (IOPS, latency)
* show **SSD erase-block interactions**
* map this to **databases (8K/16K pages)**

Just say.


# Making a file system

```bash
mkfs [options] [-t type] [fs-options] device [size]
```

device = /dev/sda5
size = number of blocks
type = type of filesystem (default = ext2)

Just writes an empty file system, with a root dir, onto that file system.

> [!info]
> for partitioning, use fstab

## Mounting a file system

Once the FS is made, it needs to be accessible within the uniform file system tree.

mount()
- takes an existing dir as the target mound, and essentially pastes a new FS onto the directory tree at that point
- Eg: unmounted ext3 FS, stores in device partition /dev/sda1, contents: root dir, with a and b dir, and b having foo 
  /root
  |-/a
  |-/b
    |- foo

- mount -t ext3 /dev/sda1 /home/users
- ls /home/users
  `a b`

Mount sort of fakes it..like, we mount a file system onto a directory. We often use it to access a usb or something...it gets mounted to /dev/sdb/ or something like that...the OS basically does abstraction. It pretends that it's there in the file system, whereas actually when we try to access the files in it, it redirects all the accesses to the directory to the new file system, and hides the original directory contents. You now see the mounted FS at that path.

## Inode - Index Node

- Each inode is implicitly referred to by a number, i-number.
- sizeof(inode) = 256 bytes
- inodeStartAddr = 12KB
- To go to inode 32, inodeStartAddr + (32\*sizeof(inode)) = 20KB
- Sector number on disk: (Assume sector size = 512 bytes)
  = (20 \* 1024) / 512 = 40

Inode is divided into a bunch of blocks
- Blk = (inumber * sizeof(inode_t))/blockSize
   gives the block number within the inode table (so, doesn't need the inodeStartAddr)
- sector = ((blk \* blockSize) + inodeStartAddr) / sectorSize
  uses the exact addr of the inode within the disk...so, we need the inodeStartAddr too....also, none of the blocks get split between diff sectors...so like, we can directly do it with the inode block number and the inode start addr to get the addr, and then the sector size to get the sector number.

## Inode pointing to data blocks

1. Direct Pointers
	- Each pointer refers to one disk block that belongs to the file
	- Such an approach is limited to small files
2. Multi Level indexing
	- Single indirect pointer - Instead of pointing to a block that contains the user data, it points to a block that contains more pointers, each of which point to user data....similarly double and triple indirect pointers

inodes store 12 direct block addresses...allows small files (<48KB) to be very fast.

if the file grows larger, inode -> \[block full of pointers] -> \[data blocks]...one 4kb block can store 4096/8 = 512 pointers....growing further, we can also have double indirect, and then triple indirect

> [!question] Suppose in a FS, block addr is 64 bit, and each block is of size 4KB. An inode contains 12 direct block addrs, 1 indirect block addr,  1 double indirect block addr, and 1 triple indirect pointer. What's the max file size??
> 
> 
> Lets just count the number of pointers, and later multiply it with the block size for the total file size.
> 
> For direct block addresses, it's 1 pointer each...so, 12 pointers.
> 
> So, for the indirect blocks,
> Number of pointers in each block = 4KB/64 bits = 4KB/8 bytes = 4\*2^10 / 8 = 2^9 = 512 
> 
> For double indirect block addresses, 512\*512 pointers,
> and for triple, it's 512\*512\*512
> 
> So, number of pointers = 134480396
> Max file size = 4KB \* 134480396 = 500.97 GB

# Inode Pointers vs Extents vs Linked Based Approach

**Pointer Based Approaches**
- Most flexible
- use a large amount of meta data per file (especially for large files)

**Extent Based Approaches (Extent stores the start)**
- Less flexible
- More Compact
- Work well when there's enough free space on the disk, and files can be laid out continuously.
- basically like a linked list, but in it, each pointer node, has a range of addresses (like, each extent pointer node stores the start, and the length of the range)

**In linked based approach (FAT uses this)**
Now, inside an inode, 1 pointer points to the first block of the file, and another pointer is at the end of that data block (to handle larger files). To access the last data block, we have to go through all the blocks. We'll have an in memory

It stores a large table (array) in the disk, that gets loaded into the memory on boot / mount....and then like, it just stores the next pointer. Like, 100 -> 250, 250 -> 300, 300 -> -1

It points to the next block. -1 could mean the end of the file. There could also be some other marker to indicate that a particular block is free. 

Also, this faster than reading from the disk. Like, instead of storing the next block at the end of the current block in the disk, we can just store it in the table, and since the memory is faster, we can essentially have a random access directly, by going through the memory super quick, and finding the 500th block or whatever...like the last block and then, directly go to that block in the disk. So, we reduce the number of disk I/O operations significantly, from 500 or which ever block we wanted to visit to 1.

There's a FAT table, (there are no inodes...block 0 points to the root...then go through the file structure to find the file you want to read)
The table is indexed by the address of a data block D. The table maps it to the next data block D'. (-1) indicates the EOF. Could be some other marker to show empty data blocks.

## Directory Organization

Dirs are themselves special types of files. The data block of a directory contains filename, inode number, record length, strlen. On deletion, the reclen still stays, and it ensures that a new smaller file name can reuse the entry...as it's basically a dynamic table, or it could also be a linked list, or a B-Tree (in XFS).

> [!info] Reserved INODE numbers
inode 0 : NULL block / a deleted dir, which hasn't be removed from the FS
inode 1 : BAD / Corrupted block
inode 2 : root

## Free Space Management

How to track which inodes and data blocks are free?
1. Bitmaps
	- Stores 1 bit per block....when getting free data blocks, could check for n contiguous free blocks, so that the file could be allocated continuously.
2. Free List
	- Superblock stores pointer to the first free block, and the free block stores the address of the next block on the list. When a block is needed, the head block is used and the list is updated accordingly.
3. Complex Data Structures
	- XFS uses B trees for free blocks

## Reading a file

- A user wants to read /foo/bar. The size of the file is 12KB = 3 blocks. open(), read(), and close()
- **open("foo/bar", O_RDONLY)**
	- Needs to find the inode for the file bar.
	- The file system must traverse the pathname and thus locate the desired inode.
	- Firstly FS will read from disk the inode of the root directory. (predefined to 2 on must Unix file systems. Inode 0 is used as NULL value to indicate that there's no inode. Inode 1 is used to keep track of bad blocks on the disk. It's essentially a hidden file containing the bad blocks)
	- Look inside the data blocks pointed by inode number 2, and search for foo's inode number.
	- Recursively read data block pointed by foo's inode to get the inode number of bar.
	- Final step: Read bar's inode into memory. FS does the final permission check, and allocates a file descriptor for this process in the open file table.
	  
- **read(fd, buf, count)**
	- The first read (at offset 0 unless lseek() has been called) with read in the first block of the file. The first data block can be found in the inode. Updates the inode with a new last accessed time. Updates the in memory open file table for this file descriptor (updates the file offset such that the next read will read the 2nd file block). This continues till all the blocks are read.
	  
- **close(fd)**
	- deallocates the fd


## Writing to a file

open() follows the same steps
write() may allocate a new block unless existing data blocks are being overwritten. Each write to a new block generates 5 I/O's
1. Read the data bitmap to find a new data block
2. Write the data bitmap to update the state
3. Read the inode to get the list of data blocks
4. Write the inode with the new block and update the metadata
5. Write to the actual block itself.

## Creating a file

When an open(O_CREAT) is called
1. Read the inode bitmap (to find a free inode)
2. Write to the inode bitmap (to mark it as allocated)
3. Write to the new inode to initialize it
4. Read the directory inode
5. Write to the data of the directory (to link the high level name of the file to its inode number)
6. Write to the directory inode to update the metadata.

If the directory needs to grow to update accommodate the new entry, additional I/O's will be needed as well.


For creating, first the root inode is read, then, the root data is read, then the directory inode is read, followed by the directory data. Then, the inode bitmap is read and is written into. Then, the directory data is updated to add the new inode for the new file, along with the file name. Then, the file inode is read, and written into (update the meta data)



# MMAP (from chatgpt)
# mmap() — Explanation

## What `mmap()` Does (Conceptually)

`mmap()` maps a file or a region of anonymous memory into a process’s virtual address space.
The kernel returns a pointer to this region, and pages are loaded lazily on demand.

---

## Syntax

void *mmap(
    void *addr,
    size_t length,
    int prot,
    int flags,
    int fd,
    off_t offset
);

### 1. `addr`
Usually `NULL`. Lets the kernel choose the address.

### 2. `length`
Size of the region. Rounded to the page size.

### 3. `prot` (memory protection)
- PROT_READ
- PROT_WRITE
- PROT_EXEC
- PROT_NONE

Example: PROT_READ | PROT_WRITE

### 4. `flags` (type of mapping)
Common flags:

- MAP_PRIVATE — copy-on-write; modifications not written to the file.
- MAP_SHARED — modifications are visible to other processes and written back.
- MAP_ANONYMOUS — not backed by a file.
- MAP_FIXED — map at exactly the given address (rarely used).

### 5. `fd`
File descriptor if mapping a file.
Use -1 when using MAP_ANONYMOUS.

### 6. `offset`
Offset in the file. Must be page-aligned (e.g., multiple of 4096).

---

## How `mmap()` Works Internally

### Step 1: The kernel creates a VMA
A Virtual Memory Area is inserted into the process's memory map.
No physical memory is allocated yet.

### Step 2: Lazy loading / page faults
On first access:
- A page fault occurs.
- The kernel loads the corresponding page from disk (if file-backed).
- The page table is updated.

### Step 3: Behavior depends on the mapping
- MAP_SHARED writes propagate to the underlying file.
- MAP_PRIVATE uses copy-on-write, so the file is unchanged.

---

## Why `mmap()` Is Useful

- Allows file access via memory instead of read/write.
- Efficient for large files due to lazy paging.
- Enables shared memory between processes.
- Used internally by malloc() for large allocations.

---

## Example 1: Shared Memory Between Processes

```c
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    int *x = mmap(NULL, sizeof(int),
                  PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS,
                  -1, 0);

    *x = 10;
    if (fork() == 0) {
        *x = 20; // child writes
        return 0;
    }
    sleep(1);
    printf("%d\n", *x); // parent sees 20
}```

---

## Example 2: Mapping a File

```c
int fd = open("data.bin", O_RDONLY);

char *buf = mmap(NULL, filesize,
                 PROT_READ,
                 MAP_PRIVATE,
                 fd, 0);

printf("first byte = %c\n", buf[0]);
```
---

## Example 3: Shared Semaphore with mmap

```c
sem_t *s = mmap(NULL, sizeof(sem_t),
                PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS,
                -1, 0);

sem_init(s, 1, 1);   // shared between processes
```
