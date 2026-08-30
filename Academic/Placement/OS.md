# Operating Systems — Interview Questions

## Introduction to Operating Systems

1. What is an operating system?  
    It is software that manages all hardware and software resources of a computer, providing a convenient and efficient environment for executing user programs by hiding hardware complexity and acting as a resource manager.
    
2. What would happen if there was no OS?  
    Every application would need to contain its own hardware-interaction code, making apps bulky and complex, allowing one app to monopolize resources, and leaving no memory protection between programs.
    
3. What are the core functions of an operating system?  
    It provides hardware access, an interface between user and hardware, resource management (arbitration) for memory/device/file/process/security, abstraction that hides hardware complexity, and isolation/protection for running programs.
    
4. Differentiate between application software and system software.  
    Application software performs a specific task for the user, while system software (like the OS) operates and controls the computer system and provides the platform on which application software runs.
    
5. What is meant by "resource management" in the context of an OS?  
    It refers to the OS's job of arbitrating access to shared resources such as memory, devices, files, CPU, and security so multiple programs can coexist without conflict.
    

---

## Types of Operating Systems

1. What are the main goals of an operating system?  
    Maximum CPU utilization, minimizing process starvation, and ensuring higher-priority jobs get executed appropriately.
    
2. What is a single-process operating system?  
    The oldest type of OS where only one process executes at a time from the ready queue, with no concurrent execution of others.
    
3. Explain how batch-processing operating systems work.  
    Users submit jobs via punch cards to an operator, who groups similar jobs into batches and submits them to the processor one batch at a time; jobs within a batch execute together without individual prioritization.
    
4. What are the drawbacks of batch-processing systems?  
    They cannot set dynamic priorities for urgent jobs, may cause starvation since a batch can take long to complete, and the CPU can go idle during I/O operations.
    
5. How does multiprogramming increase CPU utilization?  
    By keeping multiple jobs in memory simultaneously so that when one job is waiting on I/O, the CPU can context-switch to another ready job instead of sitting idle.
    
6. What is the difference between multiprogramming and multitasking?  
    Multiprogramming just keeps several jobs in memory to reduce CPU idle time via context switching on I/O waits, whereas multitasking is a logical extension that uses time-sharing to let a single CPU run multiple tasks with the perception of simultaneity, increasing responsiveness.
    
