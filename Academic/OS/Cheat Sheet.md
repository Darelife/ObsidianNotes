# Pre Midsem

- CS register's bottom 2 bits hold the mode of the ring
# Post Midsem

### Locks
- Mutex is from the posix pthread library. A variable is passed as a parameter to lock and unlock. 
- M1 : Pause / Unpause interrupts, during locking. But, if one core disables interrupts, other cores can still access the lock. And disabling interrupts can also cause the OS to delay important tasks like the DMA completing disk read.
- M2 : Spin wait. Not atomic. So, after the verification part, that the lock is no longer held, if there's a context switch the other thread can take the lock, but the current thread if context switched back to, will assume that it can acquire the lock now. 
- M3 : while(TestAndSet(&isLocked, 1)); Atomic instruction. Directly sets value to 1, and returns the old value. If it's 1, doesn't have lock. As soon as it becomes 0, as it will still keep writing 1, it will return the old value of 0, and tell us that we've acquired the lock now.
- M4 : CAS ( while(!CompareAndSwap(&lock->flag, 0, 1)); ) if val = 0, write 1 and return true, else return false
Make locks do spinlock (`while(TestAndSet(&isLocked, 1));` ) for a bit, and then, put them to sleep via `while(TestAndSet(&isLocked, 1)) yeild();`) , and context switch
1. Protect each data structure with 1 lock
2. Locks can be coarse grained (1 big fat lock), or fine-grained (many smaller locks)
3. Use locks for both reading and writing

### CV
- Shouldn't add T2 to the waiting queue of T1 directly, as it's a very poor design choice. We'll literally have to put T1 and T2 in the same lock, and then, force T1 to start first somehow. It's inefficient, and isn't the best choice.
- when a thread does wait(cv, mutex), it gets put in the queue of cv (and the mutex is in it, to release the mutex to avoid dead locks). Then, when signal(cv) comes, it selects the thread to run again. `lock(mutex); if (!done) wait(cv, mutex)`, and then, in the other thread, `lock(mutex); done = true; signal(cv); unlock(mutex);` 