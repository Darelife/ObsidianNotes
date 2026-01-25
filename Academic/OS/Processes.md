Prev : [[Operating System/Introduction]]

A process is basically  a program that's running. The OS allots memory, initializes the CPU context, and starts the process (runs in the CPU) in the **User Mode**, atleast until a system call, interrupt, or a program fault occurs, when it switches to **Kernel Mode**.

> [!info]
> Each process has a process id, and has code + data (from the executable), along with space for the stack + heap (allocated during the run time).

The execution context (the value of the CPU registers) is stored in the memory when the process is paused, and then it's restored when it resumes.

A process can also communicate with I/O devices.

There are a bunch of states in a process
- Ready : A process is ready to be run, and the OS has chosen to not run it at this moment (Only 1 process can be run at a time on a particular CPU Core by the scheduler)
- Running : A Process is running on the processor.
- Blocked, Sleep, Suspended : The process has performed an operation that requires it to not be ready until some other event occurs. The CPU context is stored in the memory, so that it can be run again when the state changes to running.

There are a few other states like,
- Zombie : A process has exited, but hasn't been cleaned up yet. Used to show other processes the examine the return code, and to see whether the process executed successfully or not (0 = successful, non-zero = failed) (When over, the parent process will call wait() or something of that sort, to wait for the completion of the child, and to also indicate to the OS to clean up (or reap) all the stuff in the process)

## Modes and Privileges

> [!cheatsheet] 
> 
> The ring mode is written in the botton 2 bits of the code segment (CS) register

There are logically 4 rings (ring 3 - ring 0). The user mode runs on ring 3, while the kernel mode runs on ring 0. The other 2 rings run the device drivers. Most processes run on ring 3 (the least privileged ring). The ring is written in the bottom 2 bits of the code segment (CS) register. You can actually even check it via this
```cpp
#include <stdint.h>
#include <stdio.h>

int main (int argc, char **argv) {
    uint64_t rcs = 0;
    asm ("mov %%cs, %0" : "=r" (rcs));
    printf("%d\n", (int) (rcs & 3));
    return 0;
}
```

Now, there are basically API's, to manage processes. We call them **System Calls**. It's a function call into the OS code, that runs at a higher CPU privilege level. Sensitive operations are only allowed at a higher privilege level.

There are also a few blocking system calls that cause the process to be blocked and the CPU context gets switched out (**read()** from the disk), while some other system calls like **getpid()** to get the process id returns the value instantly. 

## POSIX

It's a standard set of system calls, and some C lib functions available to user programs defined for portability. Programs written using POSIX API can run on any POSIX compliant OS. Most OS' are POSIX compliant. (Programs may still need to be recompiled for different architectures).


- printf uses the write() system call
- user programs don't need to worry about invoking system calls

## ABI
The Application Binary Interface is an interface between machine code, and the underlying hardware. 

>One easy way to understand "ABI" is to compare it to "API".
>
>You are already familiar with the concept of an API. If you want to use the features of, say, some library or your OS, you will program against an API. The API consists of data types/structures, constants, functions, etc that you can use in your code to access the functionality of that external component.
>
>An ABI is very similar. Think of it as the compiled version of an API (or as an API on the machine-language level). When you write source code, you access the library through an API. Once the code is compiled, your application accesses the binary data in the library through the ABI. The ABI defines the structures and methods that your compiled application will use to access the external library (just like the API did), only on a lower level. Your API defines the order in which you pass arguments to a function. Your ABI defines the mechanics of _how_ these arguments are passed (registers, stack, etc.). Your API defines which functions are part of your library. Your ABI defines how your code is stored inside the library file, so that any program using your library can locate the desired function and execute it.


>The ABI also govern things like how classes/objects are laid out in C++. This is necessary if you want to be able to pass object references across module boundaries or if you want to mix code compiled with different compilers.
>The ABI govern things like how parameters are passed, where return values are placed. For many platforms there is only one ABI to choose from, and in those cases the ABI is just "how things work".

# Process Related System Calls
- fork() : creates a new child process. All processes are created by forking its parent. Init (pid = 1) is the ancestor of every process.
- exec() : makes the process execute a given executable
- exit() : terminates a process
- wait() : causes a parent to be blocked, until its child terminates

