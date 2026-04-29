It's one of the most important layers. (In every single internet device, including hosts and routers)

It has 2 parts
1. Data Plane
	- It's about local, per router functions.
	- Decides how the traffic from the input port is forwarded to the output port
2. Control Plane
	- It's a network wide logic.
	- It's the entire view of getting packets from 1 end of the world to the other end.
	- 2 Approaches
		- Traditional Routing Algorithms (in routers)
			- Each router has a forwarding table (header -> router)(Computed by having a routing algorithm run on every router, to track the time for every other router)
		- Software Defined Networking (SDN) in remote servers.
			- There's a remote controller that computes, and distributes the forwarding tables in the routers

Has a sender and receiver. Sender sends segments to the link layer. Receiver delivers the segments to the transport layer.

Routers go through all the header fields, and move datagrams from its input ports to the output ports.

**Network Layer Functions**
- Forwarding: Move packets from a routers own input to output port. Happens in nano seconds, and is implemented via hardware.
- Routing: Determines the route taken by the packet.

### Network Layer Service Model
"BEST EFFORT" service model.
No guarantee on:
- Successful datagram delivery to destination
- Timing or order of delivery.
- Bandwidth availability to end-end flow

It's very simple to implement, maintain, and manage.

Most applications run good enough most of the time, in best effort, as there's sufficient provisioning of bandwidth.

Allows distributed services to be provided from multiple locations cuz of best effort.


# What's Inside a Router

There's basically a network within a network, and like, there's a processor that controls it all (also acts as the SDN, and sets forwarding tables for all the input ports of the router)

## Input Port

Has the Line termination (physical layer), the link layer protocol, and then the networking layer (decentralised switching), that deals with the lookup, forwarding, and queuing. 

In the decentralised switching, 
1. We'll look at the header field values, and then, lookup the output port using the forwarding table
2. Destination based forwarding: based on the destination IP address
3. Generalised Forwarding: based on a set of header field values


### Destination based forwarding
1. Range based approach
2. Longest Prefix Match
   10101010 01010101 10101\*\*\* \*\*\*\*\*\*\*\* = 0
   10101010 01010101 10101010 \*\*\*\*\*\*\*\* = 1
   10101010 01010101 10100\*\*\* \*\*\*\*\*\*\*\* = 2
   else = 3
   
   Use the longest address prefix that matches the destination address.
   
   Often performed using ternary content addressable memories (TCAMs). The content gets delivered in 1 clock cycle, regardless of the size of the table

## Switching Fabric

Moves the packets from the input port to the output port. 
Switching Rate = Rate at which the packets can be transferred from the input port to the output port.

### Switching Via Memory
Packet copied to memory, and then, to the output port. So like, the ports were basically like IO devices. Depends on memory bandwidth.

### Switching Via Bus
Packet is written directly to the Output port. Depends on bus bandwidth.

### Switching Via Interconnection Networks
Made via connecting multistage switching networks in parallel, with smaller switches. $N \times N$ switches.
We can exploit parallelism further, by stacking multiple planes together. 

## Input Port Queuing
If the switching fabric is slower than the input ports combined, queuing may occur at input queues. All the packets that are waiting, wait at the input port.

This is called HOL (head of line) blocking

## Output Port Queuing

>[!important] Read this part again

Bits arrive at $N \times R$, but only leave at $R$. Thus, it requires buffering. So, we'll need a drop policy as well, if the buffer is full. There will also be a scheduling discipline, which could give priority to specific packets.

RFC 3439 says that the amount of buffering should be $RTT \times C$ (link capacity).

A more recent recommendation says, $\frac{RTT \times C}{\sqrt{ N }}$, with N flows crossing the link.

Large buffers = larger delays, and larger RTTs
