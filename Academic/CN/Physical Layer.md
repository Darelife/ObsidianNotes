About how raw bits get moved across a physical medium.
Doesn't care about frames, ips, or packets. Just cares about sending signals on the wire or in the air.

# Line Coding

Converting digital bits, into voltages....

We can have multiple voltage levels. If we have 

---
## Baud Rate

Baud rate is the **number of signal units (symbols) transmitted per second**.
`Bit Rate = Baud Rate × Bits per Signal Unit`


# Base Band Transmission
Entire bandwidth is dedicated to just 1 signal. To physically preserve the shape of the digital signal, you technically need an infinite or a very wide bandwidth.
- **Low-pass Channels:** Baseband transmission relies on low-pass channels, which support a range of frequencies starting from 0 up to a certain cutoff frequency (f1​).

# Signal Impairment
- Attenuation:
	- $10 \log(\frac{P_{2}}{P_{1}})$ decibels
- Distortion: Shape of the signal changes, cuz different frequencies travel at different speeds in the medium, causing components to arrive out of phase.
- Noise : Unwanted Data

# Signal to Noise (SNR) Ratio
- SNR = $\frac{P_{signal}}{P_{noise}}$
- Eg: $\frac{10 mW}{10 \mu W} = 10^4$
- $SNR_{db} = 10\log_{10}(SNR) = 40$

# Fundamental Limits
- **Nyquist Theorem:** Defines the maximum bit rate for a channel in an _ideal, noiseless_ environment. Tells the number of signal levels that we need.
	- Bit Rate = $2 \times Bandwidth \times \log_{2}(Levels) \ bps$
- Shannon Capacity: Defines the maximum capacity that can be transmitted when noise is present. Gives the upper limit.
	- C = $B\log_{2}(1+SNR) \ bps$ 
- Bandwidth
	- In hz : Frequency gap
	- In bps : Speed at which bits can be transmitted
- Throughput: While bandwidth is a theoretical maximum, throughput is the actual rate at which data is successfully delivered over a network.
- Latency
	- Transmission Delay: time it takes to get the data from the PC to the network link
	- Propagation Delay: time it takes on the network link
- Bandwidth Delay Product: Defines the maximum amount of data that can be filling the ink at any given time = $bandwidth \times delay$ 

# The 4 conversion types
## Digital to Digital

This process maps digital bits directly onto digital signal levels.
- Line Coding : You convert individual bits into signal transitions
	- **Synchronization:** Ensuring sender and receiver clocks align.
	- **Baseline Wandering:** Preventing a drift in voltage levels.
- **Block Coding:** Instead of converting bit-by-bit, you map groups (e.g., an m-bit group to an n-bit group) to improve sync and error detection, such as 4B/5B encoding.

### Line Coding
### 1. Unipolar (NRZ)

- **Mechanism:** Uses only one polarity (usually positive and zero). A '1' is typically a positive voltage, and a '0' is zero voltage.
    
- **Challenge:** It is susceptible to baseline wandering and lacks synchronization because a long string of 0s or 1s results in a flat signal.

### 2. Polar Schemes

These use two voltage levels (positive and negative) to represent bits.

- **NRZ-L (Non-Return-to-Zero Level):** The voltage level determines the bit value (e.g., positive for '1', negative for '0').
    
- **NRZ-I (Non-Return-to-Zero Invert):** The signal _inverts_ (changes) if the bit is a '1' and remains the same if the bit is a '0'. This is better for synchronization than NRZ-L.
    
- **RZ (Return to Zero):** Uses three values (positive, negative, and zero). The signal returns to zero in the middle of each bit, which provides inherent synchronization but uses more bandwidth.
    
- **Manchester:** Combines the RZ and polar logic. There is always a transition in the middle of each bit, ensuring synchronization.
    
- **Differential Manchester:** A transition at the start of the bit signifies a '0', while no transition signifies a '1'.

### 3. Bipolar Schemes

These use three levels: positive, negative, and zero.

