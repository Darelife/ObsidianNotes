# Chapter 7: Wireless and Mobile Networks  
## Detailed Notes from Lectures 37 & 38 (Kurose & Ross 8th Edition)

---

### 7.1 Introduction
Wireless and mobile networks have become ubiquitous. This chapter covers:

- **Wireless** links, characteristics, WiFi, and cellular (4G/5G).
- **Mobility** management principles and practice (4G/5G, Mobile IP), and the impact on higher-layer protocols.

---

## PART I – WIRELESS

### 7.2 Elements of a Wireless Network

#### 7.2.1 Key Components

- **Wireless hosts**: end-system devices (laptops, smartphones, IoT) that run applications.
- **Base stations**: typically connected to a wired network; relay packets to/from wireless hosts. In cellular: eNode‑B; in WiFi: Access Point (AP).
- **Wireless link**: used to connect mobiles to base stations, or as backbone links. Multiple access protocol coordinates link access; various transmission rates and distances, depending on frequency bands.

#### 7.2.2 Infrastructure Mode
- Base station connects mobiles into a wired network.
- **Handoff**: mobile changes base station while maintaining connection to the wired network.

#### 7.2.3 Ad Hoc Mode
- No base stations.
- Nodes can only transmit to other nodes within link coverage.
- Nodes self‑organize, routing among themselves.

#### 7.2.4 Wireless Network Taxonomy

| | **Single hop** | **Multiple hops** |
|---|---|---|
| **Infrastructure** (e.g., APs) | Host connects to base station (WiFi, cellular) which connects to larger Internet. | Host may relay through several wireless nodes to connect to larger Internet: **mesh net**. |
| **No infrastructure** | No base station, no connection to larger Internet (Bluetooth, ad hoc nets). | No base station, no connection to larger Internet. May need relay to reach other nodes: **MANET**, **VANET** |

---

### 7.3 Wireless Link Characteristics

#### 7.3.1 Multipath Propagation
- Radio signal reflects off objects (ground, buildings), arriving at destination at slightly different times.
- **Coherence time** (\(T_c\)): amount of time a bit is present in the channel to be received. It influences the maximum possible transmission rate because coherence times cannot overlap. It is inversely proportional to the carrier frequency and to the receiver’s velocity.

#### 7.3.2 Wireless Spectrum Bands
**WiFi bands:**
- **2.4 GHz**: 11–14 channels, depending on country.
- **5 GHz**: more than 150 configurable channels with different transmission rates.
- **6 GHz**: recently added, more than 250 configurable/selectable channels.

**5G spectrum (three bands):**
- **Low band (< 1 GHz)**: long distances (tens of miles), lower speeds (50–250 Mbps).
- **Mid band (1–6 GHz)**: balance of distance (~5 miles) and speed (100–900 Mbps). Popular bands: 1.8, 3.3–3.8, 6 GHz.
- **High band (25–66 GHz, mmWave)**: very short distances (< 1 mile), very high speeds (< 3 Gbps). Requires line‑of‑sight; poor penetration through obstacles.

#### 7.3.3 Adaptive Modulation
- A given modulation scheme has a nominal data rate (assuming no errors).
- Signal‑to‑Noise Ratio (SNR) can vary due to changing interference or transmitter power.
- **Idea**: dynamically change modulation scheme and/or power to achieve the highest possible data rate while maintaining an acceptable Bit Error Rate (BER) and energy expenditure, as SNR changes.
- *Example*:  
  - 16 QAM: max 11.5 Mbps  
  - 64 QAM: max 26.9 Mbps  
  (Switch between them based on current SNR.)

#### 7.3.4 Hidden Terminal Problem
- Occurs when two sender nodes (A and C) cannot hear each other, but both can reach the same receiver (B).
- Result: A and C are unaware of their mutual interference at B.
- Attenuation (signal weakening due to obstacles) can also create “hidden terminals” where two nodes that normally would hear each other cannot, causing interference at an intermediate receiver.

---

### 7.4 Code Division Multiple Access (CDMA)

- A unique “code” (chipping sequence) is assigned to each user; code set partitioning.
- All users share the same frequency band, but each encodes data using its own code.
- Orthogonal codes allow multiple users to transmit simultaneously with minimal interference.
- **Encoding**: multiply original data bit by chipping sequence (inner product).
- **Decoding**: take inner product of received summed signal with the same chipping sequence.