## Fork
- Creates a new process, with a new pid.
- The memory image of the parent gets copied into the child.
- <mark style="background: #FF5582A6;">Parent and child both run different copies of the same code.</mark> (So, isn't that an extra process that's running then?? like, the same code gets executed twice)
- After the fork, both the parent and child resume execution in their copies of the code.
- The fork() call returns 0 if it's in the child process, while in the parent process, fork() returns the pid of the parent process.
- The parent and the child run independently
- Any changes in the parents data after the fork doesn't impact the child
- Number of copies formed is technically in powers of 2...if we get those types of questions, like if we get a single fork, we'll have 2 processes, if we get 2 forks(), we'll have 4 processes, and so on. cnt = 2^(# of forks)

## Exit
- When a process ends, (or reaches the end of main (it gets automatically called)), the OS switches the process out, and never runs it again.
- <mark style="background: #FF5582A6;">However, exit doesn't clean up the memory</mark> (So, why is exit even needed??)  <mark style="background: #BBFABBA6;">Here's the ans, from the wait thing...basically wait 
  just makes the parent wait till the memory of the child is cleaned, **after it exits**.</mark>
- Terminated process exists in a zombie state.

## Wait
- Parent calls wait() to clean up (reap) the memory of a zombie child. 
- It cleans up the memory of the child process, and returns in the parent process.
- If the child is still running, the wait system call blocks the parent, till the child process exits, and its memory gets reaped.
- If the child is terminated, its memory gets reaped, and the function returns immediately.
- If the parent has no child, it will return instantly without doing anything.
- There's also a waitpid() system call that requires a pid (of a child), and reaps it after it ends.
- Every fork must be followed by the the wait call at some point in the parent part.
- If the parent exits, while the child is still running, the child is called an orphan, gets adopted by the init process, and gets reaped when the init process terminates (shut down).
- If the parent forks children, but doesn't bother calling wait for a long time, the memory keeps getting filled with zombies. (exhausts system memory)

## Exec
- It's impractical to run the same code in all processes, as the child might want to do a completely separate work. The child process can use **exec()** to get a new memory image.
- Allows the process to switch to running a different code.
- It takes an executable as an argument.
- The memory also gets reinitialized with new executable, new code, data, stack, heap, ...
- There are 6 variants of it in linux (execl(), execlp(), execle(), execv(), execvp(), execvpe())
- Basically if we write any code after the exec system call, it won't run, as the entire code segment, and everything will change.
# How it all pans out
- After the bootup, the init process is first created, and it spawns a shell out, like bash/zsh.
- All the future processes are spawned by forking from existing processes like, init or shell, etc.
- The shell reads the user command, forks a child, execs the command as an executable, waits for it to finish, and then reads the next command.
- Common commands like ls, echo, cat, etc are readily available executables that are simply exec'ed by the shell.
- Some commands are directly implemented within the shell code itself.
- The shell doesn't exec the command directly, it forks, and then execs the child process. (We don't want the shell process to terminate)
- For cd, the chdir system call is used to change the directory of the parent process, and no child process is forked. Each process has a current working dir. We do it to the parent process, as we want to change the dir of the shell itself, and not any child process.
### Foreground and Background Execution
- By default all the processes run in the foreground, and the shell can't accept any new command until the command that's running terminates.
- When we type a command followed by **&** the shell starts running the command, but doesn't wait for it to finish.
- The reaping of background processes in some shells is done periodically, while in others, it's done when the user types in a command, and it's done via the **waitpid()** command.

### I/O Redirection
- Every process has some I/O channels (files) open, which can be accessed by file descriptors.
- STDIN, STDOUT, STDERR are open by default for all processes.
- Parent shell can manipulate these file descriptors of child before exec in order to do things like I/O redirection.
- Eg: Output redirection is done by closing the default STDOUT, and opening a regular file in its place.
```cpp
  close(STDOUT_FILENO);
  open("./redir_output.txt", O_CREAT|O_WRONLY|O_TRUC, S_IRWXU);
  // open uses the first available file descriptor (STDOUT in this case)
  ```

## PIPE
- The shell can also pipe the output of one command into another by connecting the STDOUT of one child to the STDIN of another child, via a pipe (communication method provided by the kernel).
- Example : `cat foo.c | grep factorial`

## Process Control
- **kill()** sends the **SIGKILL** signal for a misbehaving process.
- **CTRL + C** sends a **SIGINT** (Interrupt) signal to the process (usually terminating it) 
- **CTRL + Z** sends a **SIGSTP** (stop) signal to the process, pausing it. We can resume it via the fg command.
- A process should use signal(), or sigaction() system call to **catch** various signals when a particular signal is delivered to a process, it will suspend its normal execution and run a particular piece of code in response to the signal.

# Process Switching

A **trap** is a **controlled transfer of execution** from **user mode to kernel mode**
- **System call**: Program explicitly requests OS service (e.g., `read()`, `write()`)
- **Interrupt**: External device signals the CPU (e.g., keyboard pressed, disk finished I/O)
- **Exception/fault**: Program error (divide by zero, page fault, invalid memory access)

Basically the CPU switches to the kernel mode, runs the OS code to handle the particular event, and then switches back to the low privilege mode (user mode). 

> [!info]
> The process P goes to the kernel mode to run the OS code, but it's still process P that's in the running state. The OS isn't a separate process. It just runs in the kernel, within the same process.

## Function Call

When a user makes a function call, the following happens
- We allocate memory on the user stack for the function arguments, local variables, etc.
- Push the return address, and the PC jumps to the function code.
- Push the register context (for the moment when we get back to the old function when we return to it, to resume it)
- Execute the function code
- To returning from the function, pop the return address, and the register context.