- **AMI (Alternate Mark Inversion):** A '0' is represented by a zero voltage, and a '1' is represented by alternating positive and negative voltages. This prevents baseline wandering.
    
- **Pseudoternary:** The inverse of AMI; '1' is zero voltage, and '0' alternates between positive and negative.

### 4. Multilevel & Multitransition

These are designed to increase data rate or reduce bandwidth.

- **2B1Q (Two Binary, One Quaternary):** Maps 2 bits to 1 of 4 voltage levels. This transmits 2 bits for every 1 signal, effectively doubling efficiency.
    
- **MLT-3 (Multilevel Transmit-3):** Uses three levels (0, +V, -V). It cycles through them to reduce the required frequency (bandwidth) of the signal.

### Understanding Block Coding (mB/nB)

Block coding improves synchronization and error detection. It is essentially adding "redundancy" to the data.

**Why do we need it?** If you use a line coding scheme like NRZ-I, a long string of '0's might cause the receiver to lose its clock synchronization (it doesn't see enough transitions).

**The 4B/5B Solution:**

- You take a 4-bit group (which has 16 possible combinations) and substitute it with a 5-bit code (which has 32 possible combinations).
    
- **The Benefit:** Because we only use 16 of the 32 available 5-bit patterns, we can choose the patterns that _guarantee_ enough signal transitions (like 0s and 1s alternating) to keep the receiver synchronized.
    
- The extra patterns that aren't used for data and are often used as "control signals" or to detect errors if an invalid pattern arrives.

## Analog to Digital
#### Pulse Code Modulation
- **Sampling:** Measuring the amplitude of the analog signal at discrete time intervals.
	- Can be ideal, natural, or flat top sampling
	- _Nyquist Theorem:_ To accurately reconstruct the signal, the sampling rate (fs) must be at least twice the highest frequency (f) in the signal: fs​≥2f.
- **Quantizing:** Mapping the continuous amplitude values of the samples to a finite set of discrete levels.
- **Encoding:** Converting the quantized values into binary digital data

#### Data Modulation
A simpler alternative that records the _change_ in amplitude (delta) from the previous sample rather than the absolute value, using a comparator, a delay unit, and a staircase maker.

### Transmission Modes

This defines how binary data flows across a link, based on timing and the number of channels/wires used.

- **Parallel Transmission:** Multiple bits (e.g., an 8-bit group) are sent together simultaneously with each clock tick.
    - _Requirement:_ We need multiple lines/wires (one for each bit sent at a time).
- **Serial Transmission:** 1 bit is sent per clock tick, one after another.
    - _Requirement:_ Needs only one line (wire), requiring parallel/serial converters at the sender and receiver.
    - _Subclasses of Serial:_
        - **Asynchronous:** Data is sent in frames with gaps between data units. It uses **start bits** and **stop bits** to frame the data.
        - **Synchronous:** Data is sent as a continuous stream of bits (frames) without any gaps.
        - **Isochronous:** Ensures data is delivered with steady, **fixed timing** (no retries).

## Digital to Analog

- ASK (Amplitude Shift Keyring) : Modulates the signal by changing the amplitude of the carrier wave.
- FSK : Modulates the signal by changing the frequency of the carrier wave.
- PSK (Phase Shift Keying) :  Modulates the signal by changing the phase of the carrier wave.
- QAM (Quadrature Amplitude Modulation): A highly efficient technique that is a combination of both ASK and PSK. By changing both the amplitude and the phase of the carrier signal at the same time, QAM can transmit more bits per symbol than ASK or PSK alone.

## Analog to Analog

This is the representation of analog information by an analog signal. It is necessary to modulate an already-analog signal if the transmission medium is strictly bandpass in nature.

- Amplitude Modulation (AM)
- Frequency Modulation (FM)
- Phase Modulation (PM)

# Bandwidth Utilization
## Multiplexing

1. Frequency Division Multiplexing (FDM)
	1. Combines multiple analog signals by assigning each signal a different, non overlapping frequency band.
	2. Empty frequency spaces intentionally left between the assigned channels to prevent the signals from interfering with one another. 
2. Wave Division Multiplexing (WDM)
	1. Specifically for fibre optic cables.
	2. Multiple analog signals are transmitted, simultaneously over a single fibre, by using different colours (or wavelengths) of light (each carrying an independent signal)
3. Time Division Multiplexing
	1. Multiple digital signals share the same channel by transmitting in different non overlapping time slots.
	2. Interleaving: The process of taking time slots from multiple inputs and arranging them sequentially in a repeating frame so they don't overlap.
	3. If one source is faster than others, it can become a bottleneck. TDM can assign multiple time slots to higher-rate sources. It can also use Pulse Stuffing (add extra bits to increase speed) to pad slower channels to match the standard rate.

## Spread Spectrum

Instead of trying to pack as many signals into a space as possible (efficiency), Spread Spectrum combines signals from different sources to fit into a _larger_ bandwidth to prevent eavesdropping and jamming by adding redundancy.

- FHSS (Frequency Hopping Spread Spectrum): The carrier frequency rapidly switches (hops) among many different frequencies in a pseudorandom pattern. A lot of different signals can occupy different frequencies, and keep switching with time, unlike Frequency Division Multiplexing, where each signal has an assigned frequency.
- DSSS (Direct Sequence Spread Spectrum): The original data signal is mathematically multiplied by a high-rate pseudorandom "spreading code." This spreads the signal over a wide bandwidth, improving resistance to noise.

# Transmission Media
1. Guided (wired)
	1. Twisted pair cable : Uses two insulated copper wires twisted together.
	2. Coaxial cable : Has an inner conductor that carries the signal and an outer metallic conductor (shield) that provides a return path and prevents electromagnetic interference
	3. Fibre Optic : Works on refraction. 
		1. Step-Index MMF (Multimode): Has an abrupt change in the refractive index. Used for very short distances.
		2. Graded-Index MMF: Gradually varies the refractive index to reduce signal dispersion. Used for short-to-medium distances like LANs.
		3. Single-Mode: Allows only one light path, providing the highest data rates and longest distances (like long-haul ISP links).
2. Unguided (wireless)
	1. Radio Waves (Ground propagation : below 2MHz, Sky propagation : 2-30MHz, Line of sight propagation : Above 30 MHz)
	2. Micro Waves
	3. Infrared Waves

# Delay
- Processing Delay (Dproc​): Time taken by routers/switches to process the packet header.
- Queuing Delay (Dqueue​): Time a packet spends waiting in a buffer. Note: Network congestion directly increases queuing delay.
- Transmission Delay (Dtrans​): Time to push all the packet's bits onto the wire.
- Propagation Delay (Dprop​): Time for the physical signal to travel the distance of the wire. Note: Physical distance increases propagation delay.

# Transmission Modes (Switching)

- Circuit Switching A dedicated physical path is established before transmission begins. This offers guaranteed bandwidth and low latency, but it wastes resources because the line is held open even when idle.

- Packet Switching: Data is broken into packets and sent over shared paths. This improves efficiency and scalability, but because resources are shared, it can cause variable delays (jitter) and potential packet loss.
---

## Challenges

### 1. Flat Signal (Synchronization Problem)

A long constant signal with no transitions prevents the receiver from extracting clock information, leading to loss of bit synchronization.

### 2. Too Many Changes (Hardware Complexity)

Frequent signal transitions require faster and more complex hardware, increasing cost, power consumption, and design difficulty.

### 3. Sharp Edges (High Frequency / Bandwidth Demand)

Abrupt voltage changes introduce high-frequency components, increasing bandwidth requirements and causing signal distortion in the channel. (Sharp edge → needs **many high-frequency sine waves**, and Bandwidth = f_max − f_min)

### 4. Too Many Signal Levels (Noise Sensitivity)

Using multiple voltage levels makes the system more vulnerable to noise, increasing the probability of incorrect signal detection.
