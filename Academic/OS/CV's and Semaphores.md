Prev : [[Locks]]
# Condition Variables

Now suppose you are in a multi threaded environment,  and you want to run T2 only after T1 has finished a particular task

Naive Solution : T2 keeps checking periodically if T1 is done 
Wastes CPU cycles, and is inefficient

 > [!Question]
 > Why can't we just add T2 to the waiting queue of T1??
 > 
 > So, I forgot basically...waiting queues are for locks, we'll have to lock the entire T1 thread, and then, add T2 in the waiting queue for the lock, which is a poor system design choice. Mutexes are for critical sections only, and doing this, could also block any other threads using T1.
 
A condition variable is an explicit queue that threads can put themselves on when some state of execution is not as desired.


There are special variables provided by the pthread library : condition variables (cv).
A thread calls the wait function on the cv, after which it gets blocked, and gets added to a list of threads waiting on the cv. Once a thread is done using it, it calls the signal(cv) function, after which one of the waiting threads get ready to run again, and will be scheduled to run in the future.

## Atomicity in wait and signal

| T1                        | T2                                 |
| ------------------------- | ---------------------------------- |
|                           | if (!done) wait(cv)<br>(T2 Blocks) |
| done = true<br>signal(cv) |                                    |
|                           | (T2 resumes)                       |
However, if we have a context switch from T2 to T1 right before (wait(cv)),

| T1                        | T2                             |
| ------------------------- | ------------------------------ |
|                           | if (!done)<br>(CONTEXT SWITCH) |
| done = true<br>signal(cv) |                                |
|                           | wait(cv)                       |
In this case, T2 will forever be sleeping, as basically, T1 will wake up nothing, as T2 hasn't called wait(cv) yet, which won't give any response.

Hence, we need to ensure that it's atomic in nature. We'll have to lock it. We need to lock T1 too, as we can't have T1 just run, if it context switches while T2 is running to T1...as that will send the signal to T2, before the wait.

| T1                                                        | T2                                        |
| --------------------------------------------------------- | ----------------------------------------- |
|                                                           | lock(mutex)<br>if (!done) wait(cv, mutex) |
| lock(mutex)<br>done = true<br>signal(cv)<br>unlock(mutex) |                                           |
|                                                           | unlock(mutex)                             |
1. T2 Holds the lock, checks the condition, and then calls wait.
2. The lock is released only after T2 is added to the waiting list. (the pthread implementation of wait basically also releases the lock, and then reacquires it when it wakes up)
3. T1 acquires the same lock before calling signal ensuring that signal can't happen in between the checking condition and waiting
4. Now basically, when it signals, T2 comes out of the cv's queue, and tries to acquire the lock (if it has context switched to it...in the worst case) again, but it's still held by T1, so, it goes in the waiting queue of the mutex. Then, when T1 gives up the lock, T2 gets woken up again, and tries to access it, and successfully access it.

The codes below will help in understanding what's actually happening...it's from pintos. The code below is of Mesa style, and not Hoare style ie. that is, sending and receiving a signals are not atomic operations.
```c
void
lock_acquire (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));

  sema_down (&lock->semaphore);
  lock->holder = thread_current ();
}

void
cond_wait (struct condition *cond, struct lock *lock) 
{
  struct semaphore_elem waiter;

  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));
  
  sema_init (&waiter.semaphore, 0);
  list_push_back (&cond->waiters, &waiter.elem);
  lock_release (lock);
  sema_down (&waiter.semaphore);
  lock_acquire (lock);
}


void
cond_signal (struct condition *cond, struct lock *lock UNUSED) 
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  if (!list_empty (&cond->waiters)) 
    sema_up (&list_entry (list_pop_front (&cond->waiters),
                          struct semaphore_elem, elem)->semaphore);
}

void
cond_broadcast (struct condition *cond, struct lock *lock) 
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);

  while (!list_empty (&cond->waiters))
    cond_signal (cond, lock);
}
```
## Producer Consumer / Bounded Buffer problem

Producers produce items, and add it to the shared buffer.
The consumers consume the item from the shared buffer.

Producer thread produces and places items into the buffer, waits if the buffer is full.
Consumer makes a signal after making space in the buffer. The consumer thread consumes items from the buffer, and waits if the buffer is empty. The producer signals after producing items.

### Q1 : What's wrong with this code?? (assume size of the buffer = 1)
```c
int loops;
cond_t cond;
mutex_t mutex;

void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        Pthread_mutex_lock(&mutex);          // p1
        if (count == 1)                   // p2
            Pthread_cond_wait(&cond, &mutex); // p3
        put(i);                              // p4
        Pthread_cond_signal(&cond);          // p5
        Pthread_mutex_unlock(&mutex);        // p6
    }
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        Pthread_mutex_lock(&mutex);          // c1
        if (count == 0)                    // c2
            Pthread_cond_wait(&cond, &mutex); // c3
        int tmp = get();                     // c4
        Pthread_cond_signal(&cond);          // c5
        Pthread_mutex_unlock(&mutex);        // c6
        printf("%d\n", tmp);
    }
}
```

