# Operating Systems — Placement Prep

Consolidated from: [[Academic/OS/Introduction]], [[Processes]], [[Scheduling]], [[Threads]], [[Locks]], [[CV's and Semaphores]], [[IPC]], [[Virtual Memory and Paging]], [[File System]], [[Cheat Sheet]]

## Table of Contents
1. [System Basics — Compilation, Memory, Modes, Booting](#1-system-basics)
2. [Processes](#2-processes)
3. [Traps, System Calls & Context Switching](#3-traps-system-calls--context-switching)
4. [CPU Scheduling](#4-cpu-scheduling)
5. [Threads & Concurrency](#5-threads--concurrency)
6. [Synchronization: Locks](#6-synchronization-locks)
7. [Condition Variables & Semaphores](#7-condition-variables--semaphores)
8. [Inter-Process Communication (IPC)](#8-inter-process-communication-ipc)
9. [Virtual Memory & Paging](#9-virtual-memory--paging)
10. [File Systems](#10-file-systems)
11. [Deadlocks](#11-deadlocks)
12. [Memory Management — Additional Concepts](#12-memory-management--additional-concepts)
13. [Scheduling & OS Structure — Additional Concepts](#13-scheduling--os-structure--additional-concepts)
14. [Synchronization — Classic Problems & Critical Section Theory](#14-synchronization--classic-problems--critical-section-theory)
15. [I/O, Devices, Disks & Storage](#15-io-devices-disks--storage)
16. [Quick-Fire Interview Q&A](#16-quick-fire-interview-qa)

---

## 1. System Basics

### Compilation Pipeline
```
hello.c --[Preprocessor]--> hello.i --[Compiler]--> hello.s --[Assembler]--> hello.o --[Linker (+ other .o files)]--> hello (executable)
```
- Preprocessor: expands macros/headers
- Compiler: generates assembly
- Assembler: generates relocatable machine code (object file)
- Linker: combines object files (incl. libraries like printf.o) into final executable

### Process Memory Layout (low → high address)
```
Code (.text)   — instructions
Data (.data)   — initialized global/static vars
BSS (.bss)     — uninitialized global/static vars
Heap           — grows upward (malloc)
   ...gap...
Stack          — grows downward (locals, return addrs)
```

### CPU Modes / Rings
- 4 rings (0–3). **User mode = ring 3** (least privileged), **Kernel mode = ring 0**. Rings 1–2 used for device drivers.
- Ring number is stored in the **bottom 2 bits of the CS (code segment) register**.
- 3 triggers for user → kernel mode switch:
  1. **System calls** — user requests OS service
  2. **Interrupts** — external events (device signals CPU)
  3. **Program faults/exceptions** — errors needing OS attention (div by 0, page fault)

### POSIX & ABI
- **POSIX**: standard set of syscalls/C-lib functions for portability across OSes (recompilation still needed per architecture).
- **ABI** (Application Binary Interface): the compiled/binary-level equivalent of an API — defines calling conventions (how args are passed via registers/stack), object layout, symbol resolution at the machine-code level. API = source-level contract, ABI = binary-level contract.

### Booting Sequence
1. **Firmware** (BIOS or UEFI) executes first, from ROM/flash memory at a fixed address.
   - **BIOS**: POST (Power On Self Test), detects hardware, sets clock speed.
   - **UEFI**: modern replacement, backward compatible via CSM, uses **GPT** partitioning (vs BIOS's **MBR**).
   - MBR: first 512 bytes of disk, ≤4 primary partitions, ≤2TB/partition. GPT: up to 8 ZiB disks.
2. Bootloader loads OS kernel from disk into memory, jumps to first OS instruction.
3. OS sets up: stack, **IDT** (Interrupt Descriptor Table), virtual memory, scheduler, filesystem.
4. Launches user-space services; kernel modules loaded on-demand via `modprobe`.
5. **init** (PID 1) is spawned — the ancestor of every process. System shuts down when init exits.

---

## 2. Processes

A **process** = a running program. OS allocates memory, initializes CPU context, runs it in **user mode** until a syscall/interrupt/fault triggers a switch to **kernel mode**.

> Each process has: PID, code+data (from executable), stack+heap (runtime). The CPU register context is saved to memory when paused, restored on resume.

### Process States
- **Ready** — runnable, not currently scheduled
- **Running** — executing on a CPU core
- **Blocked/Sleeping/Suspended** — waiting on an event (I/O etc); context saved
- **Zombie** — exited but not yet reaped by parent (exit code still needed)

### Process-Related System Calls

| Call | Purpose |
|---|---|
| `fork()` | creates a child process (copy of parent's memory image) |
| `exec()` | replaces the calling process's memory image with a new executable |
| `exit()` | terminates a process (does **not** free memory — parent must reap) |
| `wait()` | blocks parent until a child terminates; reaps child's memory |

**fork()**
- Returns `0` in the child, child's PID in the parent.
- Parent and child are independent after fork (COW in practice) — changes in one don't affect the other.
- `n` forks → `2^n` total processes.

**exit() / wait()**
- `exit()` leaves the process as a **zombie** (memory not freed) so the parent can inspect the exit code.
- `wait()` reaps a zombie child; blocks if child still running, returns immediately if child already terminated, returns immediately (no-op) if no children exist.
- `waitpid()` — reap a *specific* child by PID.
- Parent exits before child → child becomes an **orphan**, adopted by `init`, reaped when it exits.
- Fork without eventual wait → zombies accumulate → exhausts process table / memory.

**exec()**
- Replaces code/data/heap/stack with a new program's image; code after `exec()` never runs (unless it fails).
- Variants: `execl`, `execlp`, `execle`, `execv`, `execvp`, `execvpe`.

### Shell Mechanics
- Shell = init/child of a fork chain: reads command → **forks** → child **execs** the binary → parent **waits**.
- `cd` uses `chdir()` directly on the shell process (no fork — must affect parent shell's cwd).
- `&` suffix → background execution, shell doesn't wait immediately (reaped later via `waitpid()` or periodic sweep).
- **I/O redirection**: child closes `STDOUT_FILENO` then `open()`s a file — file takes the freed lowest fd (STDOUT).
- **Pipes**: kernel-provided unidirectional channel; shell connects one child's stdout to another's stdin via `dup2()`. (See [[IPC]] pipe walkthrough for full FD-table trace of `cat foo | grep bar`.)

### Signals
- `kill()` → `SIGKILL`
- `Ctrl+C` → `SIGINT` (interrupt)
- `Ctrl+Z` → `SIGSTP` (stop; resume via `fg`)
- Processes catch signals via `signal()` / `sigaction()`.
- **Process groups**: `Ctrl+C` sends `SIGINT` to the entire foreground process group (all processes in a pipeline share a PGID but have distinct PIDs).

---

## 3. Traps, System Calls & Context Switching

A **trap** = controlled transfer from user mode → kernel mode. Triggered by:
- **System call** (`read()`, `write()`, explicit request)
- **Interrupt** (external device — keyboard, disk completion)
- **Exception/fault** (div-by-zero, page fault, illegal access)

> The OS is *not* a separate process — it runs inside the current process's context, in kernel mode. Process P stays "process P," just now executing OS code.

### Function Call vs System Call
- Function call: PC jumps to a **known address**, context saved on **user stack**.
- System call: can't let the user jump to arbitrary OS code (security), and can't trust the user stack (may contain malicious return addresses) → OS uses a **separate kernel stack** per process, and looks up the handler address via the **IDT**, not a direct jump.

### IDT (Interrupt Descriptor Table)
- Set up at boot, inaccessible from user mode. Maps trap/interrupt numbers → kernel handler addresses.
- Ranges: `0–31` system traps/exceptions, `31–127` device interrupts, `128` (`int $0x80`) syscall interface, `129–255` other interrupts.
- Example syscall invocation (fork, syscall #1):
```asm
movl $SYS_fork, %eax
int  $T_SYSCALL      ; software interrupt, e.g. 64
ret
```

### What Happens on a Trap
1. CPU raises privilege level (ring 3 → ring 0)
2. Stack pointer switches to the process's **kernel stack**
3. Register context (**trapframe**) pushed onto the kernel stack
4. Handler address looked up in IDT; PC jumps there
5. OS handler executes
6. **return-from-trap**: restores context, drops privilege back to ring 3, resumes user PC (OS may first decide to context-switch to a different process)

### xv6 PCB — `struct proc`
```c
struct proc {
    uint sz;                     // process memory size
    pde_t* pgdir;                // page table
    char* kstack;                // kernel stack base
    enum procstate state;        // UNUSED/EMBRYO/SLEEPING/RUNNABLE/RUNNING/ZOMBIE
    int pid;
    struct proc* parent;
    struct trapframe* tf;
    struct context* context;     // swtch() target
    void* chan;                  // sleep channel
    int killed;
    struct file* ofile[NOFILE];  // open files
    struct inode* cwd;
};
```

### Context Switch
- OS switches between processes when: current process **can't** continue (blocked on I/O, exited) or the OS **chooses** not to continue it (ran too long, fairness).
- Kernel context (where execution paused *in kernel mode*) is saved on the kernel stack, distinct from the user context (where it paused *in user mode*).
- `switch()` in xv6: saves registers of the old process into its `context` struct on its kernel stack, pops the new process's `context`, ESP moves to the new kernel stack.
- Each CPU runs a dedicated **scheduler thread** that loops over the process list picking a `RUNNABLE` process.

---

## 4. CPU Scheduling

**Goals**: maximize utilization, minimize turnaround/response time, fairness, low scheduling overhead (~1µs per switch).

- **Non-preemptive (cooperative)**: switches only on block/exit.
- **Preemptive**: timer interrupts let the OS forcibly switch even if the process wants to keep running.

| Algorithm | Preemptive? | Idea | Weakness |
|---|---|---|---|
| **FCFS/FIFO** | No | Run in arrival order | Convoy effect — short jobs stuck behind long ones |
| **SJF** | No | Run shortest known CPU burst first (min-heap of PCBs) | Still non-preemptive: a long-running job blocks a newly-arrived short job |
| **SRTF / STCF / PSJF** | Yes | Preemptive SJF — switch if a shorter job arrives | Needs burst-time prediction |
| **Round Robin** | Yes | Fixed time quantum per process, timer-enforced | Good response time, poor turnaround (repeated waiting) |
| **WFQ** | Yes | RR + per-process weight/priority; time slice ∝ weight | Real schedulers can't perfectly enforce slices (blocking, drift) — compensate excess/deficit later |
| **Linux CFS** | Yes | WFQ variant; tracks **vruntime**, always runs lowest-vruntime process | RB-tree overhead, but O(log n) |

### CFS Details
- **sched_latency** (~48ms): total period, divided among runnable processes → per-process time slice (too many processes → too many context switches).
- **min_granularity** (~6ms): floor on time slice to bound scheduling overhead.
- **niceness**: -20 (highest prio) to +19 (lowest), default 0(±10 in some tables). Weight table (nice → weight): `-20→88761 ... 0→1024 ... 10→110 ... 15→36`.
$$\text{time\_slice}_k = \frac{\text{weight}_k}{\sum_i \text{weight}_i} \cdot \text{sched\_latency}$$
- Runnable processes stored in a **red-black tree**, ordered by vruntime, O(log n) operations, only runnable procs kept in the tree (unlike scanning a flat list).

---

## 5. Threads & Concurrency

Threads = lightweight processes. `fork()` copies the whole memory image; **threads within one process share code, data, and heap, but each has its own stack, PC, and CPU register context.**

### Concurrency vs Parallelism
- **Concurrency**: rapid switching giving the *illusion* of simultaneity — useful even on 1 core (e.g., one thread blocks on I/O, another proceeds).
- **Parallelism**: true simultaneous execution, bounded by core count.

### POSIX Threads
```c
pthread_create(&t1, NULL, f1, arg);
pthread_create(&t2, NULL, f2, arg);
pthread_join(t1, NULL);   // optional — wait for thread to finish
pthread_join(t2, NULL);
```
Compile with `-lpthread`. Execution order between threads is **not guaranteed** by the scheduler.

### Race Conditions (classic example)
```c
int counter = 0;
void *incrementBy100000() { for (int i=0;i<100000;i++) counter++; }
// two threads run this concurrently
```
Naively you'd expect `200000`, but `counter++` is **not atomic** (read → increment in register → write back). A context switch between the read and write lets updates get lost → final value is **less than 200000**, and non-deterministic.

Fix: mutual exclusion via a lock:
```c
pthread_mutex_lock(&m);
counter++;
pthread_mutex_unlock(&m);
```

---

## 6. Synchronization: Locks

**Race condition**: incorrect result due to unfortunate interleaving of context switches on shared data — atomicity of an update is violated.

**Lock goals**: (1) mutual exclusion (2) fairness (3) low overhead.

### Implementation 1 — Disable Interrupts
Lock = disable interrupts; unlock = re-enable. Broken on multicore (another core can still touch the lock) and can delay critical OS work (e.g. DMA completion). Only viable for trusted kernel code.

### Implementation 2 — Naive Spin Wait (broken)
```c
void acquire_lock() { while (is_locked==1); is_locked = 1; }
void release_lock() { is_locked = 0; }
```
Not atomic — a context switch **between** the `while` check and the `is_locked=1` write lets two threads both "acquire" the lock. Also wastes CPU cycles.

### Implementation 3 — Test-And-Set (atomic HW instruction)
Atomically writes a new value and returns the old value in one step (x86: locked `xchg`; SPARC: `ldstub`).
```c
void lock(lock_t *lock)   { while (TestAndSet(&lock->flag, 1) == 1); }
void unlock(lock_t *lock) { lock->flag = 0; }
```

### Implementation 4 — Compare-And-Swap (CAS)
Takes (address, expected old value, new value); writes new value only if current == expected, always returns old value.
```c
void lock(lock_t *lock) { while (CompareAndSwap(&lock->flag, 0, 1) == 1); }
```

### Spin + Yield / Two-Phase Locks
Pure spinning wastes CPU if the lock-holder is context-switched out. Improve with `yield()`:
```c
void lock() { while (TestAndSet(&flag, 1) == 1) yield(); }
```
**Two-phase locking**: spin briefly (Phase 1), then fall back to a sleeping mutex (Phase 2) — hybrid is almost always best.
- **Spinlock** better when critical sections are **short** (avoids context-switch overhead).
- **Sleeping mutex** better when critical sections are **long** (frees the CPU for other work).
- Linux's real mutexes are **futex-based**, with a built-in wait **queue** for fairness (naive TAS/CAS loops are *not* fair).

### Locking Discipline
1. Protect each data structure with (at least) one lock.
2. Coarse-grained (one big lock) vs fine-grained (many small locks) — tradeoff between simplicity and concurrency.
3. Lock both reads and writes of shared state.

### Thread-Safe Counter Pattern
```c
typedef struct { int value; pthread_mutex_t lock; } counter_t;
void increment(counter_t *c){ pthread_mutex_lock(&c->lock); c->value++; pthread_mutex_unlock(&c->lock); }
```

---

## 7. Condition Variables & Semaphores

### Why Not Just a Waiting Queue?
Naively blocking T2 by putting it in T1's lock's wait queue is a poor design — it conflates a mutex (for critical sections) with signaling, and could block unrelated threads using T1.

### Condition Variables (CV)
An explicit queue threads join via `wait(cv, mutex)` when a desired state isn't true yet; woken by `signal(cv)` / `broadcast(cv)`.

**Why wait/signal must be atomic w.r.t. a mutex** — without locking, a context switch right before `wait(cv)` (after checking the condition) can cause a **lost wakeup**: the signal fires into an empty queue and the waiter sleeps forever.

Correct pattern:
```c
// Waiter
lock(mutex);
if (!done) wait(cv, mutex);   // wait() atomically releases mutex, reacquires on wakeup
unlock(mutex);

// Signaler
lock(mutex);
done = true;
signal(cv);
unlock(mutex);
```
This is **Mesa-style** (signal ≠ guaranteed immediate run — recheck the condition after waking, hence always use `while`, not `if`), as opposed to Hoare-style.

### Producer–Consumer (Bounded Buffer) — Correct Solution
Two mistakes in naive versions: (1) using `if` instead of `while` around the wait (state can change again before the waiter actually runs); (2) using one CV for both roles instead of two independent ones.
```c
cond_t empty, fill;
mutex_t mutex;

void *producer() {
    lock(mutex);
    while (count == MAX) wait(&empty, &mutex);
    put(i);
    signal(&fill);
    unlock(mutex);
}
void *consumer() {
    lock(mutex);
    while (count == 0) wait(&fill, &mutex);
    get();
    signal(&empty);
    unlock(mutex);
}
```
`signal()` wakes **one** waiter; `broadcast()` wakes **all** (all recheck condition — can cost performance).

### Semaphores

- Integer counter + two ops: **wait/down(P)** and **signal/up(V)**.
```
wait(S):   S--; if (S < 0) block
signal(S): S++; if (S <= 0) wake one blocked thread
```
- If `S < 0`, `|S|` = number of threads currently blocked.
- If `S > 0`: free resources available. `S == 0`: none free, none waiting. `S < 0`: threads waiting.

**As a binary lock**: init to **1**.
```c
sem_wait(&m); /* critical section */ sem_post(&m);
```

**For ordering (parent waits for child)**: init to **0** — nothing to "give away" until the child actually finishes.
```c
sem_init(&s, 0, 0);
// child: work(); sem_post(&s);
// parent: sem_wait(&s);  // blocks until child posts
```

**Mutual waiting / rendezvous (T1 ↔ T2 both finish step 1 before either starts step 2)**: always **up before down** on each side, never down-then-up (down-first deadlocks since nothing can ever post first).
```c
// T1                          // T2
up(T1done); down(T2done);      up(T2done); down(T1done);
```

**N-thread barrier** (generalizes the above):
```c
sem_init(&mutex, 0, 1);
sem_init(&barrier, 0, 0);
void barrier_point() {
    sem_wait(&mutex);
    count++;
    if (count == N) for (int i=0;i<N;i++) sem_post(&barrier);
    sem_post(&mutex);
    sem_wait(&barrier);   // proceeds once opened
}
```

**Producer–Consumer with semaphores** — needs *three* semaphores, not two, and careful **ordering** to avoid deadlock:
```c
// Producer                    // Consumer
down(sem_empty);                down(sem_filled);
down(mutex);                    down(mutex);
produce();                      consume();
up(mutex);                      up(mutex);
up(sem_filled);                 up(sem_empty);
```
Locking `mutex` **before** `sem_empty`/`sem_filled` is a bug — it can deadlock (mutex held while blocked on the resource semaphore, and the only thread that could post it is locked out by the same mutex). Resource semaphores must be acquired **before** the mutex.

### Semaphore vs Condition Variable

| | Semaphore | Condition Variable |
|---|---|---|
| Integer counter | Yes | No |
| Works standalone | Yes | No — needs a mutex |
| Resource counting | Yes | No |
| Waiter rechecks condition | No (built-in) | Yes (always loop) |
| Has broadcast equivalent | No | Yes |
| Typical use | producer-consumer, resource pools | "wait for state change" |

### Batch Processing (wait for N requests, then process together)
Two CV version (`cv_request`, `cv_batch_processor`) or semaphore version — see [[CV's and Semaphores]] for the full "wake one, let it wake the next" chained-semaphore trick (avoids an explicit loop of N `up()`s in the batch processor).

### Deadlock Sketch
T1 holds L1, wants L2. T2 holds L2, wants L1. Neither can proceed → **deadlock**. (Classic circular-wait; avoid via consistent lock ordering — see [[Locks]]/[[CV's and Semaphores]].)

---

## 8. Inter-Process Communication (IPC)

Five mechanisms: Unix domain sockets, **message queues**, **pipes**, **shared memory**, **signals**.

### Message-Passing vs Shared-Memory IPC
- **Message passing** (pipes, message queues, sockets): every transfer goes *through the kernel* — safer/simpler (kernel enforces synchronization and copies data), but slower (data is copied kernel↔user, sometimes twice: sender→kernel→receiver).
- **Shared memory**: kernel just maps the same physical pages into both processes' address spaces once; afterward processes read/write directly with **no kernel involvement per access** — much faster, but the *processes* must handle their own synchronization (see below) since the kernel provides no ordering guarantees.

### Message Queues
- Kernel-space linked list, identified by an IPC key/id.
- `msgget()` create, `msgsnd()` send, `msgrcv()` receive, `msgctl()` delete/control. `ipcs -q` lists active queues.
- **Full-duplex over one queue**: client sends with `mtype=1` + its PID in the body; server replies with `mtype=client_pid`; client reads messages matching its own PID as mtype. `mtype` is mandatory in System V queues and is exactly what allows many clients to share one server queue safely.

### Shared Memory
- Fastest IPC — the segment is mapped directly into both processes' address spaces (no kernel copy, unlike MQs/pipes/sockets which double-copy: sender→kernel→receiver).
- Needs manual synchronization, e.g. two semaphores:
```c
sem_init(&sem_writer, ..., 1);  // writer may write
sem_init(&sem_reader, ..., 0);  // reader waits for data
// writer: wait(sem_writer); write(); post(sem_reader);
// reader: wait(sem_reader); read();  post(sem_writer);
```
`ipcs -m` lists active shared memory segments.

### Pipes
- Unidirectional FIFO byte stream. `pipe(p)` → `p[0]` read end, `p[1]` write end. Bidirectional comms needs **two** pipes.
- **Anonymous pipes**: only between related processes (parent/child) — pipe must be created **before** `fork()` so both share the same underlying pipe. One side closes the read end, the other closes the write end.
- **Shell pipeline mechanics** (`cat foo | grep bar`): shell creates the pipe, forks child 1, `dup2(p[1], STDOUT_FILENO)` before exec'ing `cat` (redirect its stdout into the pipe), forks child 2, `dup2(p[0], STDIN_FILENO)` before exec'ing `grep`. Both children close their raw pipe fds after dup2 (the duplicated fd keeps the OFT entry open via refcount). fd tables survive `exec()` — only code/data/heap/stack are replaced. See [[IPC]] for the full step-by-step FD-table/OFT refcount trace.
- **Named pipes (FIFOs)**: for **unrelated** processes; created via `mkfifo(path, mode)`, opened by pathname (`O_RDONLY`/`O_WRONLY`), still unidirectional; writing with no reader errors out.

### Blocking vs Non-Blocking IPC
- Blocking: sender blocks if buffer full, receiver blocks if buffer empty.
- Non-blocking: instant return with an error instead of blocking — lets e.g. a server juggle many clients without one slow client stalling the rest.

### Signals
- Lightweight, data-less notifications — essentially "interrupts for user processes."
- Sent via `kill(pid, signum)`; caught via `signal()`/`sigaction()`.
- **Process groups**: e.g. `Ctrl+C` → `SIGINT` delivered to the entire foreground process group (all stages of a pipeline share a PGID, distinct PIDs).

---

## 9. Virtual Memory & Paging

Each process believes it owns the *entire* address space (e.g., 32-bit → 4GB), but really only has a **virtual** address space. All CPU register/pointer accesses and load/store instructions operate on **virtual addresses (VA)**; the **MMU** translates VA → physical address (PA) on every access.

> The OS itself is mapped into **every** process's virtual address space (at high addresses) rather than living in a separate space — this lets a trap jump straight to OS code without a full address-space switch.

### Address Translation Evolution

**1. Base & Bound** — `PA = base + VA`, checked against `bound`. OS sets base/bound registers per process at load/switch time; MMU does the actual per-access translation (OS is not involved in every access).
- Problem: unused space between heap and stack still consumes physical memory (internal fragmentation across the whole image).

**2. Segmentation** — split VA into (segment bits, offset), each segment (code/heap/stack) has its own base+bound+permissions+growth direction.
```c
if (offset >= bounds[segment]) RaiseException(PROTECTION_FAULT);
else PhysAddr = base[segment] + offset;
```
- Problem: OS must manage multiple segment mappings and remap when a segment (e.g. heap) grows.

**3. Paging** — chop memory into fixed-size **pages/frames**. Removes external fragmentation but can still have internal fragmentation (last page not fully used).

### Page Tables
- One **page table (PT) per process**, mapping VA page number → physical frame number.
- PT is a **fixed-size array indexed directly by page number** (not dynamic!) — needed for O(1) hardware lookup billions of times/sec; a search structure would be far too slow and complex for the MMU.
- **PTE fields**: physical frame number, **valid bit** (is this page part of the process at all), **present bit** (is it currently in RAM — 0 ⇒ page fault, could mean swapped out / unallocated / illegal), **dirty bit**, etc.
- MMU has a register (updated on context switch) pointing to the current process's PT base.

**Size problem**: 32-bit VA space, 4KB (2^12) pages → 2^20 PTEs; at 4 bytes/PTE that's **4MB of page table per process** — too big to keep entirely resident/contiguous.

Solutions:
1. Bigger pages → worse internal fragmentation.
2. Hybrid paging+segmentation.
3. **Multi-level page tables** — split the 4MB table into 1024 chunks of 4KB, referenced by an outer table. 20-bit page number → 10 bits outer index + 10 bits inner index. MMU walks the levels to translate.

**Worked example** — 48-bit VA, 4KB pages, 8-byte PTEs:
- Offset = 12 bits (4KB) → 36 bits left for page number.
- PTEs per page = 4KB / 8B = 512 = 2^9.
- Levels needed = ⌈36 / 9⌉ = **4 levels**.
- General formula: `levels = ceil((VAbits − log2(pageSize)) / log2(pageSize/PTEsize))`.

### TLB (Translation Lookaside Buffer)
- Hardware **cache** for VA→PA mappings inside the MMU — without it, a multi-level walk costs one memory access *per level* for every single memory reference.
- **TLB hit**: MMU finds the mapping directly, translates instantly.
- **TLB miss**: MMU walks the page table in memory, populates the TLB, then completes the translation.
- **Flushed on context switch** (mappings are per-process) — a major cost of switching processes.

### Page-Level Protection
Every PTE carries permission bits (user/kernel accessible, read/write/execute). MMU traps to the OS on any violation — this is how user programs are prevented from touching memory they don't own.

### Inverted Page Table
Instead of one PT per process, keep a **single system-wide table** with one entry per **physical** frame, storing (owning process, VA that maps here) — implemented as a hash table (a linear scan would be far too slow). Saves memory (no huge sparse per-process tables) at the cost of hash lookup overhead.

---

## 10. File Systems

### Files, Inodes, Descriptors
- Every file/directory has a low-level name: an **inode number**. A directory is just a list of `(user-readable-name, inode-number)` pairs.
- `open(path, flags, mode)` → returns an **fd** (int, private per process) — indexes into the process's `ofile[NOFILE]` array.
- `read(fd, buf, count)` / `write(fd, buf, count)` — advance the file offset by bytes transferred; a `write()` may transfer **fewer** bytes than requested (disk full, signal interruption) — caller must retry for the remainder.
- `lseek(fd, offset, whence)` repositions the offset: `SEEK_SET` (absolute), `SEEK_CUR` (relative), `SEEK_END` (from EOF — writing past EOF creates a **hole**, file size only updates on write). Also `SEEK_HOLE`/`SEEK_DATA` to find the next gap/data.

### Kernel Structures
```c
struct file { enum {FD_NONE,FD_PIPE,FD_INODE} type; int ref; char readable, writable; struct pipe *pipe; struct inode *ip; uint off; };
struct proc { ... struct file *ofile[NOFILE]; ... };
```
Chain: **per-process FD table** → pointer into the **global open file table (OFT)** → pointer to the in-memory **inode**.
- `dup(oldfd)` / `dup2(oldfd,newfd)` — new fd shares the *same* open file description (same OFT entry, same offset) — this is exactly the mechanism shell pipelines use to redirect stdio (see [[IPC]]).
- `fsync(fd)` — blocks until modified pages are actually flushed to disk.
- `rename(old,new)` — atomic.
- `stat()` — device id, inode #, permissions, link count, owner uid/gid, size, block count, atime/mtime/ctime.

### Directories
- `mkdir(path, mode)` — permission is 3 octal digits × (read,write,execute) for (user,group,other); default governed by `umask`, changed via `chmod`.
- Every directory gets `.` and `..` hardlink entries automatically.
- `opendir()/readdir()/closedir()` to iterate entries.
- `rmdir(path)` — dir must be empty except for `.`/`..`.

### Hard Links vs Soft Links
- **Hard link**: new directory entry pointing to the **same inode** (won't overwrite an existing path).
- **Soft link**: a separate file whose content is the *path string* to the target.

### Designing a File System
Three core pieces: **metadata structures**, **data block management**, **directory/index structures**. "Access methods" = the algorithms mapping `open/read/write` calls onto these structures (name lookup, offset→block translation, block allocation, efficient I/O, metadata updates).

**On-disk layout** (simple FS example, 64×4KB blocks):
1. Divide disk into fixed-size blocks.
2. Reserve most blocks as the **data region** (e.g. 56/64).
3. Reserve blocks for the **inode table** (e.g. 5 blocks × 16 inodes/block(256B each) = 80 inodes = max files).
4. Reserve **bitmaps** for free inodes and free data blocks (alternative: a linked **free list**).
5. Block 0 = **superblock**: inode/data-block counts, inode table start, magic number (FS type).

**mkfs** writes an empty FS + root dir onto a device/partition. **mount(target, fs)** grafts a filesystem onto an existing directory in the tree — subsequent accesses to that path transparently redirect to the mounted FS (original contents hidden while mounted).

### Inodes
- `sizeof(inode)` commonly 256B; addressed via i-number: `inodeAddr = inodeStartAddr + inumber*sizeof(inode)`; convert to disk sector via `sectorSize`.
- **Data block pointers**:
  - **Direct pointers** (e.g. 12 in the inode) — fast, but only cover small files (e.g. <48KB with 4KB blocks).
  - **Indirect / double-indirect / triple-indirect pointers** — point to a block full of more pointers (4KB block / 8-byte ptr = 512 pointers/block), recursively, for large files.
  - Worked example (64-bit block addrs, 4KB blocks, 12 direct + 1 single + 1 double + 1 triple indirect): pointers/block = 512; total addressable pointers ≈ 12 + 512 + 512² + 512³ ≈ 1.34×10^8 → **max file size ≈ 4KB × 1.34×10^8 ≈ 501 GB**.

### Pointer-Based vs Extent-Based vs Linked (FAT-style)
| Approach | Pros | Cons |
|---|---|---|
| Pointer-based (inode ptrs) | most flexible | large metadata for big files |
| Extent-based (start+length) | compact | less flexible; needs contiguous free space |
| Linked (FAT) | simple; the in-memory **File Allocation Table** turns "walk every block" into a fast in-memory pointer-chase, cutting disk I/Os dramatically (e.g., 500 I/Os → 1) | must load whole table into memory |

### Directory Organization
A directory's data block stores `(filename, inode#, record length, strlen)` entries — dynamic table, linked list, or even a B-Tree (XFS). Deleting an entry keeps its `reclen` slot around for reuse by a smaller filename later.

### Free Space Management
1. **Bitmaps** — 1 bit/block; can hunt for N contiguous free blocks for contiguous allocation.
2. **Free list** — superblock points to first free block, each free block points to the next.
3. **B-trees** (XFS) for large-scale free-space tracking.

### Read Path: `open("/foo/bar")` → `read()` → `close()`
1. Read root dir's inode (conventionally **inode 2**; inode 0 = NULL marker, inode 1 = bad-block list).
2. Read root's data blocks, find `foo`'s inode number.
3. Read `foo`'s data blocks, find `bar`'s inode number.
4. Read `bar`'s inode, permission-check, allocate an fd in the OFT.
5. `read()` uses the inode's block pointers to fetch data, advances the in-memory offset, updates atime.
6. `close()` deallocates the fd.

### Write Path
Writing a **new** block costs **5 I/Os**: read data bitmap → write data bitmap → read inode (block list) → write inode (add new block, update metadata) → write the actual data block.

### Create Path (`open(..., O_CREAT)`)
Read inode bitmap → write inode bitmap (mark allocated) → write new inode (init) → read directory inode → write directory data (add name→inode mapping) → write directory inode (update metadata). (Directory growth adds further I/Os.)

### `mmap()`
Maps a file (or anonymous memory) directly into a process's virtual address space; pages loaded **lazily** on first access (page fault → kernel loads from disk → PTE updated).
```c
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```
- `prot`: `PROT_READ | PROT_WRITE | PROT_EXEC | PROT_NONE`
- `flags`: `MAP_PRIVATE` (copy-on-write, changes not written back) vs `MAP_SHARED` (changes visible to others / written back); `MAP_ANONYMOUS` (not file-backed, `fd=-1`); `MAP_FIXED` (rare).
- Uses: memory-mapped file I/O (avoids read/write syscalls), inter-process **shared memory**, backs large `malloc()` allocations.
```c
// shared anonymous memory between parent/child
int *x = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
*x = 10;
if (fork()==0) { *x = 20; return 0; }   // child writes
sleep(1); printf("%d\n", *x);           // parent sees 20
```

---

## 11. Deadlocks

**Deadlock**: a set of processes permanently blocked, each holding a resource needed by another in the set, so none can proceed.

### Necessary Conditions (all four must hold)
1. **Mutual exclusion** — resource held non-sharable by one process at a time.
2. **Hold and wait** — a process holds ≥1 resource while waiting for another.
3. **No preemption** — a resource can't be forcibly taken; only voluntarily released.
4. **Circular wait** — a cycle of processes, each waiting on a resource held by the next.

### Resource Allocation Graph (RAG)
- Nodes = processes + resources. `P → R` edge = P requesting R. `R → P` edge = R allocated to P.
- A **cycle** in the RAG *may* indicate deadlock — guaranteed deadlock only if each resource type has a single instance; with multiple instances a cycle doesn't necessarily mean deadlock.

### Prevention vs Avoidance vs Detection
- **Prevention**: break one of the 4 necessary conditions structurally (e.g., request all resources upfront, impose a global lock ordering).
- **Avoidance**: allow requests but only grant ones that keep the system in a **safe state** (Banker's Algorithm).
- **Detection + Recovery**: allow deadlock to occur, periodically run a detection algorithm, recover when found.

### Safe State
A state is **safe** if there exists *at least one* ordering in which all processes can finish, each getting its max remaining need satisfied from currently-available + resources freed by processes finishing earlier in that order. Safe ⇏ deadlock-free forever, but unsafe ⇒ deadlock is possible.

### Banker's Algorithm (deadlock avoidance)
- Each process declares its **maximum** resource need upfront.
- On a request: OS pretends to grant it, runs the **safety algorithm** (checks if a safe sequence still exists using `Available`, `Max`, `Allocation`, `Need = Max − Allocation` matrices) — grants only if the resulting state is still safe, otherwise the process waits.
- Named for analogy to a banker who won't extend a loan that could leave them unable to satisfy all customers eventually.

### Detection & Recovery (if avoidance not used)
- **Detection**: similar to safety algorithm but using actual `Request` instead of `Max`; a cycle with no safe completion order ⇒ deadlock. Factors influencing how often to run it: frequency of deadlocks, number of processes affected, and cost of detection vs recovery.
- **Recovery**:
  1. **Process termination** — kill all deadlocked processes, or one at a time until the cycle breaks (pick victims by priority/cost/rollback-ease).
  2. **Resource preemption** — forcibly take a resource from a process (must handle rollback + starvation of the victim).

---

## 12. Memory Management — Additional Concepts

### Address Binding
Mapping a program's logical (symbolic) addresses to physical memory addresses. Three points at which binding can happen:
- **Compile-time**: absolute addresses fixed at compilation — must recompile if load location changes.
- **Load-time**: compiler emits relocatable addresses, bound to physical addresses when loaded.
- **Execution-time (runtime)**: binding delayed until the process actually runs, redone on every access via the **MMU** — required if a process can be moved (swapped) during execution. Most modern OSes use this.

### Dynamic Loading vs Overlays
- **Dynamic loading**: a routine isn't loaded into memory until it's actually called — unused routines (e.g., rare error-handling code) never consume memory. Handled by the program/loader, not special OS support.
- **Overlays**: predates virtual memory — used when a program is *larger* than available memory. Only the instructions/data needed at a given time are kept in memory ("overlay"); when a new overlay is needed it replaces one no longer required. Manually managed by the programmer; largely obsolete now that virtual memory exists.

### Swapping
Moves an entire **blocked/inactive process** out to disk (backing store) to free its physical memory for other processes, then swaps it back in later. Distinct from paging (which moves individual *pages*, not whole processes) — modern OSes combine both (swap out individual pages under memory pressure, i.e. paging to disk).

### Compaction
Relocates processes in memory to coalesce scattered free blocks into one contiguous chunk — fixes **external fragmentation** at the cost of relocation overhead (only possible with runtime address binding).

### Locality of Reference
Programs tend to access a small subset of their address space repeatedly over short periods:
- **Temporal locality** — recently accessed addresses are likely accessed again soon.
- **Spatial locality** — addresses near a recently accessed one are likely accessed soon.
This is *why* caching, TLBs, and demand paging (see [[Virtual Memory and Paging]]) work well in practice.

### Associative Memory vs Cache Memory
- **Associative (content-addressable) memory**: searched by *content*, not address — given a key, all entries are compared in parallel, returning matches directly. The TLB is a canonical example (search by VA, get PA).
- **Cache memory**: general high-speed memory holding recently/frequently used data (address-indexed) to reduce access latency to slower main memory.

### Hashed Page Table
Alternative to multi-level page tables for large (e.g. 64-bit) address spaces: VA's page number is hashed into a table; each bucket holds a chain of `(page number, frame number)` pairs for collision handling. Faster average lookup than a deep multi-level walk, but suffers hash collisions and extra chain-traversal overhead vs. a direct-indexed table. (See [[Virtual Memory and Paging]] for the multi-level and inverted alternatives.)

### Paging vs Segmentation (summary)
| | Paging | Segmentation |
|---|---|---|
| Unit size | Fixed | Variable, logical (code/heap/stack) |
| Visible to programmer? | No | Yes — segments map to logical program units |
| Fragmentation | Internal (last page) | External (variable-size holes) |
| Address form | (page #, offset) | (segment #, offset) |

### Effective Access Time (EAT) with Paging/Page Faults
$$EAT = (1-p) \cdot m_a + p \cdot (\text{page fault service time})$$
where `p` = page fault probability, `m_a` = normal memory access time. Since page-fault service time (disk I/O) is orders of magnitude larger than `m_a`, even a small `p` dominates EAT — this is *why* keeping page-fault rate low matters so much.

### Demand Paging & Thrashing
- **Demand paging**: pages are loaded into physical memory only when actually referenced (lazily), not all upfront at process start — a reference to a not-yet-loaded page triggers a **page fault**, the OS fetches it from disk and updates the PTE (same mechanism as [[#10-file-systems|`mmap()`'s lazy loading]]). Reduces memory footprint and startup time but adds per-fault latency.
- **Thrashing**: when the system is so overcommitted (too many processes, too little RAM) that processes spend more time faulting pages in/out than doing useful work — CPU utilization actually **drops** even though the OS keeps adding more processes to try to raise it (a vicious cycle). Fixed by reducing the degree of multiprogramming (fewer concurrent processes) or by working-set/load-control admission policies.

### Best Page Size
No universally "best" size — it's a tradeoff: smaller pages → less internal fragmentation but a bigger page table (more entries) and more TLB misses; larger pages → less page-table overhead and fewer TLB misses but more wasted space per page (internal fragmentation) and coarser-grained disk I/O on page faults. Real systems often support multiple sizes (e.g., "huge pages") and pick per-workload.

### Goals of Memory Management
**Relocation** (a process's physical location can change without breaking its logical view — needs runtime address binding), **protection** (one process can't access another's or the kernel's memory without permission — enforced via PTE permission bits), **sharing** (controlled sharing of code/data between processes, e.g. shared libraries), and **logical vs physical organization** (present programs with a clean linear address space regardless of how physical memory is actually fragmented/allocated underneath).

### Advantages: Dynamic Allocation & Virtual Memory
- **Dynamic (heap) allocation**: memory obtained on-demand at runtime (`malloc`/`new`), supports data structures whose size isn't known at compile time, avoids over-provisioning static arrays.
- **Virtual memory**: lets programs larger than physical RAM run, enables higher degrees of multiprogramming (more processes fit since only active pages are resident), removes the need for contiguous physical allocation, simplifies process address-space management, and provides isolation/protection between processes.

---

## 13. Scheduling & OS Structure — Additional Concepts

### Multiprogramming & Time-Sharing
- **Multiprogramming**: keep multiple jobs in memory so when one blocks on I/O, the CPU switches to another — goal is maximizing CPU utilization (never leave the CPU idle if there's ready work).
- **Time-sharing (multitasking)**: extends multiprogramming with frequent, fast context switches (small time slices) so multiple *interactive users* each get the illusion of a dedicated, responsive system.
- **Multitasking vs Multithreading**: multitasking = OS running multiple independent *programs/processes* concurrently; multithreading = a *single process* running multiple threads concurrently, sharing that process's address space.

### Additional Scheduling Algorithms
- **Priority Scheduling**: each process assigned a priority; highest-priority ready process runs next (can be preemptive or not). Risk: **starvation** of low-priority processes — mitigated by **aging** (gradually increasing the priority of processes that wait too long).
- **Multilevel Queue Scheduling**: ready processes partitioned into separate queues (e.g., foreground/interactive vs background/batch) by process type, each queue with its own scheduling algorithm; queues themselves are scheduled with fixed priority or time-slicing between queues.
- **Multilevel Feedback Queue**: like multilevel queue, but processes can move *between* queues based on observed behavior (e.g., CPU-bound processes demoted to lower-priority queues) — this is essentially aging generalized.

### Dispatcher & Dispatch Latency
- **Dispatcher**: the kernel module that hands the CPU to the process selected by the scheduler — performs the actual context switch, switches to user mode, and jumps to the resumed program counter.
- **Dispatch latency**: time from stopping one process to starting the next (context-switch + dispatcher overhead) — must be kept small, especially in real-time systems where scheduling delay is safety-critical.

### Monolithic vs Microkernel
- **Monolithic kernel**: all core services (scheduler, filesystem, drivers, memory management) run together in kernel space with direct function calls between them — fast (no IPC overhead) but a bug/crash in any component can crash the whole kernel; larger trusted-code footprint. (Linux is a hybrid-monolithic kernel.)
- **Microkernel**: only the bare minimum (IPC, basic scheduling, minimal memory management) runs in kernel space; filesystem, drivers, etc. run as user-space servers communicating via message passing — more fault-isolated and secure, but IPC overhead makes it slower.

### OS vs Kernel
- **Kernel**: the core program that directly manages hardware (CPU, memory, I/O) and runs in privileged (ring 0) mode — always resident in memory.
- **Operating System**: the kernel *plus* everything else — shells, utilities, system libraries, GUI, system daemons — the complete software package a user interacts with.

### Real-Time Systems
Systems where correctness depends not just on the *result* but on meeting **deadlines**:
- **Hard real-time**: missing a deadline is a system failure (e.g., pacemakers, avionics) — requires guaranteed worst-case scheduling (bounded dispatch latency).
- **Soft real-time**: occasional missed deadlines degrade quality but aren't catastrophic (e.g., video streaming, gaming).

### Trap vs Trapdoor
- **Trap**: a synchronous software-generated interrupt used for system calls or to signal an exception (div-by-zero, page fault) — see [[#3-traps-system-calls--context-switching]].
- **Trapdoor** (aka "backdoor"): an undocumented, hidden entry point into a system bypassing normal authentication/security checks — a security vulnerability, not a legitimate OS mechanism.

### Program vs Process
A **program** is a passive, static set of instructions stored on disk (an executable file). A **process** is that program *in execution* — an active entity with its own memory image, PCB, PID, and resources allocated by the OS. One program can spawn multiple processes (e.g., opening the same app twice).

### What a System Would Lack Without an OS
No abstraction over hardware: no process/memory management (programs would need to manage raw memory and CPU time themselves), no file system (raw disk access only), no standardized I/O/device drivers, no user interface, no resource protection between programs, no networking stack — every application would have to reimplement all of this itself.

---

## 14. Synchronization — Classic Problems & Critical Section Theory

### Critical Section — Formal Definition
The **critical section** is the segment of code where a process accesses shared resources (variables, files, data structures) that must not be concurrently modified by another process. A correct solution must guarantee:
1. **Mutual exclusion** — at most one process in the critical section at a time.
2. **Progress** — if no process is in the critical section, one of the processes wanting to enter must be able to, without indefinite postponement by processes not currently interested.
3. **Bounded waiting** — a hard limit on how many times other processes can enter their critical section after a given process has requested entry and before that request is granted (prevents starvation).

### Solutions to the Critical Section Problem
- **Software**: algorithms like **Peterson's** (below) — pure software, no special hardware needed, but limited to small numbers of processes and vulnerable to compiler/hardware instruction reordering on modern architectures.
- **Hardware**: atomic instructions — Test-And-Set, Compare-And-Swap (see [[Locks]]) — the practical, portable modern approach.
- **OS-provided abstractions**: semaphores, mutexes, monitors/condition variables built on top of the hardware primitives.

### Peterson's Algorithm (2-process mutual exclusion, software-only)
```c
int flag[2] = {0, 0};
int turn;

void enter_critical(int i) {           // i = this process's id (0 or 1), j = other
    flag[i] = 1;
    turn = j;
    while (flag[j] && turn == j);      // busy-wait
}
void leave_critical(int i) {
    flag[i] = 0;
}
```
- `flag[i]` = "I want to enter"; `turn` = whose turn it is to wait if both want in.
- Satisfies mutual exclusion, progress, and bounded waiting for exactly 2 processes — but relies on atomic reads/writes and can fail under modern CPU/compiler instruction reordering without memory barriers, so it's mainly of theoretical/interview interest rather than production use.

### Classic Synchronization Problems
| Problem | Setup | Key idea |
|---|---|---|
| **Bounded Buffer** (Producer–Consumer) | Producers fill a fixed-size buffer, consumers drain it | See [[#7-condition-variables--semaphores]] — two counting semaphores (`empty`, `full`) + one mutex |
| **Readers–Writers** | Multiple readers may read concurrently; a writer needs exclusive access | Track reader count with a mutex; first reader locks out writers, last reader unlocks; writer holds the resource lock exclusively. Variants prioritize readers, writers, or enforce fairness to avoid starving either side |
| **Dining Philosophers** | N philosophers around a table, N forks (shared between neighbors), need 2 forks to eat | Naive "pick up left then right fork" deadlocks (circular wait on forks). Fixes: resource ordering (odd philosophers pick right-first), limit to N-1 philosophers eating at once, or an arbitrator/waiter granting fork access |
| **Sleeping Barber** | One barber sleeps when no customers; a customer wakes him; limited waiting-room chairs | Models bounded-buffer + signaling: semaphores for `customers` (waiting count), `barbers` (free barber), and a mutex protecting the waiting-count/chair-count |

### Precedence Graphs
A **directed acyclic graph (DAG)** where nodes are tasks/statements and edges represent "must happen before" dependencies — used to identify which tasks are independent (can run in parallel, e.g. via a [[#7-condition-variables--semaphores|barrier]]) vs which have a required execution order, when designing/scheduling concurrent programs.

### Drawbacks / Issues of Concurrency
Concurrency isn't free: it requires explicit synchronization (locks/CVs/semaphores) adding programming complexity, introduces **context-switch overhead**, and opens the door to an entire class of timing-dependent bugs — **race conditions** (non-atomic ops on shared state), **deadlock**, **starvation**, and **priority inversion** — all notoriously hard to reproduce/debug since they depend on scheduling interleaving.

### Semaphores — Fairness, Advantages & Drawbacks
- **Advantages**: prevent race conditions via atomic counter ops, provide clean **resource counting** (not just binary locking), machine/language-independent concept, improve utilization by letting processes block instead of busy-waiting.
- **Fairness**: a correct implementation queues blocked processes **FIFO**, so `signal()` wakes the longest-waiting process first — this is what actually delivers the bounded-waiting guarantee (a naive re-check loop would not).
- **Drawbacks**: easy to misuse (forgetting a `wait`/`signal`, wrong ordering → deadlock, see the mutex-before-resource-semaphore bug in [[#7-condition-variables--semaphores]]), can cause **priority inversion** (a low-priority process holding a semaphore blocks a high-priority one), and bugs are notoriously hard to debug/reproduce due to timing-dependence.

### Non-Recursive Mutex Locked Twice by the Same Thread
A standard (non-recursive) mutex tracks only locked/unlocked, not *who* holds it or a lock-count. If the owning thread calls `lock()` again before unlocking, it blocks waiting for the lock to be released — but only it can release it, and it's blocked — **self-deadlock**. (A **recursive mutex** tracks owner + a count, allowing the same thread to relock and just incrementing the count.)

### User-Level vs Kernel-Level Threads
| | User-level threads | Kernel-level threads |
|---|---|---|
| Managed by | A user-space threading library | The OS kernel |
| Context-switch cost | Cheap — no kernel trap needed | Expensive — trap + kernel scheduling |
| Kernel awareness | Kernel sees only 1 process (many-to-one) | Kernel schedules threads directly |
| Blocking syscall | Blocks the **whole process** (all threads) unless the library handles it | Only the calling thread blocks; others keep running |
| Multicore parallelism | No (kernel can't schedule them onto separate cores) | Yes |
Real-world runtimes (POSIX pthreads) use kernel-level (or M:N hybrid) models specifically to get true parallelism and avoid the whole-process-blocks-on-one-syscall problem.

### Benefits of Multithreaded Programming
**Responsiveness** (a UI thread stays interactive while a worker thread does heavy work), **resource sharing** (threads share code/heap/data, cheaper than IPC between processes), **economy** (thread creation/context-switch is far cheaper than process creation/switch), and **scalability** (can exploit multiple cores within one process).

---

## 15. I/O, Devices, Disks & Storage

### Buffering, Spooling & Caching
- **Buffer**: temporary memory area that smooths speed mismatches between a fast producer and a slower consumer (or vice versa) — e.g., holding data mid-transfer between CPU and a disk/network device so neither has to wait lock-step on the other.
- **Spooling** (Simultaneous Peripheral Operations On-Line): output/input for a slow device (classically a printer) is staged on disk first; the device consumes from that disk queue at its own pace, freeing the CPU/process immediately instead of blocking on the slow device directly. Enables multiple processes to "print" concurrently without interleaving their output.
- **Caching**: keeping a copy of frequently/recently used data in faster storage (CPU cache, page cache, TLB) to avoid repeatedly paying the cost of a slower access — effectiveness relies on [[#12-memory-management--additional-concepts|locality of reference]].

### DMA & Cycle Stealing
**DMA (Direct Memory Access)**: a controller that transfers data directly between a device and memory *without* CPU involvement per byte/word — CPU just programs the transfer (source, dest, length) then is free to do other work; DMA controller signals an interrupt on completion.
**Cycle stealing**: while performing a transfer, the DMA controller briefly takes control of the system/memory bus (a bus cycle otherwise available to the CPU) — the CPU is momentarily locked out of memory access, but far less disruptive than the CPU doing the whole transfer itself.

### Block vs Character Devices
- **Block devices**: transfer data in fixed-size blocks, support random access (seek to any block) — e.g., disks, SSDs.
- **Character devices**: transfer data as a serial stream of bytes/characters, sequential, no random-access addressing — e.g., keyboards, mice, serial ports.

### Disk Access Time Components
- **Seek time**: time for the disk arm to move the read/write head to the target track — typically the **largest** component of disk access time.
- **Rotational latency**: time waiting for the target sector to rotate under the head once the arm is on the right track — depends on RPM (avg ≈ half a rotation).
- **Transfer time**: time to actually read/write the data once positioned.
Total disk access time ≈ seek time + rotational latency + transfer time — this ordering/cost is *why* SSDs (no moving parts, no seek/rotation) are so much faster for random access.

### RAID Levels (Redundant Array of Independent Disks)
| Level | Technique | Fault tolerance | Notes |
|---|---|---|---|
| RAID 0 | Striping | None | Max performance, no redundancy — one disk failure loses all data |
| RAID 1 | Mirroring | 1 disk | Full duplicate copy; 50% capacity overhead |
| RAID 2 | Bit-level striping + Hamming code ECC | Yes | Rarely used in practice |
| RAID 3 | Byte-level striping + dedicated parity disk | 1 disk | Parity disk is a bottleneck |
| RAID 4 | Block-level striping + dedicated parity disk | 1 disk | Same bottleneck issue as RAID 3 |
| RAID 5 | Block-level striping + **distributed** parity | 1 disk | Most common — no single parity bottleneck |
| RAID 6 | Like RAID 5 + a second distributed parity block | 2 disks | Extra safety margin for larger arrays |

### Belady's Anomaly
For the **FIFO** page-replacement algorithm specifically: increasing the number of available page frames can, counter-intuitively, *increase* the number of page faults for some reference strings (rather than decrease or stay the same). Does **not** occur with LRU or Optimal replacement — a key reason LRU/Optimal are considered "well-behaved" (stack) algorithms and FIFO is not.

### Direct Access Method (Files)
A file-access mode allowing reads/writes at **any** offset directly (via `lseek` + `read`/`write`, or block-number addressing), rather than only sequentially from the start — essential for e.g. database/index files where records must be fetched by key without scanning from byte 0.

### File Operations
`create`, `open`, `read`, `write`, `append`, `seek`/`reposition`, `truncate`, `rename`, `delete`, `close` — see [[#10-file-systems]] for the syscall-level detail on several of these (`open`, `read`/`write`, `lseek`, `rename`).

### Atomicity & Consistency in Multi-Process/Multi-User I/O
- **Atomicity across devices**: coordinate access to a shared device (e.g., a printer, a disk region) using mutexes/semaphores so operations from different processes don't interleave destructively; interrupt handlers must also be careful not to corrupt state a process is mid-update on.
- **Consistency under concurrent access**: techniques include file/record **locking** (advisory or mandatory), reader-writer coordination (see Readers-Writers above), atomic rename/write patterns (write to a temp file, then atomic `rename()`), **journaling** (log intended changes before applying, for crash recovery), and I/O scheduling to serialize conflicting operations.

### Advantages of Multiprocessor Systems
Higher **throughput** (more work done per unit time via parallel execution), better **cost-effectiveness** per unit of computation than multiple single-processor machines, increased **reliability** (graceful degradation — one CPU failing doesn't halt the system, just reduces capacity), and support for true **parallelism** (not just concurrency) on CPU-bound workloads.

---

## 16. Quick-Fire Interview Q&A

- **Why can't a process just wait on another thread's internal queue instead of using a CV?** Conflates locking with signaling; poor isolation — you'd need to lock the whole target thread and force ordering, potentially blocking unrelated users of it.
- **Why `while` not `if` around `wait(cv, mutex)`?** Mesa-style semantics — a woken thread must re-verify the condition since another thread could have changed state again before it actually resumes (signal ≠ guaranteed immediate execution).
- **Why is a semaphore initialized to 0 for ordering but 1 for locking?** Locking: the resource is available immediately, so give it away (1). Ordering: nothing exists to give away yet — the waiter must block until the other side produces something (0).
- **Why is a page table a fixed-size array instead of a dynamic structure?** MMU hardware needs deterministic O(1) indexing on every single memory access (billions/sec) — no room for pointer-chasing search structures.
- **TLB miss cost vs hit?** Hit = 1 memory access. Miss = full page-table walk (1 access per level, e.g. 4 for a 4-level table) + eventual data access + TLB refill.
- **Why must a lock guard both `sem_empty`/`sem_filled` order relative to `mutex` in producer-consumer?** Resource semaphore must be acquired **before** the mutex, or a thread can block on the resource semaphore while holding the mutex, deadlocking the only thread that could release it.
- **Why does `exec()` retain file descriptors but wipe code/data/heap/stack?** FDs live in the `struct proc`/OFT, not the memory image being replaced — this is exactly what lets shell redirection (`dup2` before `exec`) work.
- **fork() count for n forks?** `2^n` total processes (each existing process forks independently).
- **Orphan vs zombie?** Zombie = child exited, not yet reaped (still has exit-code state). Orphan = parent exited while child still running; child reparented to `init`.
- **Internal vs external fragmentation?** Internal = allocated block bigger than what's needed (wasted space *inside* an allocation). External = free memory fragmented into pieces too small individually to satisfy a request, despite enough total free space.
- **Spinlock vs sleeping mutex — when to use which?** Spin for short critical sections (avoid context-switch cost); sleep (mutex) for long critical sections (free the CPU for useful work) — two-phase locks combine both.
