# Lecture 20: CAP Theorem & PACELC in Distributed Databases

---

## 1. Introduction to the CAP Theorem

Formulated by Eric Brewer in 2000 and mathematically proven by Seth Gilbert and Nancy Lynch in 2002, the **CAP Theorem** (also known as Brewer's Theorem) dictates the fundamental physical limitations of distributed data systems.

```
                                  THE CAP TRIANGLE
                                     Consistency
                                        /   \
                                       /     \
                                      /   *   \
                                     /  CP  AP \
                                    /           \
                           Availability ------- Partition Tolerance
```

---

## 2. Formal Definitions of the Three Guarantees

### 2.1 Consistency ($C$) — Linearizability
* **Definition**: Every read operation receives the **most recent write** or returns an error. All nodes in the cluster view identical data at the exact same logical instant in time.
* *Note*: CAP Consistency refers strictly to **Linearizability** (Atomic Consistency), which is distinct from the 'C' in ACID (which refers to schema invariant preservation).

### 2.2 Availability ($A$)
* **Definition**: Every non-failing node returns a **non-error response** to every received request, without guarantee that it contains the most recent write.
* *Rule*: The system cannot return an error or time out; it must respond.

### 2.3 Partition Tolerance ($P$)
* **Definition**: The system continues to operate despite an arbitrary number of messages being dropped or delayed by the network between nodes (**Network Partition**).

---

## 3. The Core Imperative: You Cannot Drop $P$ in a Distributed System!

In a real-world physical network, network partitions (severed cables, router drops, packet loss) are **inevitable physical realities**. Therefore, a distributed system **MUST choose Partition Tolerance ($P$)**.

The true choice imposed by the CAP theorem during a network partition is between **Consistency ($C$)** OR **Availability ($A$)**.

```
                   DURING A NETWORK PARTITION (P):
                   /                             \
                  /                               \
        CHOOSE CONSISTENCY (CP)         CHOOSE AVAILABILITY (AP)
        - Cancel operation / Error      - Return available local data
        - Guarantees strict correctness - Responds immediately
        - Sacrifices Availability        - Sacrifices Consistency (Stale)
```

---

## 4. CP Systems vs. AP Systems Taxonomy

| Attribute | CP Systems (Consistency + Partition Tolerance) | AP Systems (Availability + Partition Tolerance) |
| :--- | :--- | :--- |
| **Partition Behavior** | Blocks or errors out reads/writes until partition heals | Returns local node state immediately (stale data possible) |
| **Consistency Level** | Linearizable (Strict Strong Consistency) | Eventual Consistency / BASE |
| **Typical Databases** | HBase, MongoDB (Primary), CockroachDB, Redis | Apache Cassandra, DynamoDB, Couchbase |
| **Best For** | Banking ledgers, Stock exchanges, Reservation systems | Social media feeds, Shopping carts, Like counters |

---

## 5. Beyond CAP: The PACELC Theorem

Formulated by Daniel Abadi in 2012, the **PACELC Theorem** extends CAP by recognizing that network partitions are rare in normal operations. PACELC incorporates system trade-offs when the network is operating **NORMALLY** (no partitions).

```
                      +-----------------------------------+
                      |         PACELC THEOREM            |
                      +-----------------------------------+
                      | IF Partition (P):                 |
                      |   Choose Availability (A) OR      |
                      |   Consistency (C)                 |
                      | ELSE (E):                         |
                      |   Choose Latency (L) OR           |
                      |   Consistency (C)                 |
                      +-----------------------------------+
```

### Statement of PACELC:
$$\mathbf{P} \text{ (Partition)} \implies \mathbf{A} \text{ vs } \mathbf{C} \quad \Big| \quad \mathbf{E} \text{ (Else)} \implies \mathbf{L} \text{ vs } \mathbf{C}$$

* If there is a **Partition** ($P$), how does the system trade off **Availability** ($A$) and **Consistency** ($C$)?
* **Else** ($E$), when the system is running normally without partitions, how does the system trade off **Latency** ($L$) and **Consistency** ($C$)?

---

### PACELC Classification Matrix:

| Database System | Partition Choice (P/A vs P/C) | Normal Choice (E/L vs E/C) | Full PACELC Classification |
| :--- | :--- | :--- | :--- |
| **MongoDB** | $P / C$ | $E / C$ | **PC/EC** |
| **HBase** | $P / C$ | $E / C$ | **PC/EC** |
| **Apache Cassandra** | $P / A$ | $E / L$ | **PA/EL** |
| **DynamoDB** | $P / A$ | $E / L$ | **PA/EL** |
| **MySQL (Async Master-Slave)**| $P / A$ | $E / L$ | **PA/EL** |

---

## 6. Conceptual Practice Exercises

1. Why can a single-node traditional RDBMS (like standalone MySQL) technically be classified as a "CA system", and why is "CA" a meaningless label in distributed networking?
2. Explain the PACELC trade-off of a database system configured with PA/EL (e.g., Cassandra with consistency level `ONE`).
3. Formulate a scenario where choosing AP over CP causes a double-booking anomaly in an airline reservation system.

---

## 7. Hard Placement & Interview Questions (FAANG Level)

### Q1: Prove the CAP Theorem mathematically using a 2-Node network partition proof intuition.

**Answer:**

```
                  NODE G1 <----- Network Severed (Partition) -----> NODE G2
             (Data v0)                                          (Data v0)
                 |                                                  |
       Client 1: Write(v1)                                  Client 2: Read()
                 |                                                  |
           Updates G1 to v1                                    Reads G2 => Returns v0!
```

* **Setup**:
  * Consider a distributed system with two nodes, $G_1$ and $G_2$, holding an initial data value $v_0$.
  * A network partition occurs, severing the communication link between $G_1$ and $G_2$.
* **Execution Sequence**:
  1. Client 1 sends a request `Write(v1)` to node $G_1$.
  2. Because $G_1$ cannot communicate with $G_2$ due to the partition, $G_1$ has two choices:
     * **Choice A (Choose Availability)**: $G_1$ updates its local state to $v_1$ and returns `Success` to Client 1.
       * Next step: Client 2 sends a `Read()` request to node $G_2$.
       * Since $G_2$ never received the update from $G_1$, $G_2$ returns stale value $v_0$.
       * *Result*: Linearizable Consistency is **VIOLATED** ($G_1 = v_1, G_2 = v_0$).
     * **Choice B (Choose Consistency)**: $G_1$ blocks or returns an `Error` to Client 1 because it cannot replicate to $G_2$.
       * *Result*: Availability is **VIOLATED** ($G_1$ failed to respond to a valid write).
* **Conclusion**: In the presence of a partition ($P$), a system CANNOT simultaneously guarantee Consistency ($C$) and Availability ($A$). Q.E.D.

---

### Q2: What is the relationship between the CAP Theorem and the FLP Impossibility Result (Fischer, Lynch, Paterson, 1985)?
**Answer:**
* **FLP Impossibility Result**: Proves that in an asynchronous network model, **no deterministic consensus protocol** can guarantee agreement (consensus) among distributed nodes if even a single process is permitted to crash unannounced.
* **Relationship to CAP**:
  * CAP is a practical extension of FLP.
  * While FLP establishes that deterministic consensus is impossible in purely asynchronous networks with crash failures, CAP demonstrates that distributed systems must explicitly compromise either strict linearizability (Consistency) or termination guarantees (Availability) when network partitions prevent message delivery within bounded time.

---

### Q3: In a financial wallet service operating across multi-region data centers, how would you design a hybrid data tier that satisfies PACELC constraints for both user balances and activity logs?
**Answer:**
* **Balance Ledger Subsystem (PC/EC)**:
  * Uses a **PC/EC** storage layer (e.g., CockroachDB or MongoDB with strict Majority Write Concerns).
  * *Reasoning*: Balance updates require absolute linearizable consistency ($C$). Under network partition, balance updates block or reject to prevent double-spending. When running normally, it incurs latency ($L$) to complete multi-region consensus commits.
* **Activity / Audit Log Subsystem (PA/EL)**:
  * Uses a **PA/EL** storage layer (e.g., Apache Cassandra or DynamoDB).
  * *Reasoning*: User login logs and analytics clicks prioritize high write throughput and zero downtime ($A$). Under partition, local nodes write logs immediately ($L$). Logs synchronize asynchronously once partition heals.