**Example (two-sender interference):**
- Sender 1 uses code (+1, +1, +1, +1), Sender 2 uses code (+1, –1, +1, –1).
- The channel sums the two transmissions.
- Receiver knowing Sender 1’s code can recover Sender 1’s original data from the summed channel data, even with the interference from Sender 2.

---

## 7.5 WiFi: IEEE 802.11 Wireless LANs

### 7.5.1 IEEE 802.11 Standards Evolution

| IEEE standard | Wi‑Fi gen | Year | Max theoretical data rate | Frequency (GHz) | Bandwidth (MHz) | PHY techniques |
|---------------|-----------|------|---------------------------|-----------------|-----------------|----------------|
| 802.11b | – | 1999 | 11 Mbps | 2.4 | 20 | Spread spectrum |
| 802.11g | – | 2003 | 54 Mbps | 2.4 | 20 | OFDM |
| 802.11n | Wi‑Fi 4 | 2009 | 600 Mbps | 2.4, 5 | 20, 40 | OFDM, MIMO |
| 802.11ac | Wi‑Fi 5 | 2013 | 6.9 Gbps | 5 | 20, 40, 80, 160 | OFDM, MIMO |
| 802.11ax | Wi‑Fi 6 | 2020 | 9.5 Gbps | 2.4, 5 | 20, 40, 80, 160 | OFDM, OFDMA, MIMO |
| 802.11be | Wi‑Fi 7 | 2024 | 30+ Gbps | 2.4, 5, 6 | 20, 40, 80, 160, 320 | OFDM, OFDMA, MIMO |

All use **CSMA/CA** for multiple access and have both infrastructure and ad‑hoc versions.

### 7.5.2 802.11 Architecture
- **Access Point (AP)**: base station.
- **Basic Service Set (BSS)**: a cell containing one AP and associated wireless hosts (infrastructure mode) or only hosts (ad‑hoc mode).
- A wireless host communicates with the AP over the wireless link.

### 7.5.3 Channels
- Spectrum is divided into channels at different frequencies.
- AP administrator chooses the frequency for the AP.
- **Interference** can occur if neighbouring APs use the same channel.
- *Example*: In 2.4 GHz band, typical 11–14 channels; only three non‑overlapping channels (1, 6, 11) in most regions.

### 7.5.4 Association & Scanning
A wireless host must **associate** with an AP before sending/receiving data.

**Passive Scanning:**
1. APs send **beacon frames** periodically.
2. Host sends **Association Request** to selected AP.
3. AP responds with **Association Response**.

**Active Scanning:**
1. Host broadcasts **Probe Request** frame.
2. APs reply with **Probe Response** frames.
3. Host chooses AP and sends **Association Request**.
4. AP sends **Association Response**.

### 7.5.5 802.11 Frame Addressing
802.11 frames contain up to four MAC address fields, used differently depending on the scenario. A typical infrastructure mode case (host → AP → router) uses three addresses:
- **Address 1**: MAC of the wireless receiver (the AP).
- **Address 2**: MAC of the wireless transmitter (the host).
- **Address 3**: MAC of the destination (the router on the wired side).
``
When a frame goes from AP to a wireless host, Address 1 is the host, Address 2 is the AP, and Address 3 is the source router’s MAC. The fourth address is used only in Wireless Distribution Systems (WDS).

### 7.5.6 CSMA/CA and Collision Avoidance
WiFi uses Carrier Sense Multiple Access with Collision Avoidance (CSMA/CA).

**802.11 sender:**
1. If channel sensed idle for **DIFS** (Distributed Inter‑Frame Space), transmit entire frame.
2. If channel busy, start a random backoff timer that counts down only while the channel is idle. Transmit when timer expires. If no ACK is received, increase the backoff interval and repeat step 2.

**802.11 receiver:**
- If frame received correctly, send ACK after **SIFS** (Short Inter‑Frame Space). ACK is needed because of the hidden terminal problem.

**RTS/CTS (optional mechanism):**
- Sender transmits a short **Request‑To‑Send (RTS)** packet to the AP using CSMA/CA. RTSs may still collide but are short.
- AP broadcasts a **Clear‑To‑Send (CTS)** packet, heard by all nodes.
- Sender then transmits its data frame; other stations defer their transmissions.
- Reserves the channel virtually (virtual carrier sensing).

### 7.5.7 Rate Adaptation
- Base station and mobile dynamically change the physical‑layer modulation technique as the mobile moves and SNR changes.
- When BER becomes too high (SNR too low), switch to a lower transmission rate that is more robust against errors.

