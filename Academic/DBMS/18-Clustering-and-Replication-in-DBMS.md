# Lecture 18: Clustering and Replication in DBMS

---

## 1. Introduction: High Availability & System Scalability

Single-node database architectures suffer from a **Single Point of Failure (SPOF)**. If the database hardware fails, the application experiences complete downtime.

To achieve **High Availability (HA)**, disaster resilience, and scale read throughput, databases deploy **Clustering** and **Replication**.

---

## 2. Database Clustering Architectures

```
     SHARED-DISK CLUSTER                         SHARED-NOTHING CLUSTER
  +-------+       +-------+                   +-------+       +-------+
  | Node1 |       | Node2 |                   | Node1 |       | Node2 |
  +-------+       +-------+                   | [RAM] |       | [RAM] |
      \               /                       | [Disk]|       | [Disk]|
       v             v                        +-------+       +-------+
    +-------------------+                         ^               ^
    | Shared SAN / NAS  |                         | (Network Link)|
    | Disk Array        |                         +---------------+
    +-------------------+
```

### 2.1 Shared-Disk Clustering
* **Architecture**: Multiple database server compute nodes share a common physical storage pool (SAN/NAS storage array).
* **Pros**: Easy node addition; any node can access any data block on the shared disk array.
* **Cons**: Storage network hardware bandwidth becomes a severe bottleneck; requires complex distributed lock management (DLM).
* **Example**: Oracle RAC (Real Application Clusters).

### 2.2 Shared-Nothing Clustering
* **Architecture**: Every node has its own independent CPU, RAM, and dedicated physical disk storage. Nodes communicate solely by exchanging messages over a high-speed network.
* **Pros**: Near-infinite horizontal scalability; zero hardware single point of failure.
* **Cons**: Requires sophisticated data partitioning (sharding) and distributed query routing.
* **Examples**: Apache Cassandra, CockroachDB, MySQL NDB Cluster.

---

## 3. Database Replication Strategies

**Replication** is the process of copying data continuously across multiple distinct database servers (replicas).

```
                            REPLICATION TOPOLOGIES
                            /                    \
                           /                      \
                  Master-Slave Replication      Multi-Master / Active-Active
                  (Primary-Replica)             (All nodes handle writes)
```

---

### 3.1 Synchronous vs. Asynchronous Replication

```
SYNCHRONOUS REPLICATION:
Client ---- Write ----> Master ---- Write ----> Slave
                         ^                        |
                         |------ ACK -------------+
  Client <--- OK --------|

ASYNCHRONOUS REPLICATION:
Client ---- Write ----> Master -------------------------> Slave (Replication Lag)
  Client <--- OK --------|  (Returns OK immediately!)
```

#### Detailed Comparison:

| Feature | Synchronous Replication | Asynchronous Replication |
| :--- | :--- | :--- |
| **Commit Latency** | **High**: Client waits for Master AND Slave disk write acknowledgments. | **Low**: Master commits and returns OK immediately to client. |
| **Data Loss Risk (RPO)**| **Zero Data Loss**: Replica is guaranteed to be 100% up to date. | **Risk of Data Loss**: If Master crashes before logs replicate to slave. |
| **Master Node Availability**| If slave fails, writes on Master are **blocked**. | Master continues accepting writes even if slave goes offline. |
| **Network Dependency**| Requires low-latency, reliable interconnect. | Tolerates high-latency WAN / cross-region connections. |

---

### 3.2 Semi-Synchronous Replication (Hybrid)
* **Mechanism**: The Master node waits for **at least ONE** slave replica to acknowledge receiving and logging the write in its relay log before returning success to the client.
* **Balance**: Combines zero data loss guarantees of synchronous setups with acceptable write latencies.

---

## 4. Availability Metrics: RTO and RPO

When evaluating replication failover strategies, system architects measure two critical metrics:

```
+-------------------------------------------------------------------------------+
| RTO (Recovery Time Objective)  | Maximum acceptable duration of DOWNTIME.      |
| RPO (Recovery Point Objective) | Maximum acceptable amount of DATA LOSS (Time).|
+-------------------------------------------------------------------------------+
```

