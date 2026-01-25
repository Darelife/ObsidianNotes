`OS Lab : 2/9/25 : Slide 8`
prev : [[Scheduling]]

They are basically light weight processes.
A process might want to run multiple copies of itself...multiple copies can run in parallel on multiple CPU cores.

If we fork a process, we'll copy the entire memory segment and all, but all threads use the same code, data, and heap, but different stacks. 
Each thread has a separate **PC**, and **CPU context** during execution.

## Concurrency vs Parallelism

In concurrency, we are rapidly switching between different processes/threads giving the illusion of parallelism, while in parallelism, we are actually doing things in parallel. With multiple threads we can have parallelism, upto a limit, as there is a limit to the number of cores in the CPU, and even if there's no parallelism (single core), concurrency of threads ensures effective use of CPU when one of the threads blocks. Example: If one thread of a server blocks on I/O read, another can still run.

## POSIX Threads

In linux, the pthreads library allows the creation of multiple threads in a process.
Each thread is given a starting function where its execution begins, and they act independently from the parent process. The parent may or may not wait for the thread to finish. 

```cpp
void f1() {
	...
}

void f2() {
	...
}

int main() {
	...
	pthread_t t1, t2;
	pthread_create(&t1, .., f1,...);
	pthread_create(&t2, .., f2,...);
	...
	pthread_join(t1,...); //optional
	pthread_join(t2,...); //optional
}
```

# Thread Creation and Wait

```c
#include <pthread.h>

int pthread_create(
    pthread_t *restrict thread,
    const pthread_attr_t *restrict attr,
    void *(*start_routine)(void *),
    void *restrict arg
);
```

```cpp
#include <pthread.h>

int pthread_join(
    pthread_t thread,
    void **retval
);

```

## Example Code
```cpp
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

void *mythread(void *arg) {
    printf("%s\n", (char *) arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p1, p2;

    printf("main: begin\n");

    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");

    // join waits for the threads to finish
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("main: end\n");
    return 0;
}

```

Compile with `-lpthread` flag

In this code, B could be before A or after A....depends on the scheduler, we aren't forcing it to be in any order.

## Threads with Shared Data
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int counter;

void *incrementBy100000() {
	for (int i = 0; i<100000; i++) counter++;
}

int main() {
	counter = 0;
	pthread_t t1, t2;
	printf("main: begin\n");
	
	pthread_create(&t1, NULL, incrementBy100000, NULL);
	pthread_create(&t2, NULL, incrementBy100000, NULL);
	
	printf("Final Counter Value %d\n", counter);
	print("main: end\n");
	return 0;
}
```

What do you think will happen??
My answer is 200000....my reason is....the stuff in the data segment is shared. 

So, the correct answer is....something less than 2 lakh. Because like, there's a read and a write. What it reads may be different than what is there when it's just about to right.

Counter is 0 initially. T1 loads the counter into the reg, increments the reg, and then suddenly context switch (it still hasn't put 1 in the memory). T2 does it , and lets say that it manages to store it in the memory, before switching back to T1, (so, now it's 1 in the memory), Now, T1 is about to put the value of 1 into the memory, as it has 1 in its reg, cuz it read the old value of counter. So basically we'll get 2 lakh, only if each Read-Write pair doesn't get interrupted. We hav eto do this via mutual exclusion, using locks.

Basically it's via a mutex. Here we are using locks (a form of it). The locks basically as the name suggests, locks and then unlocks the stuff. 
```cpp
int counter;
pthread_mutex_t m;

void *incrementBy100000() {
	for (int i = 0; i<100000; i++) {
		pthread_mutex_lock(&m); // Acquire lock
		counter++; // Critical section
		pthread_mutex_unlock(&m); // Release lock
	}
}
```

## Implementing Locks via Semaphores

Think of it like this...we have a counter = 1 initially. 
Now, we have a rule. If the counter = negative, it will be blocked.

There are 2 functions : Down and Up
**Down/Wait** : decrements the counter by one, blocks the calling thread if the resulting value is negative
**Up/Post** : increments the counter by one, wakes up any one thread that is blocked on the semaphore

There can be deadlocks too btw. Like, suppose we have 2 threads
T1 does a particular lock L1, and then, T2 executes, and lock L2....then, T1 comes back and tries to use L2, but it has already been locked, and also like, if T2 also requests L1...which has been locked....so....now we have a deadlock!!!!



!! Next Assignment
- next_thread_to_run() in thread.c
- Mainly need to check thread.c, thread.h, synch.h, and synch.c

[[Virtual Memory and Paging]]