[[Virtual Memory and Paging]]
## Mutual Exclusion
Incorrect execution of code due to concurrency: Race condition. Happens due to an unfortunate timing of the context switches, the atomicity of the data update gets violated. During concurrent execution on shared data.

Locks are special variables that provide mutual exclusion.
When a thread T1 acquires a lock, another thread T2 cannot acquire the same lock

## Mutex  : POSIX pthread library
A var is passed as a parameter to lock and unlock.
Now, how it works is:
1. There's an instruction called CAS (it's atomic in the CPU : happens instantly, and nothing can interrupt it).
2. It checks the value of the the mutex variable, and checks whether it's 1 or 0...if it's 1, someone else is holding it. So, it blocks the thread, and if it's 0, then, within the atomic instruction, it sets it to 1, and locks the mutex.
3. If a thread gets blocked, then it goes to sleep, and gets added to the locks wait queue. When the lock is released, the kernel wakes up a waiting thread.

### Goals of lock implementation
1. Mutual exclusion
2. Fairness
3. Low overhead

### Implementation 1 : Interrupt Control

On locking, disable interrupts, and upon unlocking, enable it. Now, first of all, the thread needs to be trustworthy for this to happen, and on multi core cpu's, if one core disables interrupts, another core can access the lock. Also, disabling interrupts can cause the OS to delay important tasks requiring interrupts like Eg: DMA completing the disk read. It can be used in the kernel though, as it's trustworthy.

### Implementation 2 : Spin Wait

```c
int is_locked = 0;

// wait till the lock is 1, as soon as it gets freed, take it
void acquire_lock() {
	while (is_locked==1);
	is_locked = 1; 
}

void release_lock() {
	is_locked = 0;
}
```

However, this will fail if it context switches right after `while (is_locked)`, as in that case then, if it's in a critical section, and context switches back to it after `while (is_locked)`, it won't check for is_locked again, even though it's occupied, and will continue, causing a race condition.

This also wastes CPU cycles.

So basically, we need a way to check a variable, and then, set its value automatically, without any context switch between them. This is possible via a hardware atomic instruction.

### Test & Set instruction

It has 2 arguments : address of variable, and the new value to set
It writes the new value, and returns the old value, within a single step.
On SPARC it is called the load/store unsigned byte instruction (ldstub)
On x86 it is the locked version of the atomic exchange (xchg)

A simple lock can be implemented using this instruction
isLocked variable indicates locked status (0 = free, 1 = acquired)
if test_and_set(&isLocked, 1) returns 1: the lock isn't free. Wait
if it returns 0, the lock was free, and now has been acquired.

```c
typedef struct __lock_t {
		int flag;
} lock_t;

void init(lock_t *lock) {
	lock->flag = 0;
}

void lock(lock_t *lock) {
	while (TestAndSet(&lock->flag, 1) == 1);
}

void unlock(lock_t *lock) lock->flag = 0;
```
### Compare And Swap (CAS)

It has 3 arguments, address of the variable, the expected old value, and the new value. If the old value is the expected one, it will write the new one, and will return the old value.

```c
void lock(lock_t *lock) {
	while (CompareAndSwap(&lock->flag, 0, 1)==1);
}
```


### Conclusion

So far, the locks we've seen aren't fair, as it doesn't guarantee it. We can use a queue to schedule threads intelligently. Linux uses futex-based locks which have an inbuilt queue. Queues will also solve another problem, performance. Also, in less-core systems, if the thread holding the lock gets context switched out, the other threads will basically just be doing nothing, and the thread that's supposed to do the work, and free the lock is stuck as it has been context switched, and will wait for its turn again.

We need to increase the performance by yielding.

```c
void init() {
	flag = 0;
}

void lock() {
	while (TestAndSet(&flag, 1) == 1) yield();
}

void unlock() {
	flag = 0;
}
```

## Spin lock vs Sleeping mutex

SLEEPING MUTEX
Threads can go to sleep while waiting for the lock, saving CPU cycles. OS blocks waiting thread, context switch to another thread/process. 

Better to use spinlock if locks are expected to be held for short time, avoid context switch overhead

Better to use sleeping mutex if critical sections are long

### Two Phase Locks

Phase 1 :  Spin for some time
Phase 2 : Sleeping Mutex
Hybrid is almost always better

## Guidelines

When writing multithreaded programs, maintain careful locking discipline
1. Protect each data structure with 1 lock
2. Locks can be coarse grained (1 big fat lock), or fine-grained (many smaller locks)
3. Use locks for both reading and writing

## Counters
```c
typedef struct __counter_t {
	int value;
	pthread_mutex_t lock;
} counter_t;

void init(counter_t *c) {
	c->value = 0;
	pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c) {
	pthread_mutex_lock(&c->lock);
	c->value++;
	pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c) {
	pthread_mutex_lock(&c->lock);
	c->value--;
	pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c) {
	pthread_mutex_lock(&c->lock);
	int rc = c->value;
	pthread_mutex_unlock(&c->lock);
	return rc;
}
```

Next : [[CV's and Semaphores]]