```
[ Last Valid Backup / Sync ] <----- RPO Loss Window -----> [ CRASH EVENT ] <----- RTO Downtime -----> [ System Restored ]
```

* **Example**: If RPO = 5 minutes and RTO = 1 hour:
  * In the event of a total site failure, the system must be fully online within 1 hour, and at most 5 minutes worth of recent writes may be lost.
  * For mission-critical banking systems: $\text{RPO} = 0, \text{RTO} < 30 \text{ seconds}$.

---

## 5. Conceptual Practice Exercises

1. Trace the propagation of a `COMMIT` command in a Master-Slave topology using Asynchronous vs. Semi-Synchronous replication.
2. What is Replication Lag, and how can an application handle a user reading their own profile immediately after updating it (Read-Your-Own-Writes Consistency)?
3. Compare Shared-Disk vs Shared-Nothing clustering for a multi-region deployment.

---

## 6. Hard Placement & Interview Questions (FAANG Level)

### Q1: What is the "Split-Brain" problem in high-availability database clusters during network partitions, and how does the Quorum Consensus algorithm ($Q_{read} + Q_{write} > N$) prevent it?

**Answer:**
* **Split-Brain Problem**:
  * Occurs when a network partition disconnects a cluster into two isolated sub-groups (e.g., Node 1 & 2 vs Node 3).
  * If both isolated sub-groups believe the other side is dead, both sub-groups may promote a local node to "Master" and accept conflicting write transactions concurrently.
  * When the network partition heals, the database states have diverged irreparably, corrupting data integrity.

* **Quorum Consensus Resolution**:
  * Require that any node promotion or write transaction acquire explicit approval from a strict **Quorum Majority** of nodes ($Q > N / 2$).
  * In a 5-node cluster, a valid quorum requires $\lceil (5+1)/2 \rceil = 3$ node votes.
  * The isolated sub-group with 2 nodes cannot form a majority (2 < 3), so it steps down into read-only mode. The 3-node sub-group forms the true quorum and safely promotes a leader.

---

### Q2: Explain the mechanics of Statement-Based Replication vs. Row-Based Replication vs. Mixed-Mode Replication in MySQL binlog. What are the non-deterministic write hazards of Statement-Based Replication?

**Answer:**

1. **Statement-Based Replication (SBR)**:
   * Replicates raw SQL statements (e.g., `UPDATE Users SET Status = 'Active' WHERE Age > 30`).
   * *Hazard (Non-Deterministic Functions)*: Queries containing `NOW()`, `RAND()`, `UUID()`, or non-deterministic order `LIMIT` queries generate completely different values on the master vs. slave replica, corrupting data consistency!

2. **Row-Based Replication (RBR)**:
   * Replicates exact row-level binary byte delta changes.
   * *Advantage*: Completely deterministic, 100% safe.
   * *Disadvantage*: High network bandwidth overhead for bulk updates (`UPDATE 1M rows` sends 1 million row delta events).

3. **Mixed-Mode Replication (MIXED)**:
   * Uses Statement-Based replication by default to save network bandwidth, but automatically switches to Row-Based replication whenever non-deterministic SQL statements are detected.

---

### Q3: How do database engines enforce "Read-Your-Own-Writes" consistency when reading from asynchronous read replicas?
**Answer:**
When an application routes reads to asynchronous read replicas, a user who posts a comment may refresh their page and fail to see it due to replication lag.

**Architectural Solutions**:
1. **User Pinning / Sticky Sessions**: Route all read queries for a specific user to the **Master node** for a fixed window (e.g., 5 seconds) after that user performs any write operation.
2. **Replication Monotonic Timestamp Tracking**:
   * The Master returns a transaction commit timestamp / Log Sequence Number (`LSN`) in the write HTTP response header.
   * When the client makes a read request, it passes this `LSN`.
   * The load balancer routes the read request ONLY to replica nodes whose current applied `LSN` $\ge$ the client's `LSN`. If no replica is caught up, the query waits or hits the Master.
