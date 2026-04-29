# Message Passing Interface

Basically for parallel programming, for inter process communication. (often for distributed systems)

MPI processes have unique ranks, to identify themselves, and based on those ranks, the task for each process is defined. (usually in MPI_COMM_WORLD, 0 based indexing)

`mpirun -np 4 ./a.out`
This will spawn 4 processes, running `./a.out`
Then based on the rank logic, they will all differ, but the code running will be the same.

# Communicators

`MPI_COMM_WORLD` is the default communicator. Has all the processes, and ranks

Anyone can then make their own subsets.
Only members of the group can communicate using that group.

In MPI_COMM_WORLD
Rank 0 = P0
Rank 1 = P1
Rank 2 = P2
Rank 3 = P3

In COMM_A, (Only has P0 and P1),
Rank 0 = P0
Rank 1 = P1

In COMM_B, (Only has P2 and P3),
Rank 0 = P2
Rank 1 = P3

P2: `MPI_Send(..., dest=1, COMM_B);`
(P2 sends to P3)

# MPI Calls

We can do point to point communication. (as shown above..`MPI_Send(..., dest = 1, COMM_B)`, and `MPI_Recv`)

We can also do collective communication.
`MPI_Bcast`: One to All
`MPI_Reduce`: All to one (every process in the group must call it. If any one fails to call it, the program will hang. All the processes will send it to a root process, and then, MPI will combine them using SUM, MAX, XOR, etc.)
`MPI_Barrier` : Sync. (All the processes must call it, and it will basically block the processes that have reached it, until all of them call it)

Blocking Calls: 
1. MPI_Send (Blocks till MPI has copied the data out of the buffer) (Non blocking: MPI_Isend)
   `MPI_Send(buf, count, type, dest, tag, comm);`
2. MPI_Ssend (Blocks till the receiver actually starts receiving)
3. MPI_Bsend (Blocks till the data is copied into a user provided buffer)
4. MPI_Rsend (Assumes that the receiver is already waiting...minimal blocking)
5. MPI_Recv (Blocks till the data arrives into the buffer) (Non blocking: MPI_Irecv)
6. MPI_Wait(&request, &status); (blocks until the operation completes)
7. MPI_Test(&request, &flag, &status); flag = 1 (done), flag = 0 (still running)

```c
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);
// code
MPI_Finalize(); // exiting
```

`MPI_Abort(MPI_COMM_WORLD, rank);`

```c
MPI_Send(
	&number, //data
	1, //count
	MPI_INT, //data type
	1, //destination
	0, //tag
	MPI_COMM_WORLD //communicator
)
```
```c
MPI_Recv(
	&number, //data
	1, //count
	MPI_INT, //data type
	0, //source
	0, //tag
	MPI_COMM_WORLD, //communicator
	MPI_STATUS_IGNORE //status
)
```

## MPI Collective

Used for sync, broadcast, scatter, gather, all to all
Also reductions (One member collects data from others, and performs min, max, sum, prod, &&, &, etc)

