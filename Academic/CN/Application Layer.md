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

TCP & TLS : 2 serial handshakes
QUIC : 1 RTT Handshake

Quic requests the first session ticket from the server, when it starts. Later on, when it requests for another connection, it makes 0-RTT handshakes, as it just sends that session token again, confirming encryption, and authentication for the new connection.

# TCP
1. Client sends SYN to Server
2. Server will send SYN-ACK
3. Client sends ACK-Rec

If a data packet fails to send, TCP will resend it

# UDP
Just sends the data, without establishing connection

# QUIC
  
HTTP/3 uses QUIC (developed by Google, standardized by IETF).

Made by Google

  
HTTP/2 multiplexes streams but runs over TCP. 
If a TCP packet is lost, TCP blocks delivery until retransmission, 
causing Head-of-Line blocking across all streams.

So, again,
HTTP2 (not HTTP2, TCP can't process it btw...but HTTP2 uses TCP...so, I just said HTTP2) can't process the 2nd stream, till the first stream gets sorted out (retransmits the data). HTTP2 can process multiple streams, but with TCP, if 1 packet gets lost, it pauses the delivery system, until the missing packet arrives. So, all the streams get blocked.



HTTP3 using QUIC, implements multiple streams in the transport layer

QUIC also reduces the number of handshakes (just does TLS, and in that too, TLS 1.3, which allows it to use the TLS certificate to reuse it later, avoiding another TLS handshake)

If we switch ip (eg: mobile data to wifi). QUIC has a connection migration feature. Connections are identified using a Connection ID rather than IP/port, 
allowing the connection to continue even if the client's IP changes 
(e.g., switching from mobile data to WiFi).



# E Mail

- User Agent
- Mail Agent
- Simple Mail Transfer Protocol (SMTP)

## User Agent
- Like, Outlook, Gmail, etc
- Outgoing and Incoming mails are stored on the server

## Mail Agent
- Mailbox contains incoming messages for the user
- Message queue contains outgoing mails

## SMTP Protocol
- Client : Sending Mail server
- Server : Receiving Mail server

# SMTP RFC
- Uses TCP, (from client mail server: Port 25)
- Direct transfer from the sending server (acting like a client) to the receiving server
- 3 Phases of transfer
	1. SMTP hanshaking (greeting)
	2. SMTP transfer of messages
	3. SMTP closure
- Commands/response interaction like HTTP 
	- Commands: Ascii text
	- Response: Status code and phrase


(everything is on port 25 btw)

TCP HANDSHAKE
- Initiating TCP Connection 
- (client -> server), (server -> client)
- TCP Connected initiated
- (client -> server) : ACK Rec

SMTP HANDSHAKE
- (server -> client) : 220 (service ready) (It tells the sending client, "I am an SMTP server, I am awake, and I am ready to start the handshake".)
- (client -> server) : HELO \<identifier> (Tells the server who the client is)
- (server -> client) : 250 Hello (Tells the client that the server has acknowledged who it is, and can proceed now)

SMTP Transfer happens now


## Eg: SMTP (Alice sends mail to Bob)

1. Alice uses UA to compose an email message to bob@abc.com
2. Alice's UA sends a message to her mail server using SMTP. It's placed in a queue
3. Client's side of SMTP at mail server opens a TCP connection with bob's mail server.
4. SMTP client sends Alice's message over the TCP connection
5. Bob's mail server places the message in bob's mailbox
6. Bob invokes his User Agent to read the message.

S: 220 hamburger.edu
C: HELO crepes.fr
S: 250 Hello crepes.fr, pleased to meet you
C: MAIL FROM: \<alice@crepes.fr>
S: 250 alice@crepes.fr...Sender ok
C: RCPT TO: \<bob@hamburger.edu>
S: 250 bob@hamburger.edu...Recipient ok
C: DATA
S: 354 Enter mail, end with "." on a line by itself
C: Do you like ketchup?
C: How about pickles?
C: .
S: 250 Message accepted for delivery
C: QUIT
S: 221 hamburger.edu closing connection

## SMTP vs HTTP
- Both use ASCII for commands and responses
- Status Codes: Both use numeric status codes and phrases to communicate success or failure (e.g., HTTP's 200 OK vs. SMTP's 250 Hello).
- Layering: Both operate at the Application Layer and traditionally rely on TCP at the transport layer for reliable data delivery.

| **Feature**         | **HTTP (Web)**                                                         | **SMTP (E-mail)**                                                                                      |
| ------------------- | ---------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------ |
| **Primary Action**  | **Client Pull**: You "pull" information from the server.               | **Client Push**: You "push" information to the receiving server.                                       |
| **Object Handling** | Each object (image, text) is encapsulated in its own response message. | Multiple objects (attachments, text) are sent in a single **multipart message**.                       |
| **Data Format**     | Can handle any type of data (binary, images, video) natively.          | Originally restricted to **7-bit ASCII** for both headers and body.                                    |
| **Persistence**     | Can be non-persistent (HTTP/1.0) or persistent (HTTP/1.1 onwards).     | Uses **persistent connections** by default to transfer mail.                                           |
| **Termination**     | Uses Content-Length or closing the connection to end a message.        | Uses a specific sequence: **CRLF.CRLF** (a period on a line by itself) to signal the end of a message. |

SMTP: protocol for exchanging e-mail messages, defined in RFC 5321 (like RFC 7231 defines HTTP)

RFC 2822 defines syntax for e-mail message itself (like HTML defines syntax for web documents)

QUIC is RFC 9000
QUIC version 2 is RFC 9369

Request for comment
RFC was created by the Internet Engineering Task Force (IETF). They describe the methods


- **SMTP**: delivery/storage of e-mail messages to receiver’s server
- **Mail access protocol**: retrieval from server
	- **IMAP**: Internet Mail Access Protocol [RFC 3501]: messages stored on server, IMAP provides retrieval, deletion, folders of stored messages on server
	- **HTTP**: gmail, Hotmail, Yahoo!Mail, etc. provides web-based interface on top of STMP (to send), IMAP (or POP) to retrieve e-mail messages


# DNS (Domain Name System)

DNS Services:
- Hostname to IP translation
- Host aliasing (canonical, alias names) (CNAME stuff like, bpgcmap.vercel.com to cm.darelife.xyz)
- Mail server aliasing
- Load distribution (multiple IP addresses point to a single name) (A records or AAAA records for ipv6, to store the various ips for a single domain host...goes through it in a round robin format)

Root DNS server -> .com DNS server (TOP LEVEL DOMAIN servers) -> yahoo.com DNS server (Authoritative DNS server)


Iterative approach : Local nameserver -> ISP (recursive till here) -> Root -> if root fails, tells ISP to ask TLD -> if that fails, it tells ISP to ask the Authoritative DNS server.

In recursive, the Root would have asked TLD directly and all....but it's not possible, as the root is very busy, and handles 60-100 Billion requests daily.
72% of the queries are unwarranted, including queries for websites that don't exist.

There are 13 named Root servers (A to M), but over 2000 active instances running worldwide using anycast routing

### Caching DNS info
- once any name server learns a mapping, it caches the mapping for some time (TTL : time to live)
- cached entries might be out of date. If an internet host changes its IP. We might not know till all the TTLs expire
- Often, in local name servers, the TLD servers are cached, to avoid going to the root servers

### DNS records
- A type : (name = hostname; value = ip address)
- CNAME type : (name = alias for some canonical real name)
- NS type : (name = domain, value = hostname of the authoritative name server for this domain)
- MX type : value is the SMTP mail server associated with the name

---

# DNS Protocol Message Format

Both DNS **queries** and **replies** share the same message format, consisting of a fixed header followed by variable-length data sections.

## 1. Message Header

The header contains critical control and tracking information (fixed size).

| **Field**            | **Description**                                                                              |
| -------------------- | -------------------------------------------------------------------------------------------- |
| **Identification**   | 16-bit number; the reply uses the same number as the query to match them up.                 |
| **Flags**            | Defines behavior (e.g., query vs. reply, recursion desired/available, authoritative answer). |
| **# Questions**      | Number of entries in the question section.                                                   |
| **# Answer RRs**     | Number of Resource Records in the answer section.                                            |
| **# Authority RRs**  | Number of Resource Records in the authority section.                                         |
| **# Additional RRs** | Number of Resource Records in the additional info section.                                   |

## 2. Variable Data Sections

These sections follow the header and contain the actual payload of the message.

- **Questions**: Contains the `name` and `type` fields for the query.
- **Answers**: Contains the Resource Records (RRs) providing the response to the query.
- **Authority**: Contains Resource Records pointing toward authoritative servers for the domain.
- **Additional**: Contains "helpful" information (often extra RRs) that may be useful for the requester.

### 1. The Flags Field (16 bits)

The flags field acts as the "instruction manual" for how the DNS server should process the message.

|Flag Bit|Name|Function|
|---|---|---|
|**QR**|Query/Response|0 for query, 1 for response.|
|**Opcode**|Operation Code|Usually 0 (standard query).|
|**AA**|Authoritative Answer|Set to 1 if the server is authoritative for the domain.|
|**TC**|Truncated|Set to 1 if the message was too long and cut off.|
|**RD**|Recursion Desired|Set by client if they want the server to do the work.|
|**RA**|Recursion Available|Set by server to indicate it supports recursion.|
|**RCODE**|Response Code|Indicates error status (e.g., 0 = No error, 3 = Name error/NXDOMAIN).|

Export to Sheets

---

### 2. Resource Record (RR) Structure

Everything after the header—in the questions, answers, and authority sections—follows a standard RR format:

- **Name:** The domain name being queried.
    
- **Type:** The type of record (e.g., `A` for IPv4, `AAAA` for IPv6, `MX` for mail).
    
- **Class:** Almost always `IN` (Internet).
    
- **TTL (Time to Live):** How long this record can be cached (in seconds).
    
- **RDLength:** Length of the data field.
    
- **RData:** The actual data (e.g., the IP address).
    

---

### 3. A Practical Example: DNS Lookup

Imagine you type `example.com` into your browser. Here is the lifecycle of that packet:

**The Query (Client to Server)**

- **Header ID:** `0x1234` (The ID the client assigns to track this request).
    
- **Flags:** `RD = 1` (Recursion desired).
    
- **Question:** Name: `example.com`, Type: `A`, Class: `IN`.
    

**The Reply (Server to Client)**

- **Header ID:** `0x1234` (Must match the query ID).
    
- **Flags:** `QR = 1`, `RA = 1` (Recursion available).
    
- **Answer Section:**
    
    - **Name:** `example.com`
        
    - **Type:** `A`
        
    - **TTL:** `3600` (1 hour)
        
    - **RData:** `93.184.216.34` (The IPv4 address).
        

This tells your computer: "I am replying to your request `0x1234`, recursion was used, and for `example.com`, the IP address is `93.184.216.34`."


### What does the TLD store necessarily
1. Name servers (NS)
   networkutopia.com   NS   dns1.networkutopia.com
2. Glue A Record (A record of the NS)
   dns1.networkutopia.com   A   212.212.212.1


## 1. Multimedia and Video Streaming

Video traffic is a major consumer of Internet bandwidth. To handle this, applications use specialized encoding and distribution methods.

- **Video Encoding:** Video is a sequence of images displayed at a constant rate. Coding reduces data usage by exploiting redundancy:
    
    - **Spatial Coding:** Reduces redundancy within a single image (e.g., sending repeated color values once).
        
    - **Temporal Coding:** Reduces redundancy between frames by sending only the differences from the previous frame.
        
    - **Bit Rates:** Encoding can be Constant Bit Rate (CBR) or Variable Bit Rate (VBR), where the rate changes based on the amount of spatial/temporal coding required.
        
- **Streaming & Buffering:** To handle variable network delays (jitter) and packet loss, clients use buffering to ensure continuous playout.
    
- **DASH (Dynamic, Adaptive Streaming over HTTP):**
    
    - Servers divide video into chunks and encode them at multiple rates.
    - Manifest file provides the urls for various chunks
    - Files are replicated in various cdn chunks
    - Clients monitor their own bandwidth and periodically request the chunk that provides the maximum sustainable quality.
    - Client decides when to request a chunk, what the encoding rate should be, or where to request the chunk from (can request from the URL that's close to the client or has the highest bandwidth)
    - Streaming = encoding + dash + playout buffering

---

## 2. Content Distribution Networks (CDNs)

Scaling video streaming to millions of users is challenging. Relying on a single "mega-server" creates bottlenecks and points of failure.

- **CDN Strategy:** Store and serve multiple copies of videos at geographically distributed sites to bring content closer to users.
- **Deployment Models:**
    - **Enter Deep:** Servers are pushed deep into access networks.
    - **Bring Home:** A smaller number of larger clusters are placed in Points of Presence (POPs) near access networks.

---

## 3. P2P (Peer-to-Peer) Architectures

Unlike client-server models, P2P architectures do not rely on an "always-on" central server. Peers communicate directly with one another.

- **File Distribution Efficiency:**
    - **Client-Server:** Distribution time increases linearly with the number of peers (N) because the server must upload every copy.
    - **P2P:** Distribution time also increases with N, but more slowly because each peer contributes its own upload capacity to the network. (if i've just downloaded 10% of the file, i'll still be the server for the 10% of the file, for the other files)

- **BitTorrent:** A common P2P protocol where files are divided into small chunks (256Kb).
    - **Tracker:** A node that keeps track of all peers participating in a torrent.
    - **Tit-for-Tat:** A mechanism where peers send chunks to those who are currently providing them with chunks at the highest rate (eg: top 4 peers), encouraging fair trading, and then occasionally (30 secs) allowing a random peer to receive chunks from you (optimistically unchoking the peer...what if it reciprocates, and lets you download a lot of things from it (what if it becomes one of your top 4 peers))
    - **Requesting Chunks**: Requesting for rare chunks earlier than rest
    - **Churn:** Peers may join or leave the network at any time.

---

## 4. "Taking Turns" Protocols (Link Layer)

Link-layer protocols for media access:

- **Polling:** A master node "invites" slave nodes to transmit in turn; it suffers from polling overhead and is a single point of failure.
    
- **Token Passing:** A control token is passed sequentially from one node to the next; only the node with the token can transmit.