1. We are using `if` here instead of `while`, which is a bad design choice, as if it gets context switched right after passing the check, and if the state is undesirable, we need to check again, before proceeding. While ensures that the condition gets checked again, as it only breaks if it checks again and fails the check. Right now, it moves ahead as soon as wait() lets it go, but the while makes it check again.
2. We need to use separate cv's for the producer and the consumer, 1 for the producer to wait, and the other for the consumer to wait, as they don't depend on each other, and are independent. Like, imagine if a producer intends to signal the consumers to wake up....the old code will also cause a producer to wake up, which will cause issues. Same for the consumer.

Here's the correct code (with a MAX = size of buffer)

```c
cond_t empty, fill;
mutex_t mutex;
int loops;

void *producer(void *arg) {
	int i;
	for (i = 0; i<loops; i++) {
		Pthread_mutex_lock(&mutex);
		while (count == MAX) Pthread_mutex_wait(&empty, &mutex); // empty gets signaled by the consumer once there's atleast 1 empty slot
		put(i);
		Pthread_cond_signal(&fill);
		Pthread_mutex_unlock(&mutex);
	}
}

void *consumer(void *arg) {
	int i;
	for (i = 0; i<loops; i++) {
		Pthread_mutex_lock(&mutex);
		while (count == 0) Pthread_mutex_wait(&fill, &mutex); // fill gets signalled by the producer once there's atleast 1 filled slot
		int t = get()
		Pthread_cond_signal(&empty);
		Pthread_mutex_unlock(&mutex);
		printf("%d\n", t);
	}
}
```

### Now, just like signal, there's also a broadcast

In signal, only 1 thread gets activated, but in broadcast, all of them get activated, and check the condition. However this can affect the performance.

## Batch Processing

We wait for N requests to arrive at a time, and then process them all, before waiting for the next batch of N. We only process once we get a batch of N requests.

We can use 2 cv's: 1 for the batch processing to wait, and the other for the requests to wait.

```c
// request thread
lock(mutex)
count++
if (count == N) 
	signal(cv_batch_processor)
while (!batch_started)
	wait(cv_request, mutex)
unlock(mutex)

// batch processor thread
lock(mutex)
while (count < N)
	wait(cv_batch_processor, mutex)
batch_started = true
signal_broadcast(cv_request)
unlock(mutex)
```


# Semaphores

## ChatGPT response

### 🔹 Difference Between Semaphore and Condition Variables in OS

#### 📌 Semaphore
A semaphore is a synchronization primitive used to control access to shared resources.

#### ✔ Key Features:
- Has an integer counter.
- Supports `wait()` (or `P`) and `signal()` (or `V`) operations.
- Can be used for mutual exclusion as well as resource counting.
- If a thread calls `wait()` when the value is zero, it blocks immediately.
- The wake-up behavior is built into the semaphore logic.

### 📌 Condition Variable
A condition variable is used for thread coordination when a particular condition becomes true.

#### ✔ Key Features:
- Does not maintain a counter.
- Must be used with a lock/mutex.
- Uses `wait()` and `signal()`/`broadcast()` operations.
- A thread waits until some condition is signaled.
- When signaled, the waiting thread must recheck the condition, because signaling does not enforce state changes automatically.

### 🔻 Key Differences

| Feature | Semaphore | Condition Variable |
|--------|-----------|-------------------|
| Has integer counter | ✔ Yes | ✘ No |
| Used for resource counting | ✔ Yes | ✘ No |
| Works alone | ✔ Yes | ✘ Needs a mutex |
| Automatically tracks wake-up logic | ✔ Yes | ✘ No |
| Can enforce mutual exclusion | ✔ Yes | ✔ Yes (but with mutex) |
| Waiting threads re-check condition | ✘ No | ✔ Yes |
| Typical usage | Producer–consumer, bounded buffer | Waiting for state change |
### 🧠 Intuition

- **Semaphore** = gatekeeper that counts slots  
  → Blocks when count is zero, signals when slots open.

- **Condition Variable** = doorbell  
  → Wait until someone rings, but you check again because conditions may have changed.

---


## Notes

1. Semaphores have a counter
2. Two functions : Down (wait) and Up (post)
3. Down basically decrements the counter by 1, and blocks the calling thread if the resulting value is negative.
4. Up increments the counter, and wakes up any one thread that's blocked on the semaphore.