### 7.5.8 Power Management
- Node tells the AP: “I am going to sleep until the next beacon frame.” AP buffers frames destined for the sleeping node.
- Beacon frame contains a **Traffic Indication Map (TIM)** listing which sleeping nodes have pending frames.
- Node wakes up, checks the beacon, stays awake if frames are waiting; otherwise returns to sleep until the next beacon.

### 7.5.9 Mobility within the Same IP Subnet
- H1 remains in the same IP subnet → IP address can remain unchanged.
- When H1 moves to a different AP, the Ethernet switch sees H1’s frame on a new port and uses **self‑learning** to update its forwarding table.

---

### 7.6 Personal Area Networks: Bluetooth

- Short range (< 10 m diameter).
- Cable replacement (mouse, keyboard, headphones).
- Ad‑hoc: no infrastructure.
- Operates in 2.4–2.5 GHz ISM band, up to 3 Mbps.
- **Piconet**: a master controller and up to seven active client devices, plus many parked (inactive) devices.
- Master polls clients and grants transmission requests.

**Multiple access techniques:**
- **Time Division Multiplexing (TDM)**: 625 µs time slots.
- **Frequency Division Multiplexing (FDM)** with **frequency hopping spread spectrum**: 79 different frequency channels, used in a known pseudo‑random order slot‑to‑slot. This limits interference because other devices only collide on some slots.
- **Parked mode**: clients sleep to preserve battery; later wake up and re‑synchronise.
- **Bootstrapping**: nodes self‑assemble into a piconet (plug and play).

**Bluetooth and Pandemic (contact tracing):**
- Phones exchange anonymous, frequently changing identifier beacons during encounters.
- If a person is later diagnosed with a disease, with consent their phone uploads the last 14 days of beacon keys to a cloud server.
- Other users’ devices can check if they have been exposed, preserving privacy.

---

### 7.7 Cellular Networks: 4G and 5G

#### 7.7.1 Overview
- Provide wide‑area mobile Internet connectivity.
- In 2019, mobile‑broadband devices outnumbered fixed‑broadband devices 5:1.
- 4G availability: 97% of time in Korea, 90% in US.
- Transmission rates up to hundreds of Mbps.
- Standardized by **3GPP** (3rd Generation Partnership Project); 4G standard is **Long‑Term Evolution (LTE)**.

**Similarities to wired Internet:**
- Edge/core distinction (both belong to the same carrier).
- Global network of networks.
- Uses familiar protocols: HTTP, DNS, TCP, UDP, IP, NAT, separation of data/control planes, SDN, Ethernet, tunnelling.
- Interconnected to the wired Internet.

**Differences from wired Internet:**
- Different wireless link layer.
- Mobility is a first‑class service.
- User identity via **SIM** card (IMSI).
- Subscription business model; users subscribe to a cellular provider.
- Strong notion of **home network** vs **visited network** when roaming.
- Global authentication infrastructure and inter‑carrier settlements.

#### 7.7.2 4G LTE Architecture Elements

**User Equipment (UE):**
- Device with 4G LTE radio (smartphone, tablet, etc.).
- Contains a 64‑bit **International Mobile Subscriber Identity (IMSI)** stored on the SIM card.

**Base station (eNode‑B):**
- Sits at the “edge” of the carrier’s network.
- Manages radio resources and mobile devices in its cell.
- Coordinates device authentication.
- Similar to WiFi AP but has an active role in mobility and coordinates with neighbouring base stations to optimise radio usage.

**Mobility Management Entity (MME):**
- Control‑plane element.
- Handles authentication, security, and mobility management.
- Contacts the home HSS using the mobile’s IMSI.

**Home Subscriber Server (HSS):**
- Home network database containing subscriber identity, service profiles, and authentication info.
- Used both when the user is in the home network and when roaming.

**Serving Gateway (S‑GW):**
- Data‑plane anchor point for intra‑LTE mobility.
- Tunnels user data to/from the eNode‑B; also tunnels to the P‑GW.

**PDN Gateway (P‑GW):**
- Gateway between the mobile cellular network and external packet data networks (Internet).
- Acts like any other Internet gateway router, provides NAT, and is the endpoint for tunnels from the S‑GW.

#### 7.7.3 Radio Access Network (4G LTE)
- Connects UE to eNode‑B.
- Many possible frequency bands (600, 700, 850, 1500, 1700, 1900, 2100, 2600, 3500 MHz…), with separate uplink and downlink channels.
- **OFDM(A)**: Orthogonal Frequency Division Multiplexing (Multiple Access). Combines FDM and TDM.  
  - In time domain: a 0.5 ms slot contains 7 OFDM symbols (normal cyclic prefix).  
  - In frequency domain: subcarriers spaced at 15 kHz.  
  - A **Physical Resource Block (PRB)** = 12 subcarriers × 7 OFDM symbols = 84 **resource elements**, the basic scheduling unit.  