7. What distinguishes a multi-processing OS from multitasking?  
    Multi-processing systems have more than one physical CPU, improving reliability (one CPU failing doesn't stop the system), throughput, and reducing starvation, whereas multitasking still relies on a single CPU with time-sharing.
    
8. What is a distributed operating system?  
    An OS that manages many independent, loosely coupled, physically separate computing nodes (each with its own CPU/memory/GPU) that are networked and communicate with each other.
    
9. What is a Real-Time Operating System (RTOS), and where is it used?  
    An OS designed for error-free computation within strict, tight time boundaries; it is used in systems like air traffic control and robotics where deadlines are critical.
    

---

## Multitasking vs Multithreading

1. Define a program, a process, and a thread.  
    A program is compiled, executable code stored on disk and ready to run; a process is a program under execution residing in RAM; a thread is a lightweight, independent path of execution within a process.
    
2. Why are threads called "lightweight processes"?  
    Because unlike processes, threads within the same process share memory and resources, so creating and switching between them incurs much less overhead than creating separate processes.
    
3. Differentiate multitasking from multithreading.  
    Multitasking involves context-switching between multiple processes (each isolated with its own memory), while multithreading involves context-switching between multiple threads of the same process that share memory and resources.
    
4. Why do threads lack isolation and memory protection, unlike processes?  
    Because the OS allocates memory to a process as a whole, and all threads of that process share that same memory and resource allocation instead of having separate protected spaces.
    
5. How does thread context switching differ from process context switching?  
    Thread switching doesn't require switching the memory address space (though it saves program counter, registers, and stack), making it fast with the CPU cache preserved, whereas process switching includes changing the address space, making it slower and flushing the cache.
    
6. Give a real-world example illustrating multithreading.  
    In a text editor, spell-checking, text formatting, and auto-saving can all happen concurrently via separate threads while the user types, all within the same process.
    

---

## Components of an OS

1. What is a kernel?  
    The kernel is the core part of the OS that interacts directly with hardware and performs the most crucial tasks; it is the first part of the OS loaded at startup.
    
2. What is "user space," and how does it differ from kernel space?  
    User space is where application software runs without privileged direct access to hardware, interacting with the kernel instead, whereas kernel space has direct hardware access and elevated privileges.
    
3. What is a shell?  
    A shell, or command interpreter, is the part of the OS that receives commands from users (via CLI or GUI) and gets them executed.
    
4. List the major functions performed by the kernel.  
    Process management (scheduling, creation/deletion, synchronization), memory management (allocation/deallocation and tracking usage), file management (creating/deleting files and directories, backups), and I/O management (buffering, caching, spooling).
    
5. Differentiate between buffering, caching, and spooling.  
    Buffering copies data between two devices within a single job (e.g., video buffering); caching temporarily stores frequently used data for faster access (e.g., web caching); spooling manages jobs of differing speeds by queuing them, such as print spooling.
    
6. Compare monolithic and microkernels.  
    A monolithic kernel keeps all OS functions (process, memory, file, I/O management) within the kernel itself, making it fast but bulky, memory-heavy, and less reliable since one crashing module can bring down the whole kernel; a microkernel keeps only essential functions (memory and process management) in the kernel and moves file/IO management to user space, making it smaller, more reliable and stable, but slower due to user-kernel mode switching overhead.
    
7. What is a hybrid kernel, and give examples.  
    A hybrid kernel combines advantages of both monolithic and microkernel designs — e.g., keeping file management in user space but most else in kernel space — for the speed of monolithic kernels with the modularity/stability of microkernels; examples include macOS and Windows NT/7/10.
    
8. How does communication happen between user mode and kernel mode?  
    Through Inter-Process Communication (IPC), typically using shared memory or message passing, since the two run in independently protected memory spaces but sometimes need to exchange data.
    

---

## System Calls

1. What is a system call?  
    A mechanism by which a user program requests a service from the kernel that it doesn't have permission to perform directly, such as accessing I/O devices or communicating with other programs.
    
2. How do applications interact with the kernel?  
    Applications use system calls, often through wrapper functions (like the `mkdir` command wrapping the underlying system call), to request services from the kernel.
    
3. How does a transition from user mode to kernel mode happen?  
    It happens via a software interrupt triggered by a system call, which passes control to the System Call Interface and then into the kernel.
    
4. In what language are most system calls implemented?  
    They are implemented in C.
    
5. Name the five main categories of system calls.  
    Process control, file management, device management, information maintenance, and communication management.
    
6. Give a Windows and Unix equivalent for creating a process.  
    In Windows it is `CreateProcess()`, and in Unix it is `fork()`.
    
7. Why can't a user process directly access I/O devices without a system call?  
    Because user programs run with restricted privileges and typically lack permission to perform sensitive operations like device access, so they must request the kernel to do it on their behalf.
    

---

## System Boot Process

1. What happens immediately when a PC is powered on?  
    The CPU initializes itself and looks for firmware (BIOS, or UEFI in modern systems) stored in a chip on the motherboard.
    
2. What is POST, and why does it matter?  
    Power-On Self-Test is the process where the BIOS/UEFI tests and initializes system hardware and loads configuration settings, halting the boot process with an error if something (like missing RAM) is not appropriate.
    
3. What role does the MBR play in booting?  
    The Master Boot Record is a special boot sector at the start of a disk containing code that loads the bootloader, which the BIOS executes to begin booting the actual OS.
    
4. What is a bootloader, and can you name examples for different OSes?  
    It's a small program responsible for booting the rest of the OS (kernel first, then user space); Windows uses Windows Boot Manager (Bootmgr.exe), most Linux systems use GRUB, and Macs use boot.efi.
    
5. How is UEFI different from traditional BIOS?  
    UEFI is more advanced, acting almost like a tiny OS itself — for example, providing features like Intel Management Engine for remote management — beyond simply initializing hardware like BIOS does.
    

---

## 32-Bit vs 64-Bit OS

1. What is the maximum addressable physical memory for a 32-bit OS?  
    2^32 unique memory addresses, i.e., 4 GB of physical memory.
    
2. Why can't a 32-bit OS take advantage of more than 4GB of RAM?  
    Because its 32-bit registers can only generate and address up to 2^32 unique memory locations, so any RAM beyond that is unaddressable.
    
3. Why do 64-bit processors perform better on data-heavy calculations?  
    Because they can execute 8 bytes of data per instruction cycle compared to 4 bytes for a 32-bit processor, allowing larger calculations to be processed at the same time.
    
4. Can a 64-bit CPU run a 32-bit OS? Can a 32-bit CPU run a 64-bit OS?  
    Yes, a 64-bit CPU is backward compatible and can run both 32-bit and 64-bit OSes, but a 32-bit CPU can only run a 32-bit OS.
    
5. How does bit-architecture affect graphics performance?  
    64-bit systems can perform 8-byte graphics calculations, making graphics-intensive applications run faster compared to 32-bit systems.
    

---

## Storage Devices Basics

1. List the memory hierarchy from fastest/smallest to slowest/largest.  
    Register, cache, main memory (RAM) — these form primary memory — followed by secondary memory: electronic disk, magnetic disk, optical disk, and magnetic tape.
    
2. Why are registers the fastest and most expensive form of storage?  
    Because they are built directly into the CPU using expensive semiconductor technology, giving them the highest access speed compared to any other memory type.
    
3. What is the role of cache memory?  
    It's an additional memory layer that temporarily stores frequently used instructions and data so the CPU can access them more quickly than fetching from main memory.
    
4. Compare primary and secondary memory in terms of volatility and cost.  
    Primary memory (registers, cache, RAM) is volatile and costly, while secondary memory (disks, tapes) is non-volatile and cheaper, offering much larger storage capacity.
    
5. Why is secondary storage considered necessary despite being slower?  
    Because it provides much larger, non-volatile, and cheaper storage for persisting data and programs beyond what limited, costly, volatile primary memory can hold.
    

---

## Introduction to Process

1. What is the difference between a program and a process?  
    A program is compiled code ready to be executed and sits on disk, while a process is that program actually under execution, residing in main memory.
    
2. What steps does the OS take to convert a program into a process?  
    Load the program and static data into memory, allocate the runtime stack, allocate heap memory, perform necessary I/O tasks, and finally hand off control to `main()`.
    
3. Describe the memory layout (architecture) of a process.  
    From bottom to top: text (compiled code loaded from disk), data (global and static variables), heap (dynamically allocated variables), and stack (local variables, function arguments, and return values).
    
4. What is a Process Control Block (PCB)?  
    A data structure the OS uses to store all information about a process, such as process ID, program counter, process state, priority, registers, and lists of open files/devices.
    
5. Why does the OS need a process table?  
    Because all processes must be tracked using a table-like structure, where each entry corresponds to one process's PCB, letting the OS manage and reference process metadata.
    
6. What is the purpose of storing register values in the PCB?  
    When a process's time slice expires, its current register values are saved into the PCB so that, when the process is rescheduled, those values can be restored to the CPU registers to resume execution correctly.
    

---

## Process States and Process Queues

1. List and briefly describe the five standard process states.  
    New (process being created), Ready (in memory, waiting for CPU), Running (currently executing on CPU), Waiting (blocked on I/O), and Terminated (finished execution, PCB entry removed).
    
2. What triggers a transition from Running to Waiting state?  
    The process needs to perform an I/O operation or wait for some event, so it voluntarily gives up the CPU and enters the waiting state until that I/O or event completes.
    
3. What is the difference between the job queue and the ready queue?  
    The job queue holds processes in the "new" state residing in secondary memory, waiting for the long-term scheduler; the ready queue holds processes in "ready" state residing in main memory, waiting for the short-term scheduler to assign the CPU.
    
4. What is the "degree of multiprogramming," and who controls it?  
    It's the number of processes present in memory at a given time, and it is controlled by the long-term scheduler (LTS).
    
5. What is a dispatcher, and how is it different from a scheduler?  
    The dispatcher is the module that actually gives control of the CPU to the process selected by the short-term scheduler (STS); the scheduler decides which process runs next, while the dispatcher performs the actual context switch.
    
6. Differentiate long-term, short-term schedulers by their role.  
    The long-term (job) scheduler picks processes from the job pool in secondary memory and loads them into main memory, controlling multiprogramming degree, while the short-term (CPU) scheduler picks a process from the ready queue and dispatches it to the CPU for execution.
    

---

## Swapping, Context-Switching, Orphan and Zombie Processes

1. What is swapping, and which scheduler is responsible for it?  
    Swapping is temporarily moving a process out of main memory to secondary storage (swap-out) and later bringing it back (swap-in) to free up memory; it's handled by the medium-term scheduler (MTS).
    
2. Why might swapping be necessary even in a well-running system?  
    To improve the process mix or because a change in memory requirements has overcommitted available memory, requiring memory to be freed up for other processes.
    
3. What happens during a context switch?  
    The kernel saves the state (context) of the currently running process into its PCB and loads the previously saved context of the next process scheduled to run, restoring its registers and program counter.
    
4. Why is context switching considered pure overhead?  
    Because the system performs no useful computational work while switching; time and CPU cycles are spent purely on saving/restoring state, not advancing any process's execution.
    
5. What is an orphan process, and what happens to it?  
    It's a process whose parent has terminated while it is still running; orphan processes get adopted by the init process, which is the very first process started by the OS.
    
6. What is a zombie process, and why does it occur?  
    A zombie (defunct) process has finished execution but still has an entry in the process table because its parent hasn't yet read its exit status via the `wait()` system call; the entry is removed ("reaped") only after the parent reads that status.
    

---

## Process Scheduling Fundamentals and FCFS

1. Why is CPU scheduling considered the basis of multiprogramming?  
    Because by switching the CPU among multiple resident processes, the OS keeps the CPU productive instead of idle, allowing more work to be done overall.
    
2. Name the primary goals of CPU scheduling.  
    Maximum CPU utilization, minimum turnaround time, minimum wait time, minimum response time, and maximum system throughput.
    
3. Define turnaround time, wait time, and response time.  
    Turnaround time is the total time from arrival to termination (completion time minus arrival time); wait time is time spent waiting in the ready queue for the CPU (turnaround time minus burst time); response time is the time from entering the ready queue until first getting the CPU.
    
4. What is the difference between preemptive and non-preemptive scheduling?  
    In non-preemptive scheduling, once the CPU is allocated to a process it keeps it until it terminates or moves to a waiting state, whereas in preemptive scheduling the CPU can be forcibly taken away after a time quantum expires or a higher-priority process arrives.
    
5. How does FCFS scheduling work, and what problem does it suffer from?  
    FCFS grants the CPU to whichever process arrives first in the ready queue; it suffers from the Convoy Effect, where a single long process delays many short processes behind it, harming average wait time and causing poor resource utilization.
    
6. What is the Convoy Effect?  
    A situation where many processes needing a resource for only a short time get blocked behind one process that holds the resource for a long time, resulting in poor overall resource management.
    

---

## CPU Scheduling: SJF, Priority, and Round Robin

1. How does non-preemptive Shortest Job First (SJF) work, and what's its major challenge?  
    It dispatches the process with the smallest burst time first, but the major challenge is that accurately estimating burst time in advance is practically impossible, and it can also suffer from convoy effect and starvation.
    
2. Why does preemptive SJF give a lower average wait time than non-preemptive SJF?  
    Because scheduling a short job ahead of a long one decreases the wait time of the short job by more than it increases the wait time of the long job, thereby lowering the average across all processes.
    
3. How is SJF a special case of priority scheduling?  
    SJF is equivalent to priority scheduling where the priority assigned to each process is inversely proportional to its burst time.
    
4. What problem can preemptive priority scheduling cause, and how is it solved?  
    It can cause indefinite waiting (starvation) for low-priority processes; this is solved via aging, which gradually increases the priority of processes that have waited too long.
    
5. Why is Round Robin (RR) considered fair and low-starvation?  
    Because it's based purely on arrival time plus a fixed time quantum (not burst time), so every process gets a turn periodically, and no process waits forever.
    
6. What is the trade-off in choosing the time quantum size for Round Robin?  
    A small time quantum increases the number of context switches (more overhead), while a very large time quantum makes RR behave like FCFS, potentially reintroducing the convoy effect.
    
7. What is the scheduling criterion difference between SJF and Round Robin?  
    SJF's criterion depends on burst time (AT + BT), while Round Robin's criterion is arrival time plus the fixed time quantum and does not depend on burst time at all.
    

---

## Multi-Level Queue and Multi-Level Feedback Queue Scheduling

1. What is Multi-Level Queue (MLQ) scheduling?  
    The ready queue is permanently divided into multiple sub-queues (e.g., system, interactive, batch processes) based on a fixed property, and each sub-queue uses its own scheduling algorithm.
    
2. Why is MLQ described as "inflexible"?  
    Because once a process is assigned to a particular queue based on its type or priority, it stays there permanently and cannot move to another queue even if its behavior changes.
    
3. What major problem does MLQ suffer from?  
    Lower-priority queues can starve because higher-priority queues must be completely emptied before lower ones are scheduled at all, and the convoy effect can still occur.
    
4. How does Multi-Level Feedback Queue (MLFQ) improve on MLQ?  
    MLFQ allows processes to move between queues based on their behavior — CPU-heavy processes get demoted to lower-priority queues, and processes waiting too long in lower queues get promoted (aging) — reducing starvation and increasing flexibility.
    
5. Which scheduling algorithms have no convoy effect according to the standard comparison table?  
    Preemptive SJF and Round Robin do not exhibit convoy effect, unlike FCFS, non-preemptive SJF, priority, preemptive priority, MLQ, and MLFQ.
    

---

## Introduction to Concurrency

1. What is concurrency in the context of operating systems?  
    The execution of multiple instruction sequences at (or appearing to be at) the same time, occurring when several process threads run in parallel or are interleaved.
    
2. Would a single-CPU system ever gain performance from multithreading? Why or why not?  
    No — since only one thread can execute on the CPU at a time, threads must still context switch for that single CPU, so there is no actual parallel gain, though responsiveness may still improve.
    
3. What data structure is analogous to a PCB but used for threads?  
    A Thread Control Block (TCB), which stores per-thread state information used during context switching.
    
4. What are the main benefits of multithreading?  
    Improved responsiveness, efficient resource sharing among threads of the same process, economy (cheaper to create/switch threads than processes), and better utilization of multiprocessor architectures.
    
5. Why is thread creation described as more "economical" than process creation?  
    Because allocating separate memory and resources for a new process is costly, whereas threads of the same process share existing memory and resources, making their creation and context switching cheaper.
    

---

## Critical Section Problem

1. What is a critical section?  
    The segment of code in which processes or threads access and modify shared resources, such as common variables or files, where concurrent execution could lead to inconsistency if interrupted mid-execution.
    
2. Define race condition.  
    A situation where two or more threads access shared data and attempt to modify it simultaneously, such that the final result depends unpredictably on the order in which the thread scheduler interleaves their execution.
    
3. Can a simple flag variable reliably prevent race conditions? Why not?  
    No — a simple flag variable alone is not sufficient because checking and setting the flag is not an atomic operation, so two threads can still interleave around it and enter the critical section simultaneously.
    
4. What are the main strategies to solve the race condition problem?  
    Making critical code atomic (executed in one CPU cycle), using mutual exclusion via locks, and using semaphores.
    
5. What is a limitation of Peterson's solution?  
    It only works correctly for exactly two processes or threads, not for more.
    
6. What disadvantages come with using mutex locks?  
    Contention (threads busy-waiting, or worse, infinite waiting if the lock holder dies), potential deadlocks, harder debugging, and possible starvation of high-priority threads.
    

---

## Condition Variables and Semaphores

1. What is a condition variable, and what problem does it solve?  
    It's a synchronization primitive that lets a thread wait until a specific condition occurs, working alongside a lock; it solves the problem of busy waiting since the thread releases the lock while waiting and only reacquires it once notified.
    
2. How does a semaphore differ from a mutex?  
    A semaphore is an integer representing the number of available resource instances and can allow multiple threads to execute the critical section concurrently up to that count, whereas a mutex allows only one thread to access a single shared resource at a time.
    
3. What is the difference between a binary semaphore and a counting semaphore?  
    A binary semaphore can only take values 0 or 1 (functioning like a mutex lock), while a counting semaphore can range over an unrestricted domain and controls access to a resource with multiple instances.
    
4. How does the modified (blocking) semaphore implementation avoid busy waiting?  
    When `wait()` finds the semaphore value non-positive, instead of looping, the process is placed into a waiting queue associated with the semaphore and its state is switched to Waiting; the CPU is then given to another process until a `signal()` wakes it up and moves it back to the ready queue.
    
5. What triggers a blocked process to be restarted from a semaphore's wait queue?  
    Another process executing a `signal()` operation, which invokes a `wakeup()` that changes the blocked process's state from waiting to ready and places it into the ready queue.
    

---

## The Dining Philosophers Problem

1. Describe the setup of the Dining Philosophers problem.  
    Five philosophers sit around a circular table with a bowl of noodles in the center and exactly five forks (one between each pair of adjacent philosophers); each philosopher alternates between thinking and eating, needing both adjacent forks to eat.
    
2. How is the naive solution to Dining Philosophers implemented using semaphores?  
    Each fork is modeled as a binary semaphore initialized to 1; a philosopher calls `wait()` on both adjacent forks to acquire them before eating, and calls `signal()` on both to release them afterward.
    
3. Why does the naive semaphore-only solution still risk deadlock?  
    If all five philosophers become hungry simultaneously and each picks up their left fork first, all fork semaphores become 0, and every philosopher then waits forever for their right fork — a circular wait deadlock.
    
4. Name two enhancement strategies used to make the Dining Philosophers solution deadlock-free.  
    Allowing at most four philosophers to sit (attempt to eat) simultaneously, and requiring a philosopher to pick up both forks only if both are available, done atomically within a critical section.
    
5. What is the odd-even rule solution to Dining Philosophers?  
    An odd-numbered philosopher picks up their left fork first and then their right, while an even-numbered philosopher picks up their right fork first and then their left, breaking the symmetric circular wait pattern.
    

---

## Deadlocks — Conditions and Prevention

1. Define deadlock.  
    A situation where two or more processes are each waiting indefinitely for a resource that is held by another waiting process, such that none of them can ever proceed.
    
2. List the four necessary conditions for deadlock to occur.  
    Mutual exclusion (only one process can use a resource at a time), hold and wait (a process holds at least one resource while waiting for another), no preemption (resources can't be forcibly taken away), and circular wait (a cycle of processes each waiting on a resource held by the next).
    
3. Why can't we generally prevent deadlock by denying mutual exclusion?  
    Because some resources are intrinsically non-sharable (e.g., a printer), so mutual exclusion cannot be eliminated for those resource types even though sharable resources like read-only files can allow concurrent access.
    
4. Describe the two protocols for preventing the "hold and wait" condition.  
    Protocol A requires a process to request and be allocated all its needed resources before beginning execution; Protocol B allows a process to request resources only when it currently holds none, requiring it to release everything before requesting more.
    
5. How does preventing "no preemption" work, and what risk does it introduce?  
    If a process holding resources requests another that can't be immediately granted, all its currently held resources are preempted and it must restart once it can regain both old and new resources; this approach risks causing a live lock.
    
6. How is the circular wait condition prevented?  
    By imposing a strict global ordering on resource acquisition, so all processes must request resources in the same predefined order (e.g., always lock R1 before R2), preventing cyclic dependency chains.
    
7. What are the three general categories of methods for handling deadlocks?  
    Deadlock prevention/avoidance protocols that ensure the system never enters a deadlocked state, allowing the system to enter a deadlock and then detecting and recovering from it, or simply ignoring the problem entirely (the Ostrich algorithm).
    

---

## Deadlocks — Avoidance, Detection, and Recovery

1. What is the core idea behind deadlock avoidance?  
    The kernel is given advance information about the maximum resources each process may use during its lifetime, and it uses this to decide, for every resource request, whether granting it would leave the system in a safe state.
    
2. Define a "safe state" versus an "unsafe state."  
    A safe state is one where the system can allocate resources to each process in some order (up to each one's maximum need) without deadlock occurring — implying a safe sequence exists; an unsafe state is one where no such guarantee can be made, though it may or may not actually lead to deadlock.
    
3. What is the Banker's Algorithm used for?  
    It's a deadlock-avoidance scheduling algorithm that checks, before granting a resource request, whether doing so would leave the system in a safe state; if not, the requesting process must wait.
    
4. How is deadlock detected when there is a single instance of each resource type?  
    Using the wait-for graph method — a deadlock exists if and only if there's a cycle in the wait-for graph, which the system periodically checks for using a cycle-detection algorithm.
    
5. How is deadlock detected when there are multiple instances of each resource type?  
    Using a variant of the Banker's Algorithm adapted for detection rather than avoidance.
    
6. What are the two general recovery strategies once a deadlock is detected?  
    Process termination (abort all deadlocked processes at once, or abort them one at a time until the cycle breaks) and resource preemption (successively take resources from processes and give them to others until the deadlock cycle is broken).
    

---

## Memory Management: Logical vs Physical Address & Contiguous Allocation

1. Differentiate between logical and physical addresses.  
    A logical (virtual) address is generated by the CPU and is what the user program directly works with, not existing physically; a physical address is the actual location in main memory loaded into the memory-address register, which the user can never access directly.
    
2. What role does the Memory Management Unit (MMU) play?  
    It performs the runtime mapping from a logical (virtual) address to the corresponding physical address, typically by adding the value stored in the relocation register to the logical address.
    
3. How do the relocation register and limit register work together for memory protection?  
    The relocation register holds the base value of the smallest physical address for a process, and the limit register holds the range of valid logical addresses; every logical address is first checked against the limit register (must be less than it) before the relocation value is added to form the physical address, and any violation causes a trap.
    
4. What is internal fragmentation, and when does it occur in fixed partitioning?  
    It's the wasted memory that occurs when a process's size is smaller than the partition it's loaded into, leaving unused space within that partition.
    
5. What is external fragmentation, and how does it differ from internal fragmentation?  
    External fragmentation is unused memory scattered across multiple non-contiguous partitions that together might be enough for a process but cannot be used because they aren't contiguous, unlike internal fragmentation which is wasted space within a single allocated partition.
    
6. How does dynamic partitioning improve on fixed partitioning, and what limitation remains?  
    Dynamic partitioning declares partition size at load time to exactly match the process, eliminating internal fragmentation, removing the size limit, and improving the degree of multiprogramming, but it still suffers from external fragmentation as processes complete and leave irregular gaps.
    
7. Why does fixed partitioning result in a low degree of multiprogramming?  
    Because partition sizes are fixed and cannot be adjusted to fit varying process sizes, limiting how many processes can be accommodated in memory at once.
    

---

## Free Space Management

1. What is compaction (defragmentation), and why is it used?  
    It's a technique that rearranges memory so all loaded partitions are brought together and free partitions are merged into a contiguous block, reducing external fragmentation so larger processes can be accommodated.
    
2. Why does compaction reduce system efficiency?  
    Because moving all the free spaces from scattered locations into a single place takes processing time and overhead, temporarily reducing throughput.
    
3. How is free memory space typically represented internally by the OS?  
    Using a free list, typically implemented as a linked-list data structure of memory holes.
    
4. Compare First Fit and Best Fit allocation strategies.  
    First Fit allocates the first hole found that is big enough, making it simple and fast; Best Fit searches the entire list to allocate the smallest hole that's big enough, reducing internal fragmentation but being slower and potentially creating many small unusable holes (worsening external fragmentation).
    
5. What is the difference between Next Fit and First Fit?  
    Next Fit is an enhancement on First Fit that always resumes the search from the location of the last allocated hole rather than starting from the beginning each time, while retaining First Fit's simplicity and speed.
    
6. Why might Worst Fit be chosen despite being slow?  
    Because deliberately allocating the largest available hole leaves behind larger remaining holes that may still accommodate other future processes, though it requires iterating the whole free list, making it slow.
    

---

## Paging

1. What problem does paging solve that contiguous allocation cannot?  
    Paging allows a process's physical address space to be non-contiguous, so a process can be loaded into scattered free frames instead of requiring one large contiguous block, eliminating external fragmentation.
    
2. Define frames and pages, and how they relate.  
    Frames are fixed-size blocks that physical memory is divided into; pages are fixed-size blocks of the same size that logical (process) memory is divided into, and each page maps to some frame.
    
3. What is the purpose of the page table?  
    It stores the mapping of which logical page corresponds to which physical frame, containing the base address of each page in physical memory.
    
4. How is a logical address translated using paging?  
    The CPU-generated address is split into a page number (p) and a page offset (d); p is used as an index into the page table to find the corresponding frame's base address, and d is added to that base address to get the final physical address.
    
5. Where is the page table stored, and what register tracks it?  
    The page table is stored in main memory, with its base address stored in the process's PCB; a Page Table Base Register (PTBR) points to the currently active page table and is updated at context-switch time.
    
6. Why is paging inherently slow, and how is it sped up?  
    Because looking up a physical address normally requires an extra memory reference to the page table itself; this is sped up using a Translation Look-aside Buffer (TLB), a small, fast hardware cache that stores recent page-to-frame mappings.
    
7. What is a TLB hit versus a TLB miss?  
    A TLB hit means the TLB already contains the mapping for the requested logical address, allowing direct fast lookup; a TLB miss means the mapping isn't cached, requiring the slower page-table lookup, after which the entry is added to the TLB.
    
8. What is the purpose of the Address Space Identifier (ASID) in the TLB?  
    It uniquely identifies which process a TLB entry belongs to, allowing the TLB to safely hold entries from multiple processes at once; if the ASID of the currently executing process doesn't match the entry's ASID, it's treated as a TLB miss for protection purposes.
    

---

## Segmentation

1. What problem does segmentation address that paging does not?  
    Segmentation supports the user's logical view of a program (divided into meaningful units like functions), whereas paging is closer to the OS's view and may split a single logical function across multiple, possibly non-simultaneously-loaded, pages.
    
2. How is a logical address structured under segmentation?  
    As a pair `<segment-number, offset>` denoted `{s, d}`, identifying which segment and the offset within it.
    
3. What does the segment table contain, and how is address translation performed?  
    It contains a base and limit for each segment; the CPU's segment offset (d) is checked against the limit (must be less), and if valid, added to the segment's base address to compute the physical address.
    
4. List an advantage and a disadvantage of segmentation compared to paging.  
    Advantage: no internal fragmentation and efficient contiguous access within a segment, since segments logically group related functions; disadvantage: it suffers from external fragmentation since segments vary in size.
    
5. Why do modern architectures combine segmentation and paging?  
    To gain segmentation's benefit of matching the user's logical program view while also getting paging's benefit of eliminating fragmentation issues, using a hybrid approach.
    

---

## Virtual Memory and Demand Paging

1. What is virtual memory, and what problem does it solve?  
    It's a technique allowing execution of processes that aren't completely loaded into physical memory, giving the illusion of a much larger main memory by treating part of secondary storage (swap space) as an extension of memory; this removes the constraint that a program's size must fit within available physical memory.
    
2. What is demand paging?  
    A virtual memory management method where pages of a process are loaded into main memory only when they are actually needed (demanded), rather than loading the entire process upfront.
    
3. What is the difference between a "swapper" and a "pager"?  
    A swapper manipulates entire processes when moving them between memory and disk, whereas a pager is concerned with moving individual pages of a process, which is the technically correct term when working with demand-paged systems.
    
4. Explain the valid-invalid bit scheme in the page table.  
    Each page table entry has a bit indicating whether the associated page is both legal and currently in memory (valid, bit = 1) or is either not part of the process's logical address space or currently resides only on disk (invalid, bit = 0).
    
5. What is a page fault, and what triggers it?  
    A page fault occurs when a process attempts to access a page marked invalid in its page table (i.e., a page not currently in memory), causing the paging hardware to raise a trap to the OS.
    
6. Walk through the steps the OS takes to handle a page fault.  
    Check an internal table (in the PCB) to see if the reference was valid; if invalid, throw an exception, otherwise find a free frame, schedule a disk read to bring the desired page into that frame, update the page table once the read completes, and finally restart the interrupted instruction.
    
7. What is "pure demand paging"?  
    An extreme approach where a process starts execution with literally no pages loaded in memory; the very first instruction immediately causes a page fault, bringing pages in only strictly as needed.
    
8. Why does demand paging rely on the principle of locality of reference?  
    Because programs tend to access a relatively small set of pages repeatedly over short periods, locality allows demand paging to achieve reasonable performance despite loading only parts of a process at a time.
    
9. List one advantage and one disadvantage of virtual memory.  
    Advantage: it increases the degree of multiprogramming and lets users run large applications with less real physical memory; disadvantage: the system can become slower due to swapping overhead, and thrashing may occur.
    

---

## Page Replacement Algorithms

1. Why is page replacement needed?  
    Because when a page fault occurs and all frames are already in use (high memory utilization), the OS must select an existing page to evict from a frame to make room for the newly demanded page.
    
2. What is the overall aim of any page replacement algorithm?  
    To minimize the number of page faults that occur over the process's execution.
    
3. How does the FIFO page replacement algorithm work, and what is its main weakness?  
    It replaces the oldest page that entered memory, regardless of how frequently or recently it's used; its weakness is that the oldest page might still be heavily used, causing it to be swapped back in immediately (another fault).
    
4. What is Belady's Anomaly, and which algorithm exhibits it?  
    It's the counterintuitive phenomenon where increasing the number of available frames actually increases the number of page faults, rather than decreasing them; FIFO is known to exhibit this anomaly in certain cases.
    
5. Describe the Optimal page replacement algorithm and its practical limitation.  
    It replaces the page that will not be used for the longest time in the future (or never again if such a page exists); it achieves the lowest possible page fault rate but is impractical because it requires future knowledge of the reference string.
    
6. How does Least Recently Used (LRU) approximate the Optimal algorithm?  
    LRU uses the recent past as an approximation of the near future, replacing the page that has not been used for the longest period, based on the intuition that pages unused recently are less likely to be needed soon.
    
7. Describe two implementation approaches for LRU.  
    Using counters, where each page table entry has an associated timestamp and the smallest-time page is replaced; or using a stack of page numbers, where a referenced page is moved to the top so the least recently used page naturally sits at the bottom (often implemented as a doubly linked list).
    
8. Differentiate Least Frequently Used (LFU) and Most Frequently Used (MFU) page replacement.  
    LFU tracks a reference count per page and replaces the page with the smallest count (assuming actively used pages have larger counts); MFU replaces the page with the largest count on the reasoning that a newly brought-in page (with a small count) probably hasn't been used yet and shouldn't be evicted — neither is commonly used in practice.
    

---

## Thrashing

1. Define thrashing.  
    A state where the system spends more time servicing page faults than actually executing processes, caused by processes not having enough frames allocated to support their pages in active use.
    
2. Explain why thrashing becomes a vicious, self-worsening cycle.  
    If a process lacks enough frames for its active pages, it quickly page-faults, and since all its resident pages are actively needed, it must evict one that will be required again almost immediately, causing continuous faulting and no real progress.
    
3. How does the working set model help prevent thrashing?  
    Based on the locality model, it allocates enough frames to a process to cover its current locality of reference; if the allocated frames are fewer than the size of that locality, the process is bound to thrash.
    
4. How does the Page Fault Frequency (PFF) strategy control thrashing?  
    The OS establishes upper and lower bounds on an acceptable page-fault rate; if a process's fault rate exceeds the upper bound, it's given an additional frame, and if it falls below the lower bound, a frame is taken away, keeping the fault rate — and thus thrashing — under control.
    
5. What is the relationship between degree of multiprogramming and CPU utilization when thrashing sets in?  
    CPU utilization initially increases as more processes are added (better multiprogramming), but beyond a certain point adding more processes causes thrashing, sharply dropping CPU utilization as most time goes to servicing page faults instead of useful execution.