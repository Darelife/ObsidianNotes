# Line Coding

Converting digital bits, into voltages....

---
## Baud Rate

Baud rate is the **number of signal units (symbols) transmitted per second**.
`Bit Rate = Baud Rate × Bits per Signal Unit`

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

---
## A Few Schemes

### Unipolar NRZ

- `1 = +V`, `0 = 0`
- Simple, **DC component + sync problem**

---

### Polar NRZ-L

- `1 = +V`, `0 = −V`
- Bit = **voltage level**, still sync issues

---

### Polar NRZ-I

- `1 = transition`, `0 = no transition`
- Bit = **change**, not level

---

### Polar RZ

- Signal returns to zero every bit
- Better sync, **more bandwidth**

---

### Manchester (Polar Biphase)

- **Mid-bit transition always**
- Direction = data, **clock embedded**
- `Manchester = Data ⊕ Clock`

---

### Differential Manchester

- Mid-bit transition = clock
- Start transition = data
- Polarity-safe

---

### AMI (Bipolar)

- `0 = 0`, `1 = +V / −V alternating`
- No DC, long 0s cause sync issues

---

### Pseudoternary

- `1 = 0`, `0 = +V / −V alternating`
- Same as AMI, roles swapped

---
### MLT2

### MLT3