- Hundreds of Mbps possible per user.

**Transmission scheduling example:** Within one PRB, different resource elements can be assigned to up to 7 different UEs.

#### 7.7.4 Data Plane Protocol Stack and Tunnelling

**First hop (UE to eNode‑B):**
- PHY, MAC, RLC, PDCP layers.

**From eNode‑B to core:**
- User IP datagrams are encapsulated using the **GPRS Tunnelling Protocol (GTP)**.
- GTP‑encapsulated packet is carried inside a UDP datagram, which is sent to the S‑GW.
- S‑GW re‑tunnels the datagram (again GTP/UDP) to the P‑GW.
- **Tunnelling supports mobility**: only the tunnel endpoints change when the mobile user moves; the correspondent and the rest of the Internet see only the home P‑GW as the mobile’s address.

#### 7.7.5 Control Plane / Data Plane Separation
- **Data plane** implements new link/physical layer protocols and extensive tunnelling to handle mobility.
- **Control plane** introduces new protocols for mobility management, security, and authentication (MME, HSS, etc.).

#### 7.7.6 Association with a Base Station
1. eNode‑B broadcasts a **primary synchronisation signal** every 5 ms on all its operating frequencies.
2. Mobile finds a primary sync signal, then locates a secondary sync signal on the same frequency.
3. Mobile decodes system information (MIB/SIB) broadcast by the base station: channel bandwidth, configurations, carrier identity, etc. It may receive such info from multiple base stations and multiple cellular networks.
4. Mobile selects which base station (and carrier) to associate with (typically preferring its home carrier).
5. Further steps for authentication, state establishment, and data‑plane setup follow.

#### 7.7.7 Sleep Modes
- To conserve battery, LTE radios can sleep:
  - **Light sleep**: after 100s of ms of inactivity. Wake periodically to check for downlink transmissions.
  - **Deep sleep**: after 5–10 s of inactivity. Mobile may change cells while deep sleeping; needs to re‑establish association when waking up.

#### 7.7.8 Global Cellular Network: All‑IP
- Home network HSS stores identity and service information, available to visited networks.
- Carriers interconnect at IP exchange points; entire core network is IP‑based.
- Legacy 2G/3G are handled separately (not all‑IP).

#### 7.7.9 5G Motivation and Architecture
**Drivers:**
- Enhanced Mobile Broadband (eMBB).
- Massive Machine Type Communications (mMTC).
- Ultra‑Reliable and Low Latency Communications (URLLC).
- Enablement of numerous industry verticals: manufacturing, construction, transport, health, smart communities, agriculture, finance.

**5G Core Architecture:**
- Compared with 4G’s “boxes and protocols”, 5G adopts a **Service‑Based Architecture (SBA)** similar to microservices.
- Control‑plane functions (AMF, SMF, AUSF, UDM, etc.) and a **User Plane Function (UPF)** replace the older monolithic nodes.
- Network agility through cloud technologies (SDN/NFV); features can be introduced rapidly, even bottom‑up via private 5G deployments.
- “Beyond 5G” / “NextG” follows an evolutionary path, with customisation and continuous deployment, reducing the need for global standardisation cycles.

---

## PART II – MOBILITY

### 7.8 What is Mobility?
- Spectrum of mobility from the network perspective: from no mobility (fixed) to high mobility (vehicular).
- Challenge: if a device moves from one network to another, how does the network know where to forward packets?

### 7.9 Mobility Approaches

**1. Let the network (routers) handle it:**
- Routers advertise a well‑known permanent address (e.g., 32‑bit IP address) of the mobile node via standard routing table exchanges.
- The Internet could already do this with no changes! Routing tables would indicate each mobile’s location using longest prefix match.
- *Problem:* scale – routing tables would need an entry for every mobile host.

**2. Let end‑systems handle it (functionality at the “edge”):**
- **Indirect routing**: correspondence goes through the mobile’s **home network**, which forwards to the remote mobile via tunnelling.
- **Direct routing**: correspondent learns the mobile’s **care‑of‑address** (foreign address) and sends directly.

### 7.10 The Importance of a Home Network
- A “home” is a definitive source of information about a mobile node.
- Analogous to having a permanent address where people can find your current location (e.g., parents’ house, Facebook profile).
- In mobile networks, the **home network** stores subscriber identity, service profile, and current location.