```
wait(S):
    S.value = S.value - 1
    if S.value < 0:
        block the calling thread
```

if s == -4, and a thread does wait(), the new value becomes -5. It sees -5, and since it's negative, it blocks the thread.

```
signal(S):
    S.value = S.value + 1
    if S.value <= 0:
        wake up one blocked thread
```

> [!info]
> <= 0 here cuz, if the value was -1 before ie. 1 thread was waiting, and then, when we do up(s), the value becomes 0. But we still need to wake up the 1 thread, and we know that we have space, cuz some thread that was using it just freed it. So, that's why, we are doing it for S.value <= 0...as once this operation is over, we can clearly state that, none of the threads are waiting, and all our resources are full.

now, s == -5. If a thread does up, s = -4. If the value is <= 0, we have to wake up one of the blocked threads.

if the value is -X, X = number of threads waiting.

if s > 0, there are free resources available. If it's == 0, no free resources, but no one is blocked. If it's < 0, threads are waiting. and the magnitude is the number of threads that are waiting. 

## Semaphore as a lock (Binary Semaphores)

```c
sem_t m;
sem_init(&m, 0, X) // initialize m to X (Q: What should X be??)

sem_wait(&m); // request for a lock
// critical section goes here
sem_post(&m); // release the lock
```

So, locks can only have 2 states: taken and not taken. 

