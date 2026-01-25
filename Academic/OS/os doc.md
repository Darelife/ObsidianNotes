# OS Doc | Group 13

## Introduction

OS Doc is basically just a real time colab doc env, that we're making a prototype of. We aren't implementing all the socket stuff, but it's more about processing the commands from multiple clients, to read/write on a 10x10 grid of words handling concurrent access via message queues, and locks (mutex) for synchronization.

## Architecture

- We have a server.c and a client.c
- Server: We have a thread for each incoming request to ensure non-blocking handling of concurrent clients.
- Client: It reads the commands from the input file (input.txt), and then it sends the request (osdoc_request_t) to the server.
- We use message queues to communicate between the client and server
```c
#define REQ_QUEUE_NAME "/osdoc_req_queue"
#define RES_QUEUE_FORMAT "/osdoc_res_%d"
```

So basically, we have 2 queue formats.
1. A public queue from all the clients to the server (many to one) (osdoc_req_queue)
2. Many queues from the server to the clients individually (one to one)  (osdoc_res_\<id>)

## Data Structures

### The Document

We have a 3D character array = 10x10 grid of words
```c
#define GRID_SIZE 10
#define MAX_STRING_LEN 64
char document[GRID_SIZE][GRID_SIZE][MAX_STRING_LEN];
```

### Grid Synchronization

Parallel to the grid, we have a grid of locks in order for us to lock each word individually.
```c
typedef struct {
    pthread_mutex_t lock;       // Protects this specific struct
    pthread_cond_t can_access;  // Signal when writer finishes or readers leave
    int readers;                // How many clients are currently reading
    int is_editing;             // Flag: 1 if a writer holds the lock
} word_lock_t;

word_lock_t doc_locks[GRID_SIZE][GRID_SIZE];
```

### IPC Messages

We have 2 structs

REQUEST (osdoc_request_t)
The client sends this to the server, in a public queue.

```c
typedef struct {
	long msg_type; // for the mq_send, and mq_recieve: not req, but for the funcs
	int client_id;
	msg_type_t type; // enum basically of the status of the request
	int line; // which row
	int word_pos; // which col
	int line2; // used for swap (row)
	int word_pos2; // used for swap (col)
	char write_string[MAX_STRING_LEN];
	int write_time_ms;
} osdoc_request_t;

```

RESPONSE (osdoc_response_t)
The server responds to each of the clients

```c
typedef struct {
    long msg_type; 
    msg_type_t type;
    char read_value[MAX_STRING_LEN];
    char doc_content[GRID_SIZE * GRID_SIZE * (MAX_STRING_LEN + 1)]; 
    int line;
    int word_pos;
} osdoc_response_t;
```

for all the queues, we first just delete old queues if they remain, and then, we create new queues with permission 666 (read & write for all users)

## server.c

-  We first just create the grid, and initialise the mutex and the cond for each word.
-  We enter into an infinite loop.
- Calls mq_recieve on the request queue, putting the CPU to sleep until a message arrives.
- Checks the client_id in the message. If it's a new id, it adds it to the seen_clients. This is important for the shut down part.
- If req_shutdown is received, it increments the total shutdowns, and if the total_shutdowns == total_seen_clients, server_running = 0, and the loop breaks;
- If it's a normal request, it allocates some heap memory for the request and calls pthread_create

handle_client_request
- We have a queue /osdoc_res_\<client_id> and have it with write only permission
- If it's a write request
	- Lock the mutex for the target
	- Checks if is_editing == 1 or readers > 0
		- if yes, it releases the mutex, and sends res_write_droppped
		- else, it sets is_editing to 1, and releases the mutex
	- Put the string in the document.
	- send a RES_WRITE_SUCCESS
	- sleep for write_time_ms
	- re-acquire the mutex, and is_editing = 0. Broadcast cond.
- if it's a read request
	- lock the mutex.
	- if is_editing == 1, 
		- release the mutex, send RES_READ_DROPPED, and exit
		- else increment readers++, then release the mutex
	- copy the document word and put it in the response
	- RES_READ_SUCCESS
	- re-acquire the mutex, readers--. if readers == 0, broadcast cond
- if it's a swap request
	- if client A locks (0,0) then (1,1). Client B locks (1,1) then (0,0) : deadlock
	- the server orders the locks in a linear manner, from the lower index to the higher index only (L\*10 + W)
	- lock both mutexes (in order)
	- check if either word is busy (is_editing or readers>0)
	- if busy, unlock both, and DROPPED
	- else set is_editing = 1 for both. Swap and send SUCCESS...then sleep..before unlocking for both
- if it's a print_doc request
	- Iterate through all the 100 words
	- lock all the mutex'
	- If is_editing: ans += "???"
	- if free, increment the readers, copy the word, and then decrement readers.
	- send the response back to the client

## client.c

- Sets up the queue (the private response queue) : /osdoc_res_\<client_id>, and opens the server's public request queue.
- Spawns the print_doc_thread

- reads input.txt line by line, uses sscanf , and starts reading the lines
- if the command is write or swap, checks time_ms, and if it's >= 2000, which is the print interval, it sets the global skip_final_snapshot = 1, to indicate that we'll see ??? in the output.

- we first fill the osdoc_request_t
- lock the ipc_mutex, and mq_send
- then, we see the mq_receive response
- unlock the ipc_mutex
- print the message based on response type;

- the print_doc_thread sleeps for 2000ms.
- if client_running == 0 : break
- constructs REQ_PRINT_DOC_READ
- locks the ipc mutex, sends/receives and unlocks.
- Writes the content to output_client\<client_id>.txt
- When the loop breaks, it checks !skip_final_snapshot
- if it was true, the last command was short or a read. It waits for a bit, and then takes a final snapshot for safety

## IPC Synchonization

### Client side (ipc_mutex)
- the main thread and the background thread both share the same message queue handles (res_mq and req_mq);
- if main sends a write, and then, before it can reply, the background thread sends a print request. the main will read the print reply.
- there's an IPC mutex, ensuring that only 1 of them can be in conversation witht he server at a time

### Server side (doc_locks)
- If multiple clients try to write to document\[i]\[j] at the same time, it will cause problems
- use a pthread_mutex_t inside the word_lock_t
- the condition variable is can_access, which is used to efficiently wake up the waiting threads, though here we just simply drop the requests. The condition variable is mainly used to signal the state change if we implement the blocking waits. Here we just look at the readers and the is_editing.

## Shutdown
- the client finishes input.txt
- sets client_running to 0
- joins the print_doc_thread
- sends REQ_SHUTDOWN to server
- cleans up its down queues
