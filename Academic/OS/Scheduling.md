prev : [[Processes]]

The OS maintains a list of all the active processes (PCBs) in a data structure, like proc's added via fork, removed after cleanup in wait

The OS scheduler is a special code in the OS that periodically loops over this list, and picks the processes to run.

Basic outline:
- When invoked, save the context of whatever's running in its PCB.
- Loop over all read/runnable processes and identify the process to run next.
- Restore context of the new process from its PCB and get it to run on the CPU.
- Repeat this process for as long as the system is running.

OS scheduling involves 2 processes : the policy to pick the process to run, the mechanism to switch to that process

- Non - Preemptive (Cooperative) : Schedulers are polite, and switch only when the process is blocked or terminated
- Preemptive (non-cooperative) : Schedulers can switch any time, even when the process is ready to continue
	- The CPU generates periodic timer interrupts, and after servicing the interrupt, the OS checks if the current process is run for too long

## Context Switch

During a context switch, if a process A has moved to the kernel stack, and after running for some time in the kernel stack, A can't run anymore (eg: disk read is initiated, but the data takes time to arrive), the OS scheduler picks another process B to run next. The OS saves the kernel context of A on the kernel stack. The user context captures where the execution stopped in the user mode, and the kernel context captures where the execution stopped in the kernel mode. The OS switches the ESP from the kernel stack of A to the kernel stack of the next process B. The kernel stack of B contains the kernel context and the user context of B. The OS stores the kernel context, resumes the execution in the kernel mode of B, at the point it gave to the CPU. Then the OS pops the user context, and resumes the execution in the user mode of B where it trapped into the OS.

## Saving and Restoring Context
- The CPU context and the PC are saved on the kernel stack in 2 different scenarios 
- user to kernel : saved on the stack by the trap instruction, and then restored by return-from-trap
- context switch : kernel context gets saved on the kernel stack...restored when the process is ready to run and switched back in again.

## Context Switching in xv6
- every CPU has a scheduler thread (special OS process)
- the scheduler goes over the list of processes and switches to one of the runnable ones
- the special function switch() performs the actual context switch from the scheduler to the user process.
- switch() in xv6 saves the reg in context structure on the kernel stack on the old process.
- pops the regs from the new context structure
- the CPU now has the new context

## Goals of CPU Scheduling
- Maximize utilization
- Minimize completion time / turnaround time
- Minimize response time
- Fairness
- Low overhead of scheduling policy
	- shouldn't take too long to make a decision
	- shouldn't cause too many context switches (~1 micro sec (mu s) to switch)

### FIFO / FCFS (Simplest)
- First in First out / First come First served
- Newly created processes are pushed to the FIFO queue, and the scheduler runs them one after another from the queue.
- It's non-preemptive : The process is allowed to run till it terminates/exits/blocks.
- Problem : Small processes get stuck behind long processes, and the average turn around time tends to be high (Convoy effect).
### Shortest Job First (SJF)
- Assumes CPU burst (the CPU time used by a process in a continuous stretch) of a process, and is known as apriori
- Picks a process with the smallest CPU burst to run next (non-preemptive)
- Stores PCBs in a heap like structure, and extracts the process with the min CPU burst
- It's still non-preemptive, so, even though in some cases, it could have a lower turnaround time, if the smaller tasks come when a larger task is running, we can do nothing, but wait for the larger task to finish.
### Shortest Remaining Time First (SRTF)
- AKA Shortest Time to Completion First (STCF), or Preemptive Shortest Job First (PSJF)
- Avoids the problem of smaller processes getting stuck behind larger processes.
### Round Robin
- Every process executes for a fixed quantum slice, that's neither too big, nor too small
- A timer interrupt is used to enforce periodic scheduling
- Good response time, and fair
- Bad for turnaround time (Cuz like, every process has to wait for its next turn again and again)
- xv6 is implemented like this
### Weighted Fair Queuing (WFQ)
- Round Robin with different weights or priorities to processes
	- Decided by the scheduler, or can be set by the user.
	- The time slice will be in proportion to the weight/priority
- In real life, schedulers aren't able to exactly enforce time slice...as what if the time slice isn't exactly synced with the time interrupt, and what if the process blocks before its time slice.
- Practical Modification : Keep track of the run time process, schedule process that has been used the least fraction of its fair share; Compensate excess/deficit running time in the future time slices.
### Linux Completely Fair Scheduler (CFS)
- A variant of WFQ
- Divide the CPU evenly among all of its competing processes. Through a counting based technique known as virtual run time(vruntime), pick the process with the lowest vruntime to run next.
- One configurable parameter:
	- Sched_latency : How long a process should run before considering a switch
		- Typically 48ms
		- CFS divides this number by the number of processes running to determine the time slice for each process,  but if there are too many processes, there will be too many context switches.
- Another configurable parameter:
	- min_granularity : least value permissible for a time slice
		- ensures that not too much time is spent in scheduling overhead.
		- Typical value 6ms
- Niceness:
	- man nice : value between -20 to +19
	- admin  or the user can set the priority level which determines the time slice of a process.
	- Default value = 10
	- 	$$
\text{time\_slice}_k = \frac{\text{weight}_k}{\sum_{i=0}^{n-1} \text{weight}_i} \cdot \text{sched\_latency}
$$
	- prio_to_weight Mapping (Nice → Weight)
	  ```
		| Nice | -20  | -15  | -10 | -5  | 0    | 5   | **10** | 15  |
		|------|------|------|-----|-----|------|-----|--------|-----|
		| Wt   | 88761| 29154| 9548| 3121| 1024 | 335 | **110**| 36  |
		```
		
- Uses RB Trees
	- Simple data structures like lists don’t scale - searching through a long-list every so many milliseconds is wasteful.
	- Only keeps runnable processes in the structure.
	- Processes are ordered by vruntime
	- Most operations are in log(n)

next : [[Threads]]