How it works
`X = 1`
Thread 0 will call sem_wait(), which will return, cuz the semaphore is initially 1 (it's now 0). Now, while doing the critical stuff, suppose Thread 1 calls sem_wait(). This will cause it to be -1, causing it to put Thread 1 to sleep. Now, thread 0 will start running again. Once it ends, it will call sem_post(&m), which will make it back to 0, and along with it, also wake Thread 1. Then, sem_post will return. Now, when Thread 1 runs again, it will directly jump back to that state. It will return the output from sem_wait, that was requested a while ago. Then, it will do the rest of the stuff.


## Semaphores for Ordering (T1 -> T2)

```c
sem_t s;

void *child(void *arg) {
	printf("child\n");
	sem_post(&s);
	return NULL;
}

int main(int argc, char *argv[]) {
	sem_init(&s, 0, X); // What should X be?
	printf("parent: begin\n");
	pthread_t c;
	Pthread_create(&c, NULL, child, NULL);
	sem_wait(&s); // wait here for the child to finish.
	printf("parent: end\n");
	return 0;
}
```

> [!info] What should X be?
> The answer for the ordering case is 0, and the answer for locking, is 1. 
> However, why can't we keep it as 1, for ordering too??
> 
> It's because, we can't take any chance with the parent accidentally taking the semaphore. We need to ensure that the child runs first. Since both run in parallel, we have to force the parent to wait. So, we'll do it exactly like how processes/threads do it. So, that's why the wait is directly in the parent section. The parent will cause s = -1, and will then wait for s to be >= 0. Which will happen only when the child finishes its job.
> 
> Stuff from the slides:
> With the lock, it was 1, because we were willing to have the lock locked (given away) immediately after initialization, however with the ordering case, it was 0, since there was nothing to give away at the start


## Threads waiting for each other (T1, T2)

Suppose we have 2 threads, who both want to do step 1 and step 2 in the processing. We want both of them to finish the first step, before either can start with the step 2. We'll use 2 semaphores, T1done and T2done (both initialized to 0)

Now, the question is, after finishing step 1, should the threads do, down(T2done) and then, up(T1done)? or up(T1done) and then, down(T2done)??

So, lets try it out.
T1done = Signal that T1 is done with step 1, and T2done = signal that T2 is done with step 1

Now, we'll do up first in both of them, cuz that will give the other thread the permission to run basically...it's saying that Im done with my Step 1, you can do step 2 now. And similarly, the other thread will also do the same thing

| T1 does step 1                                                                        | T2 does step 1 |
| ------------------------------------------------------------------------------------- | -------------- |
|                                                                                       |                |
| up (T1done)                                                                           | up(T2done)     |
| down(T2done)                                                                          | down(T1done)   |
| // down is to ensure <br>that like, it doesn't <br>continue till T2 done<br>gets >= 0 |                |
|                                                                                       |                |
| T1 does step 2                                                                        | T2 does step 2 |
If we do down before up, it will cause a deadlock, as the count will never be able to increase

> [!question] Can we generalize this for N threads??
> 
> (this is used in parallel computing btw)....something about OpenMP / MPI barrier

So, for N threads again, we have to first do up, before all the downs, but we can't have N locks.

so, lets initialize a few things
```c
int count = 0;
sem_t mutex; // protects the updation of the count value
sem_t barrier; // blocks all the threads till they all reach the barrier
int N = <number of threads>;

sem_init(&mutex, 0, 1); // sem_t, pshared (int), value (unsigned int)
// if pshared is 0, it's shared in the same process, otherwise, diff processes
sem_init(&mutex, 0, 0); // it's 0, cuz this one's the actual ordering semaphore, the other one is the locking semaphore
```

now, the thread func
```c
void *thread_func(void *arg) {
	step1();
	barrier_point();
	step2();
}

void barrier_point() {
	sem_wait(&mutex);
	count++;
	if (count == N) {
		for (int i = 0; i<N; i++) sem_post(&barrier); // unlock all threads
	}
	sem_post(&mutex);
	sem_wait(&barrier) // each thread will wait till the barrier is open.
	// also, if the barrier just got opened, like, it was +1 before this wait,
	// so, now, it's back to 0, and it will allow this thread to also pass.
}
```


## Producer Consumer

So, we'll need a sem_empty that will show the number of empty slots...so, that will be N. Like, that lock, will basically be there for checking if all the slots are already locked or not.
We'll also have a sem_filled that will indicate the number of filled slots in the buffer.


Nvm, sem_empty is for the producers. Like, if the buffer is full, we don't want the producers to push, and just wait. and sem_filled is for the consumers...like, again, if it's > 0, that means that there's atleast something in it, that the consumer can pull.

```c
// Producer
down(sem_empty);     // blocks if buffer full
produce item
up(sem_filled);     // wake consumer

// Consumer
down(sem_filled);   // blocks if buffer empty
consume item
up(sem_empty);      // wake producer
```

> [!question] Now, here's a question. Is it safe?? like, does this avoid race??
> 
> Nope, it's not safe. We need a lock within it. We need to ensure that no other producer can touch the buffer while we are accessing it. 
> 

Now, here's another question. Which one of these should we do??

```c
// Producer
down(mutex);
down(sem_empty);     // blocks if buffer full
produce item
up(sem_filled);     // wake consumer
up(mutex);

// Consumer
down(mutex);
down(sem_filled);   // blocks if buffer empty
consume item
up(sem_empty);      // wake producer
up(mutex);
```

or

```c
// Producer
down(sem_empty);     // blocks if buffer full
down(mutex);
produce item
up(mutex);
up(sem_filled);     // wake consumer

// Consumer
down(sem_filled);   // blocks if buffer empty
down(mutex);
consume item
up(mutex);
up(sem_empty);      // wake producer
```

So, if we do down(mutex) first, then, if the down(sem_empty) also happens in producer, the mutex will still keep blocking the consumer from accessing the buffer, and it will cause a deadlock, as the sem_empty can only be upped by a consumer. So, that's why the answer is option 2.

## Guidelines for using semaphores

About the equivalents,
down ~ wait
up ~ signal
however, semas don't have an equivalent of broadcast.

We need separate semaphores for signalling and mutual exclusion

We can use semaphores instead of storing the empty_slots, and sem_filled instead of items_in_buffer, but we can't modify/read the semaphore counters directly, we can just do up, or down.

We need to be careful with deadlocks, as down doesn't release locks. None of the locks will get released when a thread  gets blocked due to a down operation.

We also need to pay attention to the initial value of the semaphore

## Batched Processing

### CV version

#### Request thread

```c
lock(mutex)
count++

if (count == N)
    signal(cv_batch_processor)

while (not batch_started)
    wait(cv_request, mutex)

unlock(mutex)
```

#### Batch processor thread

```c
lock(mutex)

while (count < N)
    wait(cv_batch_processor, mutex)

batch_started = true
signal_broadcast(cv_request)

unlock(mutex)
```

### Sema version

mutex = 1 initially since it's a lock
sem_batch_processor = 0....as it waits for N threads to arrive
sem_request = 0....All N threads wait on it, and when the batch starts, the batch processor does up, N times to unblock them all.

#### Request Thread

```c
down(mutex)
count++

if (count == N)
    up(sem_batch_processor)

//while (not batch_started)
//    wait(cv_request, mutex)
up(mutex)
down(sem_request)
```

#### Batch processor thread

```c
down(sem_batch_processor)
for (int i = 0;i<N; i++) {
	up(sem_request) // unblock all N threads that were waiting
}
```

> [!info] Do we have to use a loop to do N ups, to wake them up??

Now, instead of waking up N threads like this, there's another way.

#### Request Thread

```c
down(mutex)
count++

if (count == N)
    up(sem_batch_processor)

//while (not batch_started)
//    wait(cv_request, mutex)
up(mutex)
down(sem_request)
up(sem_request)
```

#### Batch processor thread

```c
down(sem_batch_processor)
up(sem_request)
```

Once we get N threads together, wake 1 up. After that's done, then, make that request thread wake the next, and so on.

[[IPC]]