**Home network vs Visited network (4G/5G):**
- **Home network**: the cellular provider with which the user has a subscription (e.g., Verizon). The **Home Subscriber Server (HSS)** stores identity and service info.
- **Visited network**: any network other than the home network. An agreement between operators allows visiting mobiles to access services.

### 7.11 Mobility with Indirect Routing
- Correspondent sends packets to the mobile’s permanent address (in the home network).
- **Home agent** (e.g., P‑GW in LTE) intercepts these packets, encapsulates them, and tunnels them to the mobile’s current location (visited network).
- Mobile registers with the home agent each time it changes visited network.
- **Triangle routing** can be inefficient: if correspondent and mobile are in the same visited network, packets still travel to the home network and back.
- Advantage: mobile’s movement is completely transparent to the correspondent; ongoing TCP connections can be maintained.

### 7.12 Mobility with Direct Routing
- Correspondent obtains the mobile’s care‑of‑address (from the home agent) and sends directly to the visited network.
- Overcomes triangle routing inefficiency.
- Not transparent to the correspondent; the correspondent must be updated when the mobile moves.
- Handling mobility when the mobile changes visited networks adds complexity (forwarding from old foreign agent, etc.).

### 7.13 LTE Mobility in Practice

#### 7.13.1 Control‑Plane Configuration
1. UE identifies itself with IMSI during base station association.
2. The visited network’s **MME** contacts the home **HSS**; authentication and encryption parameters are established.
3. HSS learns that the mobile is now resident in this visited network.
4. BS and mobile agree on parameters for the radio data‑plane channel.

#### 7.13.2 Data‑Plane Configuration (Indirect Routing via GTP)
- MME configures forwarding tunnels:
  - **S‑GW ⇄ BS tunnel**: carries user IP datagrams encapsulated in GTP/UDP/IP.
  - **S‑GW ⇄ home P‑GW tunnel**: also GTP/UDP/IP, linking the visited network to the home network’s gateway to the Internet.
- This implements indirect routing: all traffic to the mobile passes through the home P‑GW.

#### 7.13.3 Handover Between Base Stations (Same Cellular Network)
*X2‑based handover procedure:*

1. Source eNode‑B decides to hand over to a target eNode‑B (based on measurement reports). It sends a **Handover Request** to the target BS.
2. Target BS pre‑allocates radio resources and replies with a **Handover Request ACK** containing information for the mobile.
3. Source BS tells the mobile to switch to the target BS (sends RRC Reconfiguration). At this point, mobile detaches from the old cell and synchronises with the new one.
4. Source BS stops sending data to the mobile; instead it forwards buffered and in‑transit data to the target BS, which delivers them to the mobile over the radio channel.
5. Target BS informs the MME that it is now serving the mobile (“Path Switch Request”).
6. MME instructs the S‑GW to change the tunnel endpoint from the source BS to the target BS.
7. Target BS acknowledges the source BS; handover is complete, source BS releases resources.
8. Now mobile’s data flows through the new tunnel.

---

### 7.14 Impact on Higher‑Layer Protocols

**Logically**, mobility and wireless should have minimal impact – the best‑effort service model remains unchanged; TCP and UDP work over wireless and mobile networks.

**But performance‑wise:**
- Wireless links suffer from bit errors and handover losses, causing packet loss or increased delay (due to link‑layer retransmissions).
- TCP interprets packet loss as congestion, triggering unnecessary congestion‑window reduction and decreasing throughput.
- Real‑time applications are sensitive to delay and jitter introduced by wireless link variations and handovers.
- Bandwidth is often a scarce resource over wireless, requiring careful design (e.g., header compression, optimisation of higher‑layer protocols for wireless).

---

### 7.15 Summary: Key Concepts

- **Wireless link characteristics**: multipath, adaptive modulation, hidden terminals, CDMA.
- **WiFi (802.11)**: CSMA/CA, RTS/CTS, scanning, association, rate adaptation, power management.
- **Bluetooth**: piconet, master/slave, frequency hopping, TDM.
- **4G/5G cellular**: LTE architecture (UE, eNode‑B, MME, HSS, S‑GW, P‑GW), OFDMA, GTP tunnelling, separation of control/data planes, 5G SBA.
- **Mobility**: home/visited network, indirect vs direct routing, LTE control‑plane setup, handover procedure, impact on TCP.

---

*These notes synthesise all content from Lectures 37 and 38 of CS F303 Computer Networks, covering Chapter 7 of Kurose & Ross (8th edition).*