[[Physical Layer|Previous Slide]]
# Client Server
Regular Stuff (HTTP, IMAP, FTP)

## Server
- Always On, with a permanent IP

## Client
- Contacts the server

# Peer to Peer
- no server
- peers are intermittently connected, and change IPs
- eg: P2P file sharing (bit torrent)

# Sockets
- procs can send/receive msgs to/from its socket

# TCP & UDP
## TCP
- reliable transport between sending and receiving process
- flow control: sender won’t overwhelm receiver 
- congestion control: throttle sender when network overloaded
- connection-oriented: setup required between client and server processes
- does not provide: timing, minimum throughput guarantee, security

## UDP

- unreliable data transfer between sending and receiving process
- does not provide: reliability, flow control, congestion control, timing, throughput guarantee, security, or connection setup.

# Securing TCP

Vanilla TCP and UDP sockets don't have any encryption, and everything is sent in cleartext.

**TLS** provides encrypted TCP connections, data integrity, and end to end point auth.

## HTTP using TCP

- Client initiates TCP connection to port 80.
- Server accepts
- Messages are exchanged
- TCP connection is closed.

HTTP is stateless (doesn't maintain any info about past client requests)
Protocols that maintain state are complex (past history must be maintained). If the client/server crashes, their views of the state might be different, and must be reconciled.

## Two types of HTTP connections

- Persistent: TCP connection is opened to the server, multiple objects can be sent over a single TCP connection. Connection is closed
- Non Persistent: Single object per connection. Downloading multiple objects requires multiple connections

# HTTP Request Message Anatomy
1. Request Line: Mentions the method, url, and the version of the HTTP
2. Header Lines: Provides the meta data info about the request 
3. Everything is delimited by `cr` (carriage return `\r`) and `lf` (line feed `\n`)
4. Header lines and body is separated by another `cr` and `lf` group
5. Entity Body: This is where data is placed if you are sending information to the server (e.g., uploading a file or submitting form data in a POST request). If no data is being sent (as in a simple GET request), this part may be empty.

## Common HTTP Protocols
1. GET: To retrieve data
2. POST: Used to send data to the server
3. HEAD: Similar to GET, but the server only returns the headers. This is a highly efficient way to check the status or size of a resource without downloading the content itself.
4. PUT: Used to upload a new resource or replace an existing one at a specific URL.

## Stateless Nature of HTTP

HTTP is stateless. This means that the server treats every single request as a brand-new interaction, having no "memory" of previous requests. While this design makes the protocol simple and robust, it is problematic for modern web experiences like e-commerce (where you need a persistent shopping cart) or user logins.

A solution to this is to use cookies. 
Cookies act as a bridge, allowing the server to "remember" the client across multiple transactions. The process works through four components:
1. Response Header: The server sends a Set-Cookie header to the client to initiate the state.
2. Client Storage: The user's browser stores this cookie in a local file.
3. Request Header: On subsequent requests, the browser automatically includes this Cookie header.
4. Backend Database: The server uses the unique cookie ID to look up the client's information (like their shopping cart or user profile) in its database.

# Web Caching

## The Problem: The Bottleneck

You have a scenario where the "access link" (the pipe connecting your local network to the public internet) is too narrow for the amount of data trying to flow through it.

- The Bottleneck: The utilisation of the access link is at .97 (or 97%). In networking, when a link is this busy, it causes massive "queuing delays," meaning data packets are stuck waiting in line, turning a 2-second transaction into one that takes minutes.

### Solution 1 : Brute force (Buy a faster link)

This is the expensive approach. You simply upgrade your access link from 1.54 Mbps to 154 Mbps.
- **Result:** By increasing the capacity, the utilisation drops from .97 to .0097.
- **Performance:** The congestion disappears, and the end-to-end delay drops from minutes to milliseconds.
- **Trade-off:** While fast, this is often cost-prohibitive.

### Solution 2: The "Smart" Approach (Install a Web Cache)

Instead of widening the pipe, you install a local server (the cache) on your own network. The cache acts as a middleman, serving requests locally whenever possible. Here is the logic behind why it works:
1. **The Hit Rate:** You assume a "cache hit rate" of 0.4 (or 40%). This means 40% of the time, the user gets the data instantly from the local cache, and only 60% of the requests need to go out through the slow access link. 
2. **Traffic Reduction:** Because 40% of requests never leave your network, the traffic on the access link drops to 60% of its previous level (0.6×1.50 Mbps=.9 Mbps).
3. **Utilization Improvement:** Now, the utilization of that same 1.54 Mbps link is only .58 (0.9/1.54).
    - Crucially, at .58 utilization, the queuing delay is negligible (milliseconds), unlike the delay caused by .97 utilization.
4. **Final Delay:** Your total delay becomes a weighted average of the fast cache hits and the slower original internet requests, resulting in roughly 1.2 seconds.

# Browser Caching

When browsers sends a get request, it basically sends a conditional get request
It includes a specific header in the HTTP request: `If-modified-since: <date>`
- **If it hasn't changed:** The server sends a **304 Not Modified** response. Crucially, it sends **no object data**, saving bandwidth and time.
- **If it has changed:** The server sends a **200 OK** response along with the new data.

# HTTP/1.1

HTTP/1.1 introduced multiple pipelined gets over a single TCP connection. The server would respond in FCFS. This led to small objects getting trapped behind larger objects (HOL : Head of Line blocking). 

If a packet containing a piece of "Object 1" goes missing, the TCP stack at your browser receives the packets for "Object 2" and "Object 3," but it cannot pass them to the browser application. It has to put them in a buffer and wait.

--- 
(pure gemini generated stuff below)

### **1. HTTP/1.0: The "One and Done" Era**

- **The Mechanism**: Every single object (image, script, HTML) required its own separate TCP connection.
- **What was Wrong**: Establishing a TCP connection involves a "handshake" (extra round-trips), making this incredibly slow for modern websites with dozens of images.

### **2. HTTP/1.1: Pipelining and the "Stupid" Buffer**

- **The Mechanism**: Introduced **persistent connections** and **pipelining**, allowing the browser to send multiple "GET" requests over one connection without waiting for each response.
- **What was Right**: Saved the time wasted on multiple TCP handshakes.
- **What was Wrong (HOL Blocking)**: The server MUST respond in a strict First-Come-First-Served (FCFS) order.

> **Addressing Your Question**: If a packet for Object 1 is lost, the server **cannot see your browser's struggle.** It blindly continues to "push" the data for Object 2 and 3 onto the network wire.
> 
> Your computer is a passive receiver; it can't tell the server, _"Stop Object 2, just fix Object 1!"_ because the protocol doesn't have a command for that. Your browser receives Object 2 and 3 but **cannot use them**; they sit in a "TCP Buffer" waiting for the missing piece of Object 1 to arrive so the "stream" can be reconstructed in order. This is the definition of **Head-of-Line (HOL) Blocking**. Now basically, as soon as the browser realizes that object 1 hasn't loaded, it loads object  

### **3. HTTP/2: Framing and Prioritization**

- **The Mechanism**: Instead of one long stream, HTTP/2 breaks objects into small **Frames** and **interleaves** them.
    
- **What was Right**: If a frame for Object 1 is lost, the server can actually "skip" it for a moment and keep sending frames for Object 2 and 3.
    
- **The Remaining Problem**: Even though the _application_ (HTTP) is now smarter, the _transport_ (TCP) still isn't. If a TCP packet is lost, the entire "pipe" still stalls because TCP itself insists on a strict order.
    

### **4. HTTP/3 (QUIC): The Solution to the Buffer**

- **The Mechanism**: Switches from TCP to **UDP**.
    
- **What was Right**: Since UDP doesn't care about "strict order" for the whole connection, HTTP/3 handles reliability **per object**.
    
- **Solving Your Inefficiency**: In HTTP/3, if Object 1's data is missing, it **only** stalls Object 1. Object 2 and 3 can be handed to the browser immediately as they arrive, without ever getting stuck in a "waiting room" buffer.
    
- **Speed**: It integrates security (TLS) into a single 1-RTT handshake, or even 0-RTT for returning users.