> [!info]
> In a function call, we know the address to jump to for the function directly via a CPU instruction (**call** in x86), however in a system call, we can't trust the user to jump to the correct OS code, without accidentally accessing some other privileged code, or memory address.

There's also another major difference, for saving the register context in a function call, we save it directly into the user stack, however in a system call, the OS doesn't wish to use the user stack, as....what if the user (or a hostile script) has set up malicious values on the stack (edited/pushed malicious return values or the pc values, or the reg values)

### Kernel Stack
- Every process uses a separate kernel stack for running the kernel code, and lives in the kernel's memory...not accessible in the user mode.
- It's used like the user stack, but only for the kernel execution stuff.
- Context is pushed to the kernel stack during a system call, and popped when done.

### IDT (Interrupt Descriptor Table) or the Trap Table
- same stuff as MuP 
- Basically a data structure with the addresses to the kernel code to jump for events.
- It's set up by the OS during bootup, and not accessible via the user mode.
- The CPU uses the IDT to jump to the OS code it wants.

## PCB in xv6: `struct proc`

### Per-process state

```c
struct context {
    uint edi;
    uint esi;
    uint ebx;
    uint ebp;
    uint eip;
};


enum procstate {
	UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE
};

struct proc {
    uint sz;                     // Size of process memory (bytes)
    pde_t* pgdir;                // Page table
    char* kstack;                // Bottom of kernel stack for this process
    enum procstate state;        // Process state
    int pid;                     // Process ID
    struct proc* parent;         // Parent process
    struct trapframe* tf;        // Trap frame for current syscall
    struct context* context;     // swtch() here to run process
    void* chan;                  // If non-zero, sleeping on chan
    int killed;                  // If non-zero, have been killed
    struct file* ofile[NOFILE];  // Open files
    struct inode* cwd;           // Current directory
    char name[16];               // Process name (debugging)
};
```

## Hardware Trap Instruction
- When the user code wants to make a system call, it invokes a special trap instruction. Eg: int n (interrupt) in xv6, argument n indicates the type of the trap (syscall, interrupt).
- The value of 'n' references the IDT.

When the CPU runs the trap instruction,
- CPU moves to a higher privilege level
- CPU shifts the stack pointer reg to the kernel stack of the process
- Reg context is stored on the kernel stack (part of the pcb)
- Address of the OS code to jump to is obtained from IDT, PC points to the OS code
- OS code starts to run, on a secure stack

The base address of the IDT is stored in the CPU, and upon trap, the CPU looks up the address of its interrupt handler. It's needed as the OS needs a secure way of jumping to the OS code, as the user can't be trusted. 

> [!info]
> A trap can be called by a system call, or if an external hardware raises an interrupt. 
>The process is pretty simple though; It saves the context on the kernel stack, switches to the OS address via the IDT, and then runs the OS code to handle that particular trap, before returning back via the information on the kernel stack.

In the IDT,
- 0-31 : System traps, and exceptions
- 31-127 : Device Interrupts
- 128 : int80 syscall interface
- 129-255 : other interrupts

When the **syscall** is called, (eg: fork (its system call number is 1)) this happens
```asm
movl $SYS_fork, %eax       # SYS_fork = 1 here (we put it in the EAX reg)
int $T_SYSCALL             # Triggers the software interrupt (usually 64)
ret
```

## Trapframe & Traphandler
- A trapframe is basically a saved snapshot of the CPU state (all the regs, flags, segment selectors, etc) when the CPU enters the kernel because of a trap.
- It's stored on the kernel stack.
- When a trap is called, it pushes all of it onto the stack

```
alltraps:
  pushl %ds
  pushl %es
  pushl %fs
  pushl %gs
  pusha                # push all general registers
```

then, we set up the kernel data segments
```
  movw $(SEG_KDATA<<3), %ax
  movw %ax, %ds
  movw %ax, %es
```

```
  pushl %esp   # push pointer to trapframe (so kernel code can find it)
  call trap    # call C function trap(tf)
  addl $4, %esp # removes the function argument from the stack after it returns
```


Now, when the OS is done handling the syscall, or the interrupt, it calls a special instruction, **return-from-trap**. It basically
- Restores the CPU context from the kernel stack
- Changes the CPU privilege from the kernel mode to the user mode
- Restore the PC and jumps to the user code after the trap
- The user code resumes execution
- (Before switching to the user mode, the OS checks if it must switch to another process)

Switching between processes
- Sometimes when the OS is in kernel mode, it can't return back to the same process it was running,
	- process terminated or exited (eg: segfault)
	- process made a blocking sys call
- Sometimes the OS doesn't want to return back to the same process
	- the process has run for too long
	- must share CPU time with other processes
- In such cases the OS performs a context switch from one process to another
	- Switch from the kernel mode of 1 process to the kernel mode of another
	- The OS scheduler decides which process to run next and switches to it

next : [[Scheduling]]