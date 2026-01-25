`Reference Book Ch 2`
presented by darelife
# Compilation

```
Source Code
===========
hello.c  (The code we wrote)
   |
   |  [Preprocessor]
   V
hello.i  (Modified source code with all macros and headers expanded)
   |
   |  [Compiler]
   V
hello.s  (Assembly code generated from hello.i)
   |
   |  [Assembler]
   V
hello.o  (Object file - machine code, relocatable)
   |
   |          Other object files (e.g., printf.o)
   |                |
   |                |
   |         [Linker]
   |                |
   V                V
        hello  (Final executable file)

```

# Memory

```
+-----------------------+  Low Memory
|   Code Segment (.text)|  (Executable instructions)
|-----------------------|
| Data Segment (.data)  |  (Initialized global/static vars)
|-----------------------|
|   BSS Segment (.bss)  |  (Uninitialized global/static vars)
|-----------------------|
|       Heap            |
|-----------------------|
|      |                |
|      |                |
|      V     A          |
|            |          |
|            |          |
|-----------------------|
|       Stack           |  
+-----------------------+  High Memory

```

# Modes

There are 3 cases in which the CPU switches from the user mode to the kernel mode (protected mode):
- System Calls (User requests for the OS service)
- Interrupts (External events that require the attention of the OS)
- Program Faults (Errors that need the OS' attention)

# Booting
## Firmware

When we boot our pc, it will first execute a set of instructions on a well defined address. (from a read only, or a flash memory)
- BIOS
- or UEFI on new models (the new version of BIOS)

## BIOS
Basic Input Output System. Originally it used to be written on ROM...later on, they switched to  flash memory, so that it can be rewritten without removing it from the motherboard.

BIOS detects memory modules, and external devices, sets clock speeds, and monitors fans. 
It performs a system test to ensure that all the hardware works properly. (Power On Self Test : POST)

## UEFI
The replacement of BIOS (backwards compatible via the Compatibility Support Module (CSM))
BIOS was too restrictive for larger server operations. First open source implementation was made by Intel (Tiano)

## MBR
So, it uses BIOS. After it performs its checks, it will locate the primary disk, and will read the first 512 bytes -> Master Boot Record (MBR), which will point towards Boot Loader.

UEFI works by using the GPT (GUID Partition Table) partitioning scheme. 

MBR allows upto 4 primary partitions per disk, and upto 2tb per partition.
GPT allows upto 8 ZiB disks.

## How the OS takes control
- So first if u've dual booted, once you select the OS, it loads from the disk to memory.
- Jump to the first instruction, and the OS takes control.
- Creates a stack, sets up the interrupt descriptor table (IDT), initializes virtual memory,
- Organizes the task scheduler, file system, etc
- Launches services for the user space
- As and when the kernel module is required, the module is is loaded via modprobe.

## Init
This is the first process, and it has the process id of 1. It's the parent process. When it ends, the PC shuts down

Next : [[Processes]]