# Computer Networks — Placement Prep

Consolidated from: [[Physical Layer]], [[Link Layer]], [[Network Layer]], [[Transport Layer]], [[Application Layer]], [[Wireless]], plus standard interview-topic coverage (GfG Networking Interview Questions; "Top 100 Networking Interview Questions & Answers").

## Table of Contents
0. [Networking Fundamentals](#0-networking-fundamentals)
1. [Physical Layer & Cabling](#1-physical-layer--cabling)
2. [Link Layer](#2-link-layer)
3. [Switching & LAN Design](#3-switching--lan-design)
4. [Network Layer](#4-network-layer)
5. [Routing Protocols](#5-routing-protocols)
6. [Transport Layer](#6-transport-layer)
7. [Application Layer](#7-application-layer)
8. [Security Basics](#8-security-basics)
9. [Wireless & Mobile Networks](#9-wireless--mobile-networks)
10. [Command-Line Network Tools](#10-command-line-network-tools)
11. [Quick-Fire Interview Q&A](#11-quick-fire-interview-qa)

---

## 0. Networking Fundamentals

### Basic Terminology
- **Node**: any connection point/junction in a network (a device). A network needs at least two nodes.
- **Link**: the connectivity between two devices — the cabling/protocol enabling communication between them.
- **Networking**: interconnection of computers and peripherals for data communication, via wired cabling or wireless links.
- **Backbone network**: the centralized infrastructure that distributes routes/data across an organization's networks and manages bandwidth/channels between them.
- **Point-to-Point Link**: a direct connection between two computers — just a cable between their NICs, no intermediate device needed.
- **NOS (Network Operating System)**: specialized software whose main job is providing network connectivity, letting a computer communicate with other devices (e.g. Windows Server, Linux).
- **Sneakernet**: the earliest, informal form of "networking" — physically carrying data between machines on removable media (floppy disks, tapes, USB drives).
- **IEEE's role**: the Institute of Electrical and Electronics Engineers issues and manages the standards for networking devices, interfaces, cables, and connectors (e.g. the 802.x family — 802.3 Ethernet, 802.11 WiFi, 802.1Q VLANs).

### Network Types by Coverage
| Type | Coverage | Example |
|---|---|---|
| **PAN** (Personal) | Very small, around one person | Bluetooth headset ↔ phone |
| **LAN** (Local) | Home/office/school, small physical proximity | Office Ethernet/WiFi |
| **MAN** (Metropolitan) | City-wide, multiple LANs joined | Cable TV network |
| **WAN** (Wide) | Geographically dispersed — different regions/countries | The Internet |

### Network Architectures
| | Client-Server | Peer-to-Peer (P2P) | Hybrid |
|---|---|---|---|
| Structure | Dedicated always-on server(s) hold shared resources (files, printers); clients access them | No central server — every PC is both a provider and consumer of resources | Combines both: some centralized services, some direct peer sharing |
| Weakness | Server is a bottleneck/single point of failure | A peer sharing resources suffers **performance degradation** while others access it | More complex to manage |

### Network Topologies
| Topology | Structure | Pro | Con |
|---|---|---|---|
| Bus | Single shared backbone cable | Simple, cheap | One cable failure kills the segment |
| Star | All devices → central hub/switch | Centralized management, isolated failures, easy to set up | Central hub/switch failure takes down the **entire** network |
| Ring | Circular, data passes node to node | Fair, predictable access | One malfunctioning workstation can disable the whole ring; reconfiguration often needs a full network shutdown |
| Mesh | Every device directly connects to every other device (min. 2 connections/device) | Highly fault-tolerant — a link failure still leaves alternative paths | Expensive, complex cabling, hard to scale |
| Tree | Star networks joined via a backbone | Hierarchical, scalable | Backbone failure is critical |

### OSI Model (7 layers)
| # | Layer | Role | "Hardware" or "Software" layer |
|---|---|---|---|
| 7 | Application | Delivers services directly to end-user apps (HTTP, DNS, SMTP) | Software |
| 6 | Presentation | Translation, encryption, compression of data | Software |
| 5 | Session | Establishes/manages/terminates communication sessions | Software |
| 4 | Transport | Process-to-process delivery, reliability (TCP/UDP) | — |
| 3 | Network | Best path selection, routing packets between networks (IP) | Hardware-facing |
| 2 | Data Link | Reliable transfer between directly-connected nodes, frames, MAC addressing | Hardware-facing |
| 1 | Physical | Raw bits over cable/wireless signals; converts bits ↔ electrical signals, defines cable/device specs | Hardware-facing |

TCP/IP's stack (used in this doc) collapses layers 5–7 into one **Application** layer and layers 1–2 into one **Link** layer: **Link → Internet (Network) → Transport → Application** (4 layers vs OSI's 7). TCP/IP's single Application layer is the practical equivalent of OSI's Session + Presentation + Application layers combined.

### Device ↔ OSI Layer Mapping
| Device | Layer |
|---|---|
| Hub, Repeater | Physical (L1) |
| Switch, Bridge, NIC (MAC address) | Data Link (L2) |
| Router | Network (L3) |
| Gateway, (Layer-7) Firewall | Transport–Application (L4–L7) |

- **Gateway**: a device (or computer running gateway software) that connects two dissimilar network segments and translates between their protocols so systems on each side can communicate — a broader concept than a router, which just forwards IP packets.
- **NIC (Network Interface Card)**: the peripheral card attached to a device to connect it to a network; each card has a unique **MAC address** hard-wired into its circuitry (not tied to the PC) — swapping the NIC changes the machine's MAC address.

### Encapsulation / Decapsulation
- **Encapsulation** (sender, moving down the stack): each layer wraps the data from the layer above with its own header (and sometimes trailer) — e.g. `[Eth [IP [TCP [HTTP data]]]]`. Also involves breaking a large message into manageable chunks and attaching source/destination addressing plus error-checking (parity/checksum) info.
- **Decapsulation** (receiver, moving up the stack): each layer strips its own header before passing the payload up.
- This is exactly the framing/segment/datagram terminology used throughout: **segment** (transport) → **datagram/packet** (network) → **frame** (link) → **bits** (physical).

### Reliability Concepts
- **Fault tolerance**: a system design that keeps data continuously available by eliminating single points of failure (e.g. RAID, redundant links/servers). It protects against hardware/link failure — **not** against accidental deletion or corruption of the data itself (that needs backups).
- **Clustering**: a network OS's ability to group multiple servers into a fault-tolerant cluster; if one server fails, another in the cluster picks up its processing so service continues uninterrupted.

---

## 1. Physical Layer & Cabling

Concerned only with moving raw bits over a medium — no concept of frames, IPs, or packets.

### Key Formulas
- **Bit Rate** = Baud Rate × Bits per Signal Unit
- **Attenuation** (dB) = $10\log_{10}(P_2/P_1)$
- **SNR** = $P_{signal}/P_{noise}$; $SNR_{dB} = 10\log_{10}(SNR)$
- **Nyquist Theorem** (noiseless max): Bit Rate = $2 \times B \times \log_2(Levels)$ bps
- **Shannon Capacity** (max with noise): $C = B\log_2(1+SNR)$ bps
- **Bandwidth-Delay Product (BDP)** = bandwidth × RTT — the max amount of unacknowledged data that can be "in flight" on the link at once; sizes the ideal pipelined/sliding-window width.

### Delay Components
| Delay | Meaning |
|---|---|
| Processing ($D_{proc}$) | Router/switch time to process packet header |
| Queuing ($D_{queue}$) | Time waiting in buffer — grows with congestion |
| Transmission ($D_{trans}$) | Time to push all bits onto the wire = Length/Rate — depends on packet size/bandwidth |
| Propagation ($D_{prop}$) | Time for signal to travel the wire — depends on physical distance |

- **RTT (Round-Trip Time)**: total time for a packet to travel sender→receiver→sender; measured via `ping`. Lower RTT = lower latency.
- **Jitter**: variation in packet delay across a flow (not the delay itself). Hurts real-time apps (VoIP, video calls) — arriving packets at uneven intervals causes stutter even if average delay is fine. Mitigated with playout buffers.
- **Signal attenuation**: weakening of a signal over distance/cable length. Fixed with **repeaters/hubs**, which regenerate (not just amplify) the signal, plus ensuring proper cable termination.

### Line Coding (Digital → Digital)
- **Unipolar (NRZ)**: one polarity; poor sync on long runs of same bit.
- **Polar**: NRZ-L (level = bit), NRZ-I (transition = 1), RZ (returns to zero mid-bit, self-syncing), **Manchester** (transition mid-bit always — **self-clocking**: receiver recovers timing directly from the transition, better sync/error detection than NRZ but needs more bandwidth; classic 10BASE-T Ethernet used it), Differential Manchester.
- **Bipolar**: AMI (0=zero volt, 1=alternating +/-, avoids baseline wander), Pseudoternary (inverse).
- **Multilevel**: 2B1Q (2 bits → 1 of 4 levels), MLT-3.
- **Block coding (4B/5B)**: maps 4-bit groups to 5-bit codes; extra unused patterns guarantee enough transitions for clock sync and can flag invalid/control codes.

### Analog↔Digital
- **PCM**: Sample (Nyquist: $f_s \ge 2f$) → Quantize → Encode.
- **Delta Modulation**: encodes only the *change* from the previous sample.

### Digital→Analog Modulation
ASK (amplitude), FSK (frequency), PSK (phase), **QAM** (amplitude + phase combined — most bits/symbol).

### Multiplexing
| Type | Idea |
|---|---|
| FDM | Different non-overlapping frequency bands per signal, with guard bands |
| WDM | FDM for fiber optics — different light wavelengths |
| TDM | Different, interleaved time slots; pulse stuffing pads slower sources to match rate |

**Spread Spectrum** (anti-jamming/eavesdropping, not efficiency): **FHSS** (hop frequencies pseudorandomly) vs **DSSS** (multiply data by a wideband pseudorandom code).

### Transmission Media & Cabling Standards
- Guided: twisted pair, coaxial, fiber (step-index MMF short-range, graded-index MMF LAN-range, single-mode = highest rate/longest distance).
- Unguided: radio (ground/sky/line-of-sight), microwave, infrared.
- **Fiber optic advantages over copper**: much higher bandwidth, faster transmission, minimal signal loss/degradation over long distances, **immune to electromagnetic interference (EMI)**, and harder to tap (more secure).

**Twisted pair cabling specifics**:
- **UTP (Unshielded Twisted Pair)**: standard, cheaper; a single segment supports **~90–100 m** before signal degradation — repeaters/switches extend a network past that limit. Proper termination requires **100 ohm** impedance matching.
- **STP (Shielded Twisted Pair)**: adds shielding specifically to prevent **crosstalk** — electromagnetic interference/noise bleeding between adjacent wire pairs and corrupting transmitted data.
- **Naming convention** (e.g. `10Base-T`, `100Base-FX`): `<speed>Base<medium>` — the number is the transfer rate in Mbps, "Base" means **baseband** (whole bandwidth dedicated to one signal, vs "broadband"), and the letter indicates the medium (`T` = twisted pair, `FX` = fiber). `100Base-FX` supports up to ~412 m per segment (up to ~5 km for full-duplex/long-haul variants).
- **Straight-through vs Crossover cables**:
  - **Straight-through**: wiring is identical pin-to-pin at both ends; used to connect **dissimilar** devices — e.g. PC ↔ switch/hub/router.
  - **Crossover**: swaps the transmit and receive pin pairs; used to connect **similar** devices directly — e.g. PC ↔ PC, switch ↔ switch, hub ↔ hub (this is how two computers can share files directly with no hub/router at all).
  - Standard **T568B** straight-through color order: orange/white, orange, green/white, blue, blue/white, green, brown/white, brown.

### Transmission Modes: Sync vs Async
- **Synchronous**: continuous stream of bits sharing a common clock between sender/receiver; efficient for large transfers (e.g. Ethernet).
- **Asynchronous**: data sent character-by-character with **start/stop bits** framing each unit; simpler hardware, more per-unit overhead (e.g. old serial/keyboard links).
- **Isochronous**: guarantees fixed, steady timing (no retries) — used where jitter is unacceptable.

### Switching
- **Circuit switching**: dedicated path reserved end-to-end before transmission — guaranteed/fixed bandwidth, predictable, but wastes capacity when idle.
- **Packet switching**: data split into packets sharing paths dynamically, packets may take different routes — efficient, scalable, but variable delay (jitter) and possible loss/reordering. The Internet is packet-switched.

### MTU & Path MTU Discovery
- **MTU (Maximum Transmission Unit)**: largest frame/packet size a link can carry without fragmentation (Ethernet default 1500 bytes).
- **PMTUD (Path MTU Discovery)**: sender probes the path (packets with the "don't fragment" bit) to find the smallest MTU among all hops, then sizes packets to avoid fragmentation end-to-end — reduces overhead/fragmentation-related drops.

---

## 2. Link Layer

Moves frames between **physically adjacent** nodes (hosts/routers connected by a link). Frame = Layer-2 encapsulation of the IP datagram.

### Services
- **Framing/Encapsulation**: wraps datagram with header+trailer; multiple clients on shared medium need a **Multiple Access (MAC)** protocol. MAC addresses are 48-bit, physical, and (usually) burned into the NIC.
- **Flow Control**: speed-matches sender/receiver to avoid overflowing the receiver's link-layer buffer.
- **Error Detection & Correction**: more powerful than transport-layer checksum; can sometimes correct without retransmission.
- **Duplex**: full (send+recv simultaneously) vs half (one at a time).

Implemented partly in hardware (NIC) and partly in software.

### Error Detection & Correction
- **Single-bit parity**: parity bit makes total 1s even; detects single-bit (or odd-count) errors only.
- **2D parity**: row+column parity — can detect **and correct** a single-bit error by finding the intersecting row/column.
- **Checksum**: same idea as transport layer (weak — some compensating bit-flip patterns go undetected).
- **CRC (Cyclic Redundancy Check)**: strongest of these. Sender picks R bits such that $\langle D,R\rangle$ is exactly divisible by generator G (r+1 bits). Receiver divides by G; nonzero remainder ⇒ error. Detects all burst errors < r+1 bits.
  - $R = remainder(D \times 2^r / G)$, computed via binary (XOR) division.

### ARP (Address Resolution Protocol)
Maps an **IP address → MAC address** within a local (same-subnet) network — needed because the link layer only understands MAC addresses, not IPs.
1. Host broadcasts an ARP request: *"Who has IP 192.168.1.5? Tell 192.168.1.10."*
2. The host owning that IP replies directly with its MAC address.
3. Result is cached in the sender's **ARP cache/table** to avoid repeating the broadcast for every packet.

**MAC vs IP address**:
| | MAC Address | IP Address |
|---|---|---|
| Layer | Data Link (L2) | Network (L3) |
| Type | Physical identifier | Logical identifier |
| Scope | Identifies device on the local link | Identifies location/host in the wider network |
| Mutability | Usually fixed (burned into NIC) | Changeable (DHCP, network moves) |

**IPv6 replaces ARP with NDP** (Neighbor Discovery Protocol) — see [Network Layer](#4-network-layer).

### CSMA/CD vs CSMA/CA
Both are **Multiple Access** protocols for a shared medium, deciding when a node may transmit.

| | CSMA/CD (Collision **Detect**) | CSMA/CA (Collision **Avoidance**) |
|---|---|---|
| Used on | Wired (classic/legacy Ethernet) | Wireless (WiFi/802.11) |
| Approach | Transmit, **monitor the wire while sending**; if a collision is detected, stop and retransmit after a random backoff | **Announce transmission intent beforehand** (sense channel idle, wait DIFS, possibly RTS/CTS) to avoid causing a collision in the first place |
| Why the difference | On a wire, a sender can listen while transmitting and detect a collision immediately | Wirelessly, a transmitting radio can't hear a weak incoming collision over its own strong outgoing signal, and the **hidden terminal problem** means it may never even hear the other transmitter — so collisions must be *avoided*, not detected |

(See [WiFi (802.11)](#9-wireless--mobile-networks) for the full CSMA/CA mechanics — DIFS/SIFS, backoff, RTS/CTS.)

---

## 3. Switching & LAN Design

### How a Switch Learns MAC Addresses
1. On each incoming frame, the switch reads the **source MAC** and records `(MAC → incoming port)` in its MAC address table.
2. If the **destination MAC is unknown** (not yet in the table), the switch **floods** the frame out every port except the one it arrived on.
3. Once the destination replies, its source MAC gets learned too — future frames to it are forwarded directly instead of flooded.

### Store-and-Forward vs Cut-Through Switching
| | Store-and-Forward | Cut-Through |
|---|---|---|
| Mechanism | Buffers the **entire frame**, checks CRC, then forwards | Starts forwarding as soon as the **destination MAC** (early in the frame) is read |
| Latency | Higher | Lower |
| Reliability | Won't forward a corrupted frame | May forward a corrupted frame (no time to check CRC) |
| Typical use | Enterprise / reliability-focused networks | Low-latency environments (e.g. HFT, some data centers) |

### Hub vs Switch vs Router
| | Hub | Switch | Router |
|---|---|---|---|
| Layer | Physical (L1) | Data Link (L2) | Network (L3) |
| Forwarding | A multiport **repeater** — regenerates and broadcasts to **all** ports, no traffic intelligence | Forwards to the specific port via MAC address table — much better performance under high traffic since it doesn't blast every frame everywhere | Forwards between networks via IP/routing table |
| Collision domain | One shared domain for all ports | One collision domain **per port** | Separates broadcast domains too |

**Layer-3 Switch vs Router**:
- **L3 switch**: routes in hardware (ASICs), optimized for high-speed LAN/inter-VLAN routing; used inside campus networks/data centers.
- **Router**: supports richer WAN/Internet-edge features (BGP, MPLS, VPN termination, NAT); used at network boundaries.

### Collision Domain vs Broadcast Domain
- **Collision domain**: set of devices where simultaneous transmissions can collide (relevant mainly to shared/half-duplex media and hubs).
- **Broadcast domain**: set of devices that receive the same broadcast frame.
- A switch creates a **separate collision domain per port**; only a **router** (or VLAN boundary) separates **broadcast domains**. Without VLANs, an entire switched LAN is a single broadcast domain.
- Example: a 24-port switch (no VLANs) = 24 collision domains, but still just **1** broadcast domain.

### Spanning Tree Protocol (STP)
- Layer-2 protocol that prevents **switching loops** in networks with redundant links (loops would cause broadcast storms).
- Elects a **Root Bridge** as the reference point, then computes a loop-free tree of shortest paths from it, blocking redundant links.
- Automatically activates a blocked backup path if the primary fails.

### VLANs (IEEE 802.1Q)
- Adds a **VLAN tag** to Ethernet frames to logically separate traffic on a shared physical switch fabric — this is what creates **separate broadcast domains at the switch level**, since a plain unmanaged switch is one single broadcast domain.
- Multiple VLANs can share one physical **trunk link** (tagged frames carry their VLAN ID across it).
- Benefits: traffic isolation, security, easier management — each VLAN behaves as its own broadcast domain.

### Power over Ethernet (PoE)
- A single Ethernet cable carries both data **and** electrical power.
- Eliminates the need for separate power cabling for devices like IP phones, WiFi APs, and IP cameras — reduces installation cost/complexity.

---

## 4. Network Layer

Present in **every** internet device (hosts + routers). Two planes:

| Plane | Scope | Approaches |
|---|---|---|
| **Data Plane** | Local, per-router — how input port traffic reaches an output port | Forwarding tables |
| **Control Plane** | Network-wide — how packets get end-to-end | Traditional distributed routing algorithms (per-router) vs **SDN** (remote controller computes/pushes forwarding tables) |

- **Forwarding**: move packet from a router's input to output port (hardware, nanoseconds).
- **Routing**: determine the path/route a packet takes (see [Routing Protocols](#5-routing-protocols)).
- Service model: **"best effort"** — no guarantees on delivery, order, timing, or bandwidth. Simple to implement/manage; sufficient given adequate provisioning.

### Inside a Router
**Input port**: physical layer → link layer → network layer (lookup/forward/queue).
- Destination-based forwarding: match on destination IP.
  - **Longest Prefix Match**: pick the forwarding-table entry with the longest matching prefix. Often implemented with **TCAM** (constant-time lookup regardless of table size).
- Generalized forwarding: match on arbitrary header fields (OpenFlow), actions = forward/drop/modify/send-to-controller.

**Switching fabric** (input port → output port):
| Type | Bottleneck |
|---|---|
| Via memory | Memory bandwidth |
| Via bus | Bus bandwidth |
| Via interconnection network | $N\times N$ crossbar-style, parallelizable across planes |

**Queuing**:
- **Input queuing** → can cause **HOL (Head-of-Line) blocking** if fabric is slower than combined input rate.
- **Output queuing** → bits arrive at up to $N\times R$ but leave at $R$, so buffering is required. Buffer sizing guideline: $RTT \times C$ (or $\frac{RTT\times C}{\sqrt N}$ for N flows) — RFC 3439. Large buffers ⇒ larger delay/RTT ("bufferbloat").
- **Buffer management**: drop policy (tail drop vs priority drop) + marking (ECN/RED).
- **Scheduling**: FCFS, Priority, Round Robin, Weighted Fair Queuing.

### IP Datagram (IPv4) Header — key fields
Version, header length, type of service (diffserv+ECN), total length, 16-bit ID + flags + fragment offset (for fragmentation), **TTL** (decremented per hop; drop at 0 — prevents infinite loops), upper-layer protocol (6=TCP, 17=UDP), header checksum, source/dest IP, options.
Typical overhead: 20B IP + 20B TCP = 40B + app-layer overhead.

### IP Addressing
- 32-bit, `a.b.c.d` — displayed in **dotted-decimal notation** (four decimal numbers 0-255 separated by dots, e.g. `192.168.101.2`). Split into subnet (high bits) + host (low bits) portions.
- **Subnet mask**: a 32-bit value paired with an IP address that marks which bits are the network portion vs the host portion — used to determine whether two addresses are on the same subnet.

**Classful addressing (legacy, still asked in interviews)**: the first bits of the first octet determine the class:
| Class | First bits | First octet range | Default mask | # Networks | Hosts/network |
|---|---|---|---|---|---|
| A | `0` | 0–127 | 255.0.0.0 | 126 | 16,777,214 |
| B | `10` | 128–191 | 255.255.0.0 | 16,384 | 65,534 |
| C | `110` | 192–223 | 255.255.255.0 | 2,097,152 (2^21) | 254 (2^8−2) |
| D | `1110` | 224–239 | — (multicast) | — | — |
| E | `1111` | 240–255 | — (reserved/experimental) | — | — |

Modern networks use **CIDR** (classless) instead, which is far more flexible — see below.

- **CIDR**: `a.b.c.d/x` — x bits are the network prefix. Also used for **route aggregation/summarization**: multiple contiguous network prefixes are combined into one shorter-prefix routing-table entry, shrinking table size.
- **VLSM (Variable Length Subnet Masking)**: subdivides a network into subnets of **different sizes** (unlike classful/fixed subnetting) for efficient address utilization and flexible internal design. CIDR is generally used for external/Internet route summarization; VLSM for internal subnet design — both rely on the same variable-prefix-length idea.
- **DHCP** (over UDP; client port 68, server port 67) — the **DORA** process: **D**iscover (client broadcasts a request) → **O**ffer (server offers an available IP) → **R**equest (client requests that offered IP) → **A**cknowledge (server confirms allocation, sends lease + config). Also returns first-hop router (default gateway), DNS server, subnet mask.
  - Administrators create a **scope** — a pool of IP addresses DHCP can dynamically hand out to clients on a subnet.
- **APIPA (Automatic Private IP Addressing)**: if DHCP is unreachable, Windows hosts self-assign an address in `169.254.0.0/16` so local (LAN-only) communication still works — no Internet access. Seeing a `169.254.x.x` address is a classic sign of a DHCP failure.
- **Default Gateway**: the router a host sends traffic to when the destination is **outside** its local subnet (typically the router's LAN-facing port address). Traffic to hosts on the same subnet is sent directly (via ARP-resolved MAC); everything else routes via the gateway. E.g., host `192.168.1.10` with gateway `192.168.1.1`.
- **Address allocation**: ICANN owns IPv4 space (IANA/ICANN for IPv6 too) → 5 Regional Registries → ISPs. Longer/more specific prefix always wins during routing regardless of which ISP block it nominally belongs to.

### NAT (Network Address Translation)
- Converts private IPs to a shared public IP. Router rewrites `(private IP, private port) ↔ (public IP, public port)` and keeps a translation table so return traffic reaches the right internal host/socket — the **port** is what lets one public IP serve many internal hosts.
- Private ranges (**not globally routable** — reusable across any number of independent intranets without conflict, and Internet routers drop packets destined to them): `10.0.0.0/8`, `172.16.0.0/12`, `192.168.0.0/16`.
- **Benefits**: conserves scarce IPv4 address space; adds a layer of obscurity/security — Internet hosts only ever see the router's single translated public IP, never the internal private addresses.
- **Limitations**: breaks true end-to-end connectivity, complicating P2P protocols, VoIP, and some online gaming (needs NAT traversal techniques like STUN/TURN/hole-punching).
- **IPv6 removes the need for NAT**: 128-bit address space is large enough to give every device its own public/globally-unique address, restoring true end-to-end connectivity and simplifying routing/performance.

### IPv6
- 128-bit addresses, 40-byte **fixed-length** header for fast processing. Designed to replace IPv4 and overcome its (near-exhausted) address-space limits.
- Removed vs IPv4: checksum (recomputing it at every hop was slow; other layers already error-check), TTL→hop limit stays but simpler, fragmentation/reassembly (now endpoints' job only), options (moved to be a payload-layer extension).
- Added: flow label (identify datagrams in the same flow, semantics left to ISP).
- **NDP (Neighbor Discovery Protocol)**: IPv6's replacement for ARP, built on **ICMPv6** messages. Also handles router discovery, automatic address configuration (SLAAC), and duplicate address detection — folding several IPv4 mechanisms (ARP + ICMP Router Discovery + more) into one framework.
- **Tunnelling**: to cross IPv4-only routers, an IPv6 packet is encapsulated inside an IPv4 packet: `[IPv4 [IPv6]]`.

### Generalized Forwarding / OpenFlow
In generalized forwarding, matches can use up to 12 header fields (Network, Transport, Link layer + ingress port); actions = forward to port / drop / modify header / send to controller. The forwarding table is often called the **flow table**.

### Middleboxes / NFV
Middlebox = anything on the path doing more than plain IP forwarding (firewalls, NAT boxes, load balancers). **NFV (Network Functions Virtualization)**: run these as software on general-purpose machines instead of dedicated hardware.

### Architectural Principles
Core internet beliefs: simple connectivity, single IP protocol, **intelligence at the edge** (End-to-End Argument) — routers only see packets, not application semantics, so functions like reliability/congestion control can only be *completely and correctly* implemented at the endpoints, not hop-by-hop in the network core.

---

## 5. Routing Protocols

### Distance Vector vs Link-State
| | Distance Vector | Link-State |
|---|---|---|
| Info shared | Only with direct neighbors ("what I can reach and at what cost") | Full network topology (link states) flooded to everyone |
| Each router knows | Just distances, not the full topology | The **entire** network map |
| Convergence | Slower | Faster |
| Algorithm | Bellman-Ford style | **Dijkstra's Shortest Path First (SPF)** |
| Example protocol | **RIP** | **OSPF** |
| Best for | Small/simple networks | Large networks (faster, more efficient convergence) |

### RIP (Routing Information Protocol)
- Lets routers exchange reachability info by **broadcasting their entire routing table** to neighboring routers.
- Determines the best path using **hop count** as the distance metric (max 15 hops; 16 = unreachable).

**Count-to-Infinity & Fixes**:
- **Problem**: after a link fails, routers can keep incrementing each other's advertised hop count for the now-unreachable destination in a loop, converging very slowly ("counting to infinity" toward RIP's max of 16 = unreachable).
- **Split Horizon**: don't advertise a route back out the same interface it was learned from (prevents an immediate loop with that neighbor).
- **Route Poisoning**: explicitly advertise a failed route with an infinite metric, so neighbors learn immediately it's dead instead of waiting for a timeout.

### OSPF (Open Shortest Path First)
- Link-state protocol; each router builds a full map of the network and propagates only **changed** information (not full periodic re-broadcasts like RIP).
- Runs **Dijkstra's SPF algorithm** on that topology to determine the optimal path, giving faster and more efficient convergence than RIP.

### Route Flapping
- A route repeatedly toggling between up/down (available/unavailable) states.
- Causes excessive routing updates, high CPU usage, and constant reconvergence.
- Mitigations: BGP **route dampening** (temporarily ignore/penalize a flapping route), **hold-down timers** (don't accept new info about a route for a period after it changes), and link-state protocols like OSPF limiting recalculation frequency.

### BGP (Border Gateway Protocol)
- Exchanges routing information **between different Autonomous Systems (AS)** — the protocol that glues the Internet together.
- A **Path-Vector** protocol: each advertised route carries the full **AS path** it traversed, which is used to detect and prevent routing loops (a router rejects a route whose AS-path already contains its own AS).
- Supports rich routing **policies** (prefer/avoid certain paths), not just shortest-path metrics.

### IGP vs EGP
| | IGP (Interior Gateway Protocol) | EGP (Exterior Gateway Protocol) |
|---|---|---|
| Scope | Within a single AS | Between different ASes |
| Examples | RIP, OSPF, EIGRP, IS-IS | BGP |
| Priority | Fast convergence | Policy control |

### Route Redistribution
- Sharing routes learned via one routing protocol into another routing protocol running on the same network (e.g., redistributing OSPF routes into EIGRP, or RIP into OSPF).
- Needed when a network runs multiple protocols simultaneously (common during migrations or multi-vendor environments).
- Must be configured carefully — mismatched metrics/loop-prevention semantics between protocols can create routing loops.

### CIDR (Aggregation) + ECMP
- **CIDR route aggregation**: combine multiple contiguous prefixes into one shorter/summarized route, shrinking routing table size (see also [Network Layer](#4-network-layer)).
- **ECMP (Equal-Cost Multi-Path)**: when multiple paths to a destination have equal cost, distribute traffic across all of them instead of using just one — improves bandwidth utilization and adds redundancy.
- Together they improve scalability, efficient bandwidth use, and reliability.

### Multicast Routing
Delivers targeted broadcasts — sending to a **selected group** of interested subscribers rather than every host on the subnet (unlike a true broadcast). See also [IP Multicast](#7-application-layer) below.

---

## 6. Transport Layer

Provides **process-to-process** logical communication (vs network layer's host-to-host). Sits between application and network layers.

| | Transport Layer | Network Layer |
|---|---|---|
| Communication | Process-to-process | Host-to-host |
| Unit | Segment | Packet/Datagram |
| Protocols | TCP, UDP | IP |
| Reliability | May provide it | Best-effort only |

### Sockets
A **socket** is one endpoint of a two-way communication link between two programs on a network — the interface between the application layer and the transport layer. It is bound to a **port number**, which is what lets the transport layer identify *which application* on a host a given segment's data belongs to.
```
Application Layer
     |
   Socket
     |
Transport Layer
```

### Multiplexing/Demultiplexing
- **Multiplexing** (sender): gather data from multiple sockets, add headers, hand to network layer.
- **Demultiplexing** (receiver): use header info (IP + port) to route segment to correct socket.
- **UDP (connectionless) demux**: uses **destination port only** — same dest port + different source IPs → same socket.
- **TCP (connection-oriented) demux**: uses the full **4-tuple** (src IP, src port, dst IP, dst port) → each client connection gets its own socket even on the same server port.

### Ports
- Identify a specific application/service on a device, letting many applications share one network stack simultaneously.
- Well-known: HTTP 80, HTTPS 443, FTP 21, DNS 53.
- **Ephemeral ports**: temporarily assigned by the OS (from a high-numbered range) for the *client* side of an outgoing connection; released once the connection ends.

### UDP
Connectionless, unreliable, no ordering/flow/congestion control — "best effort," low overhead. Used where speed matters more than reliability: DNS, SNMP, streaming, gaming, HTTP/3.
- **Segment**: Source Port | Dest Port | Length | Checksum | Data.
- **Checksum**: 16-bit one's-complement sum; weak error detection (some errors go undetected).
- **UDP vs TCP for streaming/gaming**: UDP preferred despite unreliability — no connection setup delay, no retransmission stalls; low latency/overhead matters more than perfect delivery, and small losses are tolerable (a dropped video frame vs. stalling to retransmit it).

### Reliable Data Transfer (RDT) — Protocol Evolution
| Version | Handles | Mechanism |
|---|---|---|
| rdt1.0 | Perfect channel | No ACKs needed |
| rdt2.0 | Bit errors | Checksum + ACK/NAK |
| rdt2.1 | Corrupted ACK/NAK | Add sequence numbers (0/1), detect duplicates |
| rdt2.2 | Same, NAK-free | Only ACKs; duplicate ACK acts as implicit NAK |
| rdt3.0 | Packet loss | Add timeout-based retransmission |

**Stop-and-wait utilization** = $D_{trans}/(RTT+D_{trans})$ — typically tiny (e.g. 0.00027 for a 1Gbps link with 15ms one-way prop delay), hence the need for pipelining.

### ARQ Protocols Compared
| Protocol | Idea | Efficiency | Weakness |
|---|---|---|---|
| **Stop-and-Wait** | Send one frame, wait for its ACK before sending the next | Very low (idles the link for a full RTT per frame) | Simple but wastes bandwidth |
| **Go-Back-N** | Sender window of N unACKed frames; receiver only accepts **in-order** frames | Better — pipelines transmission | On loss, retransmits **everything** from the lost frame onward, even correctly-received later frames |
| **Selective Repeat** | Receiver **buffers** out-of-order frames, ACKs each individually | Best — sender retransmits **only** the lost frame | More complex, needs more buffering + per-frame timers |

- **Selective Repeat window-size rule**: window size ≤ (sequence number space)/2 — otherwise an old, delayed/duplicate packet's sequence number can be mistaken for a new one, since the window could wrap around and overlap with stale numbers still in flight.

### TCP
Reliable, connection-oriented, ordered, full-duplex, with flow control + congestion control.

**Segment header**: Src/Dst Port | Sequence Number (first byte of data in the segment) | ACK Number (next byte expected) | Header Length/Flags/Receive Window | Checksum | Options | Data.
**Flags**: SYN (setup), ACK, FIN (teardown), RST (reset), PSH (push), URG (urgent, rarely used).

- **Sequence Number**: identifies the byte-stream position of the first byte in this segment.
- **Acknowledgment Number**: the next byte the receiver expects — implicitly a **cumulative ACK** confirming everything before it.
- **Duplicate prevention**: TCP discards segments whose sequence number range is already covered by data it has received, protecting against duplicate delivery.
- **Cumulative ACK example**: receiver has bytes 1,2 → sends `ACK=3` ("expecting byte 3 next").
- **Duplicate ACKs**: a gap in the stream (e.g. byte 2 lost from a 1-2-3-4 send) makes the receiver keep re-sending `ACK=2` for every subsequent correctly-received segment, since it still hasn't gotten byte 2.
- **Fast Retransmit**: on **3 duplicate ACKs**, retransmit immediately without waiting for the RTO timer.
- **RTO / RTT estimation**:
  - $EstimatedRTT = (1-\alpha)EstimatedRTT + \alpha \cdot SampleRTT$ (α≈0.125)
  - $DevRTT = (1-\beta)DevRTT + \beta|SampleRTT-EstimatedRTT|$ (β≈0.25)
  - $Timeout = EstimatedRTT + 4\times DevRTT$

**TCP Sliding Window**: the general flow-control mechanism allowing the sender to have **multiple unacknowledged packets in flight** at once instead of stopping-and-waiting; the window "slides" forward as ACKs arrive, improving throughput/bandwidth utilization. Go-Back-N and Selective Repeat are specific sliding-window implementations.

**TCP Connection Establishment & TIME_WAIT**:
- **3-way handshake**: SYN(seq=x) → SYN+ACK(seq=y, ack=x+1) → ACK(ack=y+1).
- Teardown uses a **4-way FIN handshake**: Client FIN → Server ACK → Server FIN → Client ACK.
- After sending the final ACK, the client enters **TIME_WAIT**: it waits (typically ~2×Maximum Segment Lifetime) before fully closing, in case that final ACK was lost.
- If the server never receives the final ACK, it **retransmits its FIN**; because the client is still in TIME_WAIT (not yet gone), it can respond with the ACK again — this state is what makes connection teardown reliable despite loss on the underlying network.

**Nagle's Algorithm**:
- Buffers small outgoing TCP segments and coalesces them into fewer, larger packets before sending, rather than sending each tiny write immediately.
- **Pro**: fewer packets, less header overhead, better bandwidth efficiency for bulk/non-interactive transfers.
- **Con**: adds delay waiting to batch data with pending ACKs — bad for latency-sensitive, small-message traffic (gaming, VoIP, interactive terminals). Commonly disabled via `TCP_NODELAY` for such applications.

### TCP Flow Control vs Congestion Control
| | Flow Control | Congestion Control |
|---|---|---|
| Protects | The **receiver's** buffer | The **network** from overload |
| Scope | Single sender ↔ single receiver | Aggregate of many senders sharing the network |
| Mechanism | Receiver advertises **rwnd** (receive window) | Sender maintains **cwnd** (congestion window) |
| Sending rule | Sender limited to `bytes in flight ≤ rwnd` | Sender limited by cwnd, adjusted via AIMD/slow-start/etc. |
| **Actual send window** | `min(cwnd, rwnd)` — whichever is smaller wins |||

### TCP Congestion Control
Purpose: prevent **aggregate** overload from many senders (vs flow control's single sender/receiver focus). Sending window = $\min(cwnd, rwnd)$.

- **AIMD**: additive increase (+1 MSS/RTT) until loss, multiplicative decrease (halve cwnd) on loss (detected via triple-dup-ACK). On **timeout**, cwnd drops all the way to 1 MSS (more severe than a triple-dup-ACK event).
- **Slow Start**: cwnd starts at 1 MSS, **doubles every RTT** until it hits ssthresh (half of the cwnd at last loss).
- **Congestion Avoidance**: past threshold, increase **linearly** (+1 MSS/RTT).
- **Fast Retransmit**: triggered by 3 duplicate ACKs — retransmit immediately, don't wait for the timer.
- **Fast Recovery**: after fast retransmit, cwnd resets to ssthresh (not all the way to 1) — continues transmission without restarting from Slow Start.

**Algorithm generations**:
| Variant | Behavior |
|---|---|
| **Tahoe** | Slow Start + Fast Retransmit; on **any** loss (timeout or 3-dup-ACK), always drops all the way back to Slow Start |
| **Reno / New Reno** | Adds **Fast Recovery** — after a 3-dup-ACK loss, drops to ssthresh and continues in Congestion Avoidance instead of restarting Slow Start; recovers faster |
| **CUBIC** | Window is a **cubic function** of time since the last loss relative to $W_{max}$: larger increases far from the inflection point K, smaller increases near it (approaching the last known ceiling cautiously). Default on Linux; better suited to high-speed, long-distance (high BDP) networks than Reno's linear growth |

- **Delay-based control**: compares measured throughput to uncongested throughput ($cwnd/RTT_{min}$); increase if close, decrease if far below — keeps the pipe full without deliberately inducing loss.
- **ECN (Explicit Congestion Notification)**: routers **mark** packets (ECN bits) instead of dropping them under congestion; sender marks ECN=10, a congested router sets ECN=11, and the receiver echoes this back to the sender via the **ECE bit** in its ACK. Advantages: sender can react and slow down **before** an actual drop happens, improving throughput, cutting retransmissions, and lowering latency compared to loss-triggered reactions alone.
- **TCP Fairness**: K sessions sharing bottleneck bandwidth R converge toward R/K each on average (AIMD's "sawtooth" naturally converges to fairness); UDP ignores this entirely, and apps opening multiple TCP connections effectively grab more than their fair share.
- **Distinguishing congestion loss from wireless loss**: classic TCP assumes *all* loss means congestion, which is wrong on wireless links where loss often comes from signal interference/bit errors, not congestion. Modern approaches use ECN (explicit signal, so ambiguity is removed), SACK (Selective ACK — reports exactly which segments arrived, not just a cumulative ACK, avoiding unnecessary retransmission of correctly-received data), and link-layer retransmission/error recovery to avoid needlessly shrinking cwnd for non-congestion loss.

### QUIC (Transport evolution)
Runs over **UDP**, but reimplements reliability, congestion control, and **encryption (TLS 1.3)** at the transport layer, with per-stream loss recovery (see HTTP/3 below).

---

## 7. Application Layer

### Client-Server vs P2P
- **Client-Server**: always-on server with permanent IP (HTTP, IMAP, FTP).
- **P2P**: no dedicated server; peers intermittently connected with changing IPs (BitTorrent).
- **File distribution time**: grows linearly with N peers for client-server (server must upload every copy); grows more slowly for P2P since each peer also uploads.

### TCP vs UDP (Application-facing)
| | TCP | UDP |
|---|---|---|
| Connection | Connection-oriented (3-way handshake) | Connectionless |
| Reliability | Reliable — error checking, ACKs, retransmission | Unreliable — no delivery guarantee |
| Ordering | Guaranteed | Not guaranteed |
| Speed | Slower (overhead of reliability) | Faster, lower overhead |
| Typical uses | Web (HTTP), email (SMTP), file transfer (FTP) | Streaming, gaming, DNS |

TLS adds encryption + integrity + endpoint auth on top of vanilla (cleartext) TCP.

### FTP (File Transfer Protocol)
- Transfers files between different systems (e.g. UNIX ↔ Windows) — platform-independent, runs over TCP, uses separate control (port 21) and data connections.
- **Anonymous FTP**: a way of granting access to files on a public server without requiring individual user identification — users log in with a generic "anonymous" username (commonly using their email as a password by convention) to download publicly shared files.

### HTTP
- Stateless — no memory of past requests (simple/robust, but needs **cookies** for state: server sets `Set-Cookie` → browser stores → sends `Cookie` header on future requests → server looks up in a backend DB).
- Persistent (many objects/connection) vs non-persistent (one object/connection) connections.
- Request format: request line (method, URL, version) → headers → CRLF CRLF → entity body.
- Methods: GET, POST, HEAD (headers only, no body), PUT (upload/replace).
- **Conditional GET / caching**: `If-Modified-Since` header → **304 Not Modified** (no body, saves bandwidth) if unchanged, else 200 OK + new data.

### HTTPS & TLS
- HTTPS = HTTP layered over **TLS (Transport Layer Security)**, encrypting the connection.
- **TLS handshake**: verifies the server's certificate (chain of trust to a CA), negotiates a cipher suite, and derives session encryption keys — establishing confidentiality, integrity, and (server, optionally client) authentication, versus HTTP's plaintext.

### HTTP Version Evolution (HOL Blocking Story)
| Version | Mechanism | Problem solved | Problem remaining |
|---|---|---|---|
| HTTP/1.0 | New TCP connection per object | — | Slow — repeated handshakes |
| HTTP/1.1 | Persistent connections + pipelining, strict FCFS response order | Saves handshake overhead | **HOL blocking**: a lost packet for object 1 stalls delivery of already-arrived objects 2, 3 (buffered, in-order TCP stream) |
| HTTP/2 | Breaks objects into interleaved **frames** over one TCP connection, multiplexed | App layer can skip a stalled object's frames; faster than 1.1, better bandwidth use | TCP itself still enforces strict byte order — one lost **TCP packet** still stalls the whole connection |
| HTTP/3 (QUIC) | Runs over **UDP**; reliability handled **per stream**, independent multiplexed streams | A lost packet only stalls its own object/stream — others delivered immediately; lowest latency especially on unstable/mobile networks | — |

- QUIC integrates TLS 1.3 into the handshake: 1-RTT (or 0-RTT for returning clients reusing a session ticket) vs TCP+TLS's two serial handshakes.
- **Connection migration**: QUIC identifies connections by a Connection ID (not IP/port), so switching networks (WiFi↔mobile) doesn't break the connection.

### Email
- Components: User Agent (Gmail/Outlook) → Mail Agent (mailbox + outgoing queue) → **SMTP**.
- **SMTP**: handles mail delivery on the TCP/IP stack, over TCP port 25, client push model (vs HTTP's client pull), commands/responses in ASCII with numeric status codes, ends with `CRLF.CRLF`.
- Sequence: HELO → 250 → MAIL FROM → RCPT TO → DATA → `.` → QUIT.
- SMTP (RFC 5321) defines transfer; RFC 2822 defines message syntax.
- **Retrieval protocols**:
  - **IMAP**: mail stays **on the server**; supports folders, retrieval, deletion remotely — good for accessing mail from multiple devices.
  - **POP3**: **downloads** email from the server to the local device (often removing it from the server), enabling offline reading afterward; simpler, widely supported, but less multi-device-friendly than IMAP.
  - **HTTP-based webmail** (Gmail, etc.): browser interface built on top of SMTP (send) + IMAP/POP (retrieve).

### DNS
Services: hostname↔IP translation, host/mail aliasing (CNAME), load distribution (round-robin over multiple A/AAAA records).

- Hierarchy: **Root** → **TLD** (.com) → **Authoritative** server. This **hierarchical structure** (vs a single centralized server) improves scalability and reliability, distributes load across many servers, and removes any single point of failure.
- **Resolution order**: a resolving host first checks its **local OS cache**; only if that misses does it query out to external DNS servers (iterative/recursive, below).
- **Iterative** query: local nameserver asks root → root refers to TLD → TLD refers to authoritative (each hop returns a referral, not the final answer, to keep root/TLD load low — 60-100B queries/day at root, ~72% for nonexistent domains).
- **Recursive**: each server does the work on your behalf and returns the final answer (rare at root due to load).
- **Caching**: results cached with TTL; local resolvers often cache TLD servers to skip a root hop.
- Record types: **A** (name→IPv4), **AAAA** (IPv6), **CNAME** (alias), **NS** (authoritative server for a domain), **MX** (mail server).
- Message format: fixed header (ID, flags: QR/AA/TC/RD/RA/RCODE, question/answer/authority/additional counts) + variable sections.
- **DNS Cache Poisoning**: an attacker injects a **fake DNS record** into a resolver's cache, redirecting users of that resolver to a malicious IP for a legitimate name. Dangerous because it can silently affect every user relying on that (poisoned) resolver. Mitigations: **DNSSEC** (cryptographically signs DNS responses), trusting only reputable DNS servers, and promptly flushing/removing suspicious cache entries.

### What Happens When You Type a URL and Hit Enter
1. **DNS resolution**: browser resolves the domain name to an IP address (local cache → resolver → recursive/iterative lookup as above).
2. **TCP connection establishment**: 3-way handshake to the server's IP.
3. **TLS handshake** (if HTTPS): certificate verification + key negotiation.
4. **HTTP request/response**: browser sends the HTTP request; server responds with the page, which the browser downloads and renders.

### ICMP (Internet Control Message Protocol)
- A **Network Layer** protocol used for **error reporting and diagnostics** (e.g. "destination unreachable," "TTL exceeded" — powers `traceroute`; echo request/reply — powers `ping`).
- Does **not** run over TCP: it carries small control messages, not application data, so TCP's connection setup/reliability/congestion-control machinery would be unnecessary overhead for a one-off diagnostic message.

### Multimedia, DASH, CDN
- Video coding: **spatial** (redundancy within a frame) vs **temporal** (redundancy between frames) coding; CBR vs VBR.
- **DASH**: server hosts video chunks at multiple encoding rates + a manifest file; client monitors its own bandwidth and picks chunk rate/source per-chunk (adaptive bitrate streaming).
- **CDN**: replicate content at geographically distributed servers to avoid single-server bottlenecks. Deployment: "enter deep" (push servers deep into access networks) vs "bring home" (fewer, larger clusters at POPs).

### BitTorrent (P2P deep dive)
- File split into small chunks (~256KB); **tracker** tracks peers in a torrent.
- **Tit-for-tat**: upload to top ~4 peers currently giving you the best rate; **optimistic unchoking** periodically (~30s) gives a random peer a chance in case it reciprocates.
- **Rarest-first**: request rare chunks earlier to keep availability spread across the swarm.
- **Churn**: peers join/leave anytime — protocol must tolerate this.

### IP Multicast
- Delivers one stream to multiple interested receivers efficiently (instead of N separate unicast streams) — targeted at a selected group rather than a full broadcast.
- **Not load-balanced**: traffic from a single source follows **one path/tree** to reach all receivers, even if multiple physical paths exist — ensures consistent delivery to everyone rather than splitting load.
- **CGMP (Cisco Group Management Protocol)**: a Cisco-proprietary protocol where multicast routers inform switches which ports actually have interested multicast receivers, so switches avoid flooding multicast traffic to every port unnecessarily.

---

## 8. Security Basics

### VPN (Virtual Private Network)
Creates a secure, encrypted **tunnel** across an untrusted network (like the Internet), protecting data in transit and enabling safe remote access to a private network.
- **Remote Access VPN**: connects an individual user's device to a private network (e.g. employee working from home).
- **Site-to-Site VPN**: connects entire private networks to each other.
  - **Intranet VPN**: links branches of the *same* organization.
  - **Extranet VPN**: links an organization's network with an external partner's/customer's network.

### IPSec Tunnel Mode
- A mode of IPSec where the **communicating computers themselves don't run IPSec** — instead, a **gateway** at each end creates the encrypted tunnel and transparently secures traffic on their behalf. Common for site-to-site VPNs where individual hosts shouldn't need IPSec configuration.
- (Contrast: **Transport mode** IPSec, where the endpoint hosts themselves apply IPSec directly — not asked here but useful context.)

### Firewall
- A security system that monitors and controls incoming/outgoing traffic based on **predefined rules**, blocking unauthorized access while allowing legitimate traffic through.
- Can be hardware-based or software-based; sits as a barrier between a trusted network and an untrusted one (e.g. the Internet).
- A Layer-7 (application-aware) firewall can also act as a **gateway**, inspecting traffic up through the application layer.
- Protects the internal network from external attacks (hackers, malware) by preventing unauthorized inbound access.

### Proxy Servers
- Sit between clients and the wider Internet, forwarding requests on the client's behalf. This **prevents external hosts from seeing internal client IPs** — the internal network becomes effectively invisible to the outside, masking its physical/topological location (similar security benefit to NAT).
- **SOCKS proxy**: a general-purpose proxy that establishes a TCP connection to the destination on the client's behalf and relays traffic bidirectionally between client and server — protocol-agnostic (unlike an HTTP-only proxy), so it works for many kinds of traffic.

### Denial-of-Service (DoS)
An attack attempting to prevent legitimate users from accessing a network/Internet service — commonly by **overloading a server** with excessive requests so it can't process real traffic. (A **DDoS** — distributed DoS — does this from many sources at once.)

### Authentication & Passwords
- **Authentication**: the process of verifying a user's credentials (typically username/password) before allowing network/system login — a core defense against unauthorized intruder access.
- **Strong password guidance**: mix letters and numbers, mix uppercase/lowercase, avoid easily guessable values (dates, names, dictionary words), prefer longer passwords.
- **Practical network security**: combine updated antivirus on all endpoints, correctly configured firewalls, and proper authentication — no single measure is sufficient alone. Antivirus should be installed on **every** server and workstation, since any user at any machine can introduce malware via removable media.

### Symmetric vs Asymmetric Encryption
| | Symmetric | Asymmetric |
|---|---|---|
| Keys | Same key encrypts and decrypts | Public/private key pair |
| Speed | Faster | Slower |
| Security model | Key must be shared secretly beforehand | Public key can be shared openly |
| Typical role | Bulk **data** encryption | Initial **key exchange** / authentication |

In practice (e.g. TLS): asymmetric crypto is used to securely negotiate a shared secret during the handshake, then symmetric crypto encrypts the actual bulk data for speed — best of both.

- **RSA (Rivest–Shamir–Adleman)**: the most widely used public-key (asymmetric) encryption algorithm — underlies much of TLS's key exchange and digital signatures.
- **Encryption's role generally**: translates readable information into unreadable ciphertext, decryptable only with the correct key/password — so intercepted traffic stays unreadable to an eavesdropper without the right credentials.

---

## 9. Wireless & Mobile Networks

### Wireless Network Modes
- **Infrastructure mode**: hosts connect via a base station (AP/eNodeB) to the wired network; **handoff** = switching base stations while staying connected.
- **Ad hoc mode**: no base station; nodes self-organize and route among themselves (MANET/VANET, mesh nets).

### Wireless Link Characteristics
- **Multipath propagation**: reflected signals arrive at slightly different times; **coherence time** bounds max transmission rate, inversely proportional to carrier frequency and receiver velocity.
- **Adaptive modulation**: dynamically switch modulation scheme/power as SNR changes to maximize rate within an acceptable bit-error rate (e.g., 16-QAM ↔ 64-QAM).
- **Hidden terminal problem**: two senders (A, C) can't hear each other but both reach the same receiver (B) → unaware of mutual interference at B. Solved partly via ACKs and RTS/CTS.

### CDMA
Each user gets a unique orthogonal "chipping" code; all share the same frequency band simultaneously. Encode = data bit × code (inner product); decode = received signal · same code recovers that user's data even amid interference from others.

### WiFi (802.11)
- Uses **CSMA/CA** (see [Link Layer](#2-link-layer) for the CD vs CA comparison) — wireless senders can't reliably listen for collisions while transmitting, and the hidden terminal problem means a sender may not even hear a colliding transmitter, so WiFi *avoids* collisions proactively rather than detecting them after the fact.
- **Sender**: sense channel idle for DIFS → transmit; if busy, random backoff (counts down only while idle); no ACK → increase backoff and retry.
- **Receiver**: ACK after SIFS if frame is correct (needed because of the hidden terminal problem — sender can't otherwise detect a collision).
- **RTS/CTS**: sender sends short RTS; AP broadcasts CTS heard by all nearby nodes, reserving the channel virtually — reduces collision cost since only short RTS packets can collide.
- **Association**: passive (listen for AP beacon frames) or active (broadcast Probe Request, APs reply with Probe Response) scanning, then Association Request/Response.
- **Power management**: sleeping node tells AP; AP buffers frames and lists them in the beacon's Traffic Indication Map (TIM); node wakes at beacons to check.
- Standards summary: 802.11b/g (2.4GHz, spread spectrum/OFDM) → 802.11n (Wi-Fi 4, MIMO) → 802.11ac (Wi-Fi 5) → 802.11ax (Wi-Fi 6, OFDMA) → 802.11be (Wi-Fi 7).

**2.4 GHz vs 5 GHz band**:
| | 2.4 GHz | 5 GHz |
|---|---|---|
| Range | Farther coverage | Shorter range |
| Speed | Slower | Faster |
| Interference | More crowded (shared with many devices/protocols) | Less crowded, more available channels |
| Obstacle penetration | Better (lower frequency penetrates walls better) | Worse |

### Short-Range Wireless: Bluetooth vs WiFi vs ZigBee
| | Bluetooth | WiFi | ZigBee |
|---|---|---|---|
| Range | Short (< 10 m) | Medium (tens of m) | Short–medium |
| Power | Low | Higher | Very low |
| Data rate | Low–moderate | High | Low |
| Typical use | Cable replacement — headphones, keyboards, phone accessories | Internet access, general local networking | IoT sensors, home automation (large low-power mesh networks) |

**Bluetooth details**: **piconet** = 1 master + up to 7 active clients (+ parked/inactive devices); master polls clients. Uses TDM (625μs slots) + frequency-hopping spread spectrum (79 channels, pseudorandom order) to limit interference.

### Cellular (4G LTE / 5G)
| Element | Role |
|---|---|
| UE | Device + SIM (IMSI) |
| eNode-B | Base station; radio resource mgmt + mobility coordination |
| MME | Control plane: auth, security, mobility mgmt |
| HSS | Home network DB: identity, service profile, auth info |
| S-GW | Data plane anchor for intra-LTE mobility, tunnels to eNode-B and P-GW |
| P-GW | Gateway to the Internet, does NAT, tunnel endpoint |

- Radio access: **OFDMA** (combines FDM+TDM); Physical Resource Block = 12 subcarriers × 7 symbols.
- Data plane tunnelling: user IP datagrams wrapped in **GTP over UDP/IP**, tunneled eNode-B ↔ S-GW ↔ P-GW — this is what lets only tunnel endpoints change as a mobile moves.
- **5G**: Service-Based Architecture (microservices-style control plane: AMF, SMF, AUSF, UDM + a User Plane Function), driven by eMBB/mMTC/URLLC use cases.

### Mobility Management
- **Indirect routing**: correspondent always sends to mobile's permanent (home) address; **home agent** intercepts and tunnels to the mobile's current location. Transparent to correspondent, but suffers **triangle routing** inefficiency.
- **Direct routing**: correspondent learns the mobile's **care-of-address** and sends directly — avoids triangle routing, but not transparent (correspondent must be updated on movement), and needs extra handling for further movement.
- **Handover (X2, same network)**: source eNode-B → Handover Request → target eNode-B pre-allocates resources → mobile switches → source forwards buffered data to target → target notifies MME (Path Switch) → MME retargets the S-GW tunnel to the new eNode-B.

### Impact on Higher Layers
Wireless bit errors/handover losses cause packet loss that **TCP misinterprets as congestion**, unnecessarily shrinking cwnd and hurting throughput — a key reason wireless-aware congestion control (e.g., delay-based methods, ECN, SACK) matters.

---

## 10. Command-Line Network Tools

| Tool | Platform | Purpose |
|---|---|---|
| `ping` | Both | Sends ICMP echo requests to test connectivity/reachability to a host and measure RTT. |
| `tracert` / `traceroute` | Windows / Unix | Traces the route a packet takes to a destination, showing each hop (uses ICMP TTL-exceeded messages — increments TTL by 1 each probe to reveal the next router). |
| `ipconfig` / `ifconfig` | Windows / Unix | Displays a host's network configuration — IP address, subnet mask, default gateway, MAC (physical) address. |
| `netstat` | Both | Shows active TCP/IP connections, listening ports, and related socket statistics. |
| `nslookup` / `dig` | Both | Manually queries DNS servers to resolve a hostname (or do a reverse lookup) — useful for diagnosing DNS issues directly, bypassing the OS cache. |

---

## 11. Quick-Fire Interview Q&A

- **Why does TCP need a 3-way (not 2-way) handshake?** A 2-way handshake can't distinguish a fresh connection request from a stale duplicate (e.g., a retransmitted SYN after a timeout) — the 3rd ACK confirms the client actually received the server's SYN-ACK before either side commits state, preventing duplicate/late requests from opening spurious connections or replaying data twice.
- **Why does the client sit in TIME_WAIT after closing?** So that if its final ACK is lost and the server retransmits its FIN, the client is still around to resend the ACK — without this wait, a lost final ACK could leave the server stuck resending a FIN to a connection the client has already abandoned.
- **Why does HTTP/2 still suffer HOL blocking despite framing?** Framing solves *application-layer* HOL blocking (interleaved frames let independent objects progress), but the frames still ride one TCP byte stream — a single lost TCP packet stalls the whole connection because TCP enforces strict in-order delivery.
- **Why does HTTP/3/QUIC fix that?** It replaces TCP with UDP and implements reliability **per stream**, so a lost packet only blocks its own stream/object, not unrelated ones in flight.
- **Why is UDP checksum weaker error protection than CRC?** Checksum is 16-bit one's-complement addition — some bit-flip patterns (e.g., compensating errors) sum to the same value and go undetected; CRC's polynomial division catches all burst errors shorter than the generator length.
- **Why does go-back-N retransmit more than necessary vs Selective Repeat?** GBN's receiver discards any out-of-order packet (no buffering), so a single lost packet forces retransmission of everything after it; SR buffers out-of-order arrivals and the sender only resends the specific lost packet, at the cost of more receiver-side buffering and per-packet timers.
- **Why must Selective Repeat's window size be ≤ half the sequence number space?** With a larger window, an old (already-ACKed, wrapped-around) sequence number could be mistaken for a new one; ≤N/2 guarantees the sender's and receiver's windows never overlap on stale vs fresh use of the same sequence number.
- **Why does a TCP timeout reset cwnd to 1 MSS but triple-dup-ACK only halves it?** A timeout means no ACKs arrived at all — a much stronger signal of severe congestion (or an entirely broken path) — while triple-dup-ACKs mean *most* packets are still getting through and only one segment was lost, a milder congestion signal deserving a milder response (fast recovery, not full slow start). This is exactly the Tahoe (always full reset) vs Reno/New Reno (fast recovery) distinction.
- **Why is TTL needed if IP is best-effort?** Without a hop-count ceiling, a routing loop (e.g., misconfigured forwarding tables) would let a packet circulate forever, wasting bandwidth indefinitely; TTL guarantees eventual drop.
- **Why does IPv6 drop the header checksum?** Every router hop had to recompute IPv4's checksum (since TTL changes each hop), which is expensive at line rate; IPv6 accepts that upper layers (TCP/UDP, link layer) already do error checking, so it trades that redundant safety net for a simpler, fixed-length, faster-to-process header.
- **Why can't routers alone implement reliability well (End-to-End Argument)?** Routers only see individual packets, not the application's notion of "correct arrival" — true reliability requires knowledge only the endpoints have, so hop-by-hop reliability is at best a redundant optimization, and full correctness must live in the endpoints (e.g., TCP).
- **Why does WiFi use CSMA/CA instead of CSMA/CD (like classic wired Ethernet)?** Wireless senders can't listen while transmitting (their own strong outgoing signal drowns out any weak incoming collision), and the hidden terminal problem means a sender may not even hear another transmitter colliding at the receiver — so WiFi avoids collisions proactively (random backoff, RTS/CTS) rather than trying to detect them, whereas wired Ethernet's shared medium lets a sender listen and detect a collision directly.
- **Why does NAT need to track a port mapping, not just an IP mapping?** Many internal hosts share one public IP, so the port number is the only remaining information that lets the router demultiplex return traffic back to the correct internal host/socket.
- **Why does DNS use both iterative and (limited) recursive resolution?** Recursion pushes all the work (and load) onto whichever server receives the query first; keeping root/TLD servers iterative-only bounds their load, while the local resolver (which has far less traffic) can afford to do the recursive legwork on the client's behalf.
- **Indirect vs direct mobile routing — the core tradeoff?** Indirect keeps the mobile's movement fully transparent to the correspondent at the cost of inefficient triangle routing through the home network; direct routing is efficient but breaks transparency, requiring the correspondent (and protocol) to actively track the mobile's care-of-address.
- **Why do lost packets hurt TCP performance more on wireless links than wired?** TCP's congestion control assumes packet loss implies network congestion and reacts by shrinking cwnd; but wireless loss is frequently due to bit errors or handover, not congestion, so TCP throttles throughput unnecessarily even when the network isn't actually overloaded. ECN and SACK help disambiguate the two causes.
- **Why does a switch flood a frame instead of dropping it when the destination MAC is unknown?** It hasn't learned that MAC's port yet — flooding (like a hub, but only until the reply is learned) guarantees delivery on the first try, and the switch learns the correct port from the destination's reply, avoiding future floods for that address.
- **Only a router (or VLAN) — not a switch — separates broadcast domains. Why?** A switch operates at L2 and by design forwards broadcast frames out every port (that's the definition of a broadcast domain); only L3 devices (routers) or logical segmentation (VLANs) stop broadcasts from propagating further.
- **Why does Nagle's algorithm hurt interactive applications?** It intentionally delays small writes to batch them into fewer, larger packets for bandwidth efficiency — but interactive apps (gaming, VoIP, SSH keystrokes) need each small message sent immediately, so the batching delay directly adds to perceived latency; hence `TCP_NODELAY` is commonly set for such traffic.
- **Why is asymmetric encryption used only for the handshake, not the whole session?** It's computationally much slower than symmetric encryption; TLS uses it just once, to securely exchange a symmetric session key, then switches to fast symmetric encryption for the actual bulk data.
- **Why does classless (CIDR) addressing dominate over classful (A/B/C) today?** Classful addressing wastes huge chunks of address space (e.g. a Class C network stuck at exactly 254 hosts, a Class B forced to size 65,534 even if you need 300) — CIDR's variable-length prefixes let a network be sized to exactly what's needed, and also enable route aggregation to shrink backbone routing tables.
- **Why does traceroute work by manipulating TTL rather than a dedicated "trace" protocol?** It sends probes with increasing TTL (1, 2, 3, …); each router that decrements TTL to 0 sends back an ICMP "TTL exceeded" message identifying itself, so incrementing TTL one hop at a time reveals the whole path using only existing IP/ICMP mechanics — no special protocol needed.
- **Why is a proxy server considered a security benefit similar to NAT?** Both hide the internal network's real IP addresses from the outside world — external hosts only ever see the proxy's (or NAT router's) address, making the internal topology invisible and harder to target directly.
