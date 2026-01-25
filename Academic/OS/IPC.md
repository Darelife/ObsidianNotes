[[CV's and Semaphores]]
# IPC Mechanisms

1. Unix Domain Sockets
2. Message Queues
3. Pipes
4. Shared Memory: threads, and libs
5. Signal: basically an intr

## Message Queues

Internal linked list within the kernels addressing space basically. Messages can be sent to the queue in order and retrieved from the queue in several different ways.

Each message queue is uniquely identified by an IPC identifier. Open connection a message queue identified by a "key" get a handle. Sender opens connection to message queue, and sends the message. Receiver opens connection to the message queue, and retrieves the message later on.

Message buffered within message queue / mailbox until retrieved by receiver

`msgget()` : to create the message queue
`msgsend()` : to send the message in the queue
`msgrcv()` : to receive the message
`msgctl()` : to delete the message queue

Active Queues in the kernel : `ipcs -q`

> [!question] How can we extend the message queue example so that the receiver sends a reply back to the sender through the same queue (full duplex)??
> 
> Clients send a request message with a fixed mtype (like 1), and include their PID in the message body. The server reads the requests, processes them, and replies by sending a message with mtype = client_pid. The client then reads a message whose mtype == its PID. Mtype is absolutely mandatory in System V message queues.

> [!question] How can you implement a multi client version, where several senders can talk to one receiver and each gets its own reply (using their PID as a message type)?
> 
> Just like the last question, the server replies with mtype = client_pid. This allows many clients to share 1 queue safely. Each client blocks on msgrcv(..., mtype=its_pid,...) and gets only messages intended for it.

## Shared Memory

In Message Queues, Pipes, Sockets, we basically have to copy the data from the sender process address space to the kernel address space, and then, from it to the reader process address space.

In shared memory, both processes can access the memory directly, making it the faster form of IPC.

The shared memory segment is mapped to the address space of both processes. As soon as the first process writes data in the shared memory segment, it becomes available to the second process.

Active Shared Memory segments : `ipcs -m`

> [!question] How can you include synchronization in reader-writer example of shared memory? (for example using semaphores so writer and reader coordinate safely)
> 
> sem_writer (initial value 1) -> writer may write
> sem_reader (initial value 0) -> reader waits for data
> 
> writer : sem_wait(sem_writer); write(); sem_post(sem_reader);
> reader: sem_wait(sem_reader); read_from_shm(); sem_post(sem_writer);


## Pipes

Pipes are unidirectional FIFO channels into which bytes are written at one end, read from the other end. The sys call `pipe` creates a pipe channel, with 2 file descriptors for endpoints, and it returns 2 integers,
1. One FD used to write into the pipe
2. Another FD to read from the pipe

Data written into pipes are stores in a buffer of the pipe channel until read.

Bi directional communication needs 2 pipes

### Anonymous Pipes

Anon pipes only available for use within related processes - process and its children. Open the pipe before the fork, so pipe FDs shared between parent and child point to the same pipe structure.

One of the parent/child closes the read end, and the other closes the write end.

### Pipes in shell commands

How does the shell run commands with pipes (output of one command given as input to another command)??

`cat failures.txt | grep "Prakhar"`

Shell opens a pipe, shared with child processes that run commands

Shell duplicates stdout of first child to write end of pipe, and the read end of the pipe to stdin of the second process. 

```
FD TABLE (cat child after fork)

fd 0 → OFT T   (stdin = terminal)
fd 1 → OFT T   (stdout = terminal)
fd 2 → OFT T   (stderr = terminal)
```

```
OFT:
T: terminal device (refcount = many)

```

1. The shell creates the pipe
```c
int p[2];
pipe(p); // p[0] = read end, p[1] = write end of pipe
```

```
fd 0 → OFT entry: terminal (vnode for pty)
fd 1 → OFT entry: terminal
fd 2 → OFT entry: terminal
fd p[0] → OFT entry A (pipe read end)
fd p[1] → OFT entry B (pipe write end)
```

2. Shell forks the first child (for `cat "failures.txt"` in the example)


```FD-Table
fd 0 (stdin)  → terminal (pty)
fd 1 (stdout) → pipe's write end
fd 2 (stderr) → terminal (pty)
```

```OpenFileTable
T: terminal       (refcount++)
A: pipe read end  (refcount = 2)  # parent + child
B: pipe write end (refcount = 2)  # parent + child
```


```c
dup2(p[1], STDOUT_FILENO)
```
dup2(old fd, new fd) :- closes the new fd, duplicates the old fd, and new fd is assigned to refer to old fd. On success, returns the value of new fd. FDs manipulated by dup2() remain open and **unchanged after a call to exec(). The OS gets rid of the code, data, heap, and stack, but retains the existing processor attributes, including the FD table.**

```FD-Table
fd 0 → OFT T
fd 1 → OFT B      # stdout now = pipe write end
fd 2 → OFT T
p[0] → OFT A
p[1] → OFT B
```

```OFT
T: terminal       (refcount--)
A: pipe read end  (refcount = 2)
B: pipe write end (refcount = 3)   # fd1 + child’s p[1] + parent’s p[1]
```

Then, it does exec() , and runs `cat`

```c
close(p[0]);
close(p[1]);
```

```FD Table
fd 0 → OFT T
fd 1 → OFT B      # still open! (via dup2)
fd 2 → OFT T
# p[0] closed
# p[1] closed
```

```OFT
T: terminal       (unchanged)
A: pipe read end  (refcount = 1)   # grep child still holds stdin = A
B: pipe write end (refcount = 2)   # fd1 in cat + parent's p[1]
```


3. Shell forks the 2nd child (for the grep part)

```FD-Table
FD TABLE (grep child right after fork)

fd 0 → OFT T      (stdin = terminal)
fd 1 → OFT T      (stdout = terminal)
fd 2 → OFT T
p[0] → OFT A      (pipe read end)
p[1] → OFT B      (pipe write end)

```

```OFT
T: terminal
A: pipe read end  (refcount = 2)   # parent + grep child
B: pipe write end (refcount = 2)   # parent only, cat closed its copy
```

```c
dup2(p[0], STDIN_FILENO);
```
it does it to read the input from the pipe's read end to stdin. Stdin's fd points to p\[0] basically now. 

```FD-Table
fd 0 → OFT A      # now comes from the pipe
fd 1 → OFT T
fd 2 → OFT T
p[0] → OFT A
p[1] → OFT B
```

```OFT
T: terminal
A: pipe read end  (refcount = 3)   # fd0 + p[0] + parent's p[0]
B: pipe write end (refcount = 2)   # parent’s p[1] + cat fd1
```

```c
close(p[0]);   // duplicates removed, but fd0 still points to A
close(p[1]);   // grep does NOT write into pipe
```

```FD-Table
fd 0 → OFT A    # stdin from pipe
fd 1 → OFT T
fd 2 → OFT T
# p[0] closed
# p[1] closed
```

```OFT
A: pipe read end  (refcount = 1)   
    # only fd 0 of grep

B: pipe write end (refcount = 1)
    # only fd 1 of cat
```

```steps
cat stdout → write end of pipe (B) → pipe buffer → read end (A) → grep stdin
```



The parent shell still has this
```FD-Table
p[0] → OFT A
p[1] → OFT B
```
So, in the parent shell, we do this
```c
close(p[0]);
close(p[1]);
```

```OFT
A refcount--  → 0? No, grep still holds fd0 (refcount = 1)
B refcount--  → 1 (cat still holds fd1)
```

```OFT
A: pipe read end   (refcount = 1)  # grep
B: pipe write end  (refcount = 1)  # cat
```


# Named Pipes

- Between unrelated procs
- Opened with a pathname 
- One proc access the read end, and another accesses the write end
- Uni directional 
- Error if you write to a pipe with no reader

```c
// reader
mkfifo(name, ...)
fd = open(name, O_RDONLY)
read(fd, message, ...)

// writer
fd = open(name, O_WRONLY)
write(fd, message, ...)
```

- fifo
- mkfifo (path, mode)
- data flows in order
- any proc can access it
Build a bi-directional chat system using two named pipes (FIFOs), so two
unrelated processes can send and receive messages in real time — just like a
mini terminal chat!

## Blocking vs Non blocking IPC

Send / Receive sys calls can block
- the proc waits until the op completes
- sender can block if the temp buffer is full
- rec can block if the temp buffer is empty

Can be configured to be non blocking
- send/rec will give error instead of blocking
- proc doesn't wait...instantly returns even if the op couldn't complete
- Example: The server can handle many connections. If one client is slow, it doesn’t block others

## Signals

- simplest to send notifs
- are lightweight notifs sent from 1 proc to another. Don't carry data, but can alert or synch procs
- they are like the interrupts for user procs
- one proc can send a signal like kill(pid, signal_number)
- another proc can catch that signal via a signal handler, through signal(), or sigaction()

## Process groups

When you send ctrl+c, the terminal sends sigint to the entire foreground proc group, and not just to 1 proc.
`cat failure.txt | grep prakhar | sort`
All of them have diff process ids, but the same process group id.

[[File System]]