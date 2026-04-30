 1. Nodes: Hosts, Routers
 2. Links: Communication channels that directly connect physically adjacent nodes. Could be wire/wireless, or a LAN
 3. Frame: The Layer 2 packet (data link layer) that encapsulates the layer 3 packet (IP datagram)

Each link protocol provides different services. Like, some might not provide reliable data transfer.

# Services

## Framing / Encapsulation

It'll encapsulate the IP datagram into a frame, adding the header, & trailer. For multiple clients to access together, we'll need a Multiple Access Protocol, or Media Access Protocol aka MAC.

48 bit MAC address scheme.

There's error control in the link layer. Not properly used in low bit error links, but heavily used in wireless medium, etc. So like, there's technically a link level as well as end-end reliability.

## Flow Control

There's speed matching between the sending and receiving sides of the links. To ensure that the link layer sender doesn't overflow the link layer buffer of the receiver.

## Error Detection & Correction

There are more powerful error detection mechanisms than the checksum, and it can sometimes even correct the bit errors, without requiring a re transmission or dropping the frame.

## Half duplex and Full duplex

Full duplex = can send & recv at the same time
Half duplex = can only do 1 of them at the same time

# Implementation

Part of the link layer is in hardware (NIC, network interface card), and the other part is in software.

Sender encapsulates the datagram in the frame, adds error checking bits, reliable data transfer, flow control, etc
Receiver looks for errors, reliable data transfer, flow control, etc, extracts the datagram, and passes to the upper layer.

# Error Detection & Correction

`[D data bits][EDC]`

## Single Bit Parity

Set parity bit = 1 if there are an odd number of 1's in the data bits
If the total number of 1's in the d+1 bits is even, no error or even number of errors. Else, there's an error

## 2D Parity

Row and Column Parity.
If there's a single bit error, based on the row and column, we can not just detect, but also correct the error without a re transmission.

```
1 0 1 0 1 - 1
1 1 1 1 0 - 0
0 1 1 1 0 - 1
-----------
0 0 1 0 1 - 0
```

is correct, but if it was

```
1 0 1 0 1 - 1
1 0 1 1 0 - 0
0 1 1 1 0 - 1
-----------
0 0 1 0 1 - 0
```

there would be an error in row 2 and column 2 (one based indexing)
We would simply flip the (2,2) bit, and correct it.

## Checksum

Same as the checksum in transport layer

## Cyclic Redundancy Check (CRC)

More powerful error detection
D: data bits
G: bit pattern (generator) of r+1 bits (given, specified in CRC standard)

```
<D,R> = (D<<R + R) 
```

Sender computes r CRC bits, R, such that `<D,R>` is exactly divisible by G.

Receiver knows G, divides `<D,R>`  by G. If non zero, error detected.

Can detect errors < r+1 bits.


$D \times 2^{r}\ XOR\ R = nG$
$D \times 2^{r}= nG \ XOR\ R$

$R = remainder(\frac{D\times 2^r}{G})$

D = $101110$
G = $1001$

r = $3$

$101110000/1001 = 101011$
remainder = $101110000 - 101011*1001$ = $368-43*9$