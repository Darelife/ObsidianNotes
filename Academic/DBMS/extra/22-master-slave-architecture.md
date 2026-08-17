# Lecture 22: Last Lecture — Master-Slave Architecture

The final lecture of the series dives deeper into **Master-Slave (Primary-Replica) Architecture**, a pattern introduced briefly in Lecture 18, tying together replication, scaling, and availability concepts covered throughout the series.

## 1. Recap: What is Master-Slave Architecture?

In a master-slave setup:
- The **master (primary)** node handles all **write** operations (INSERT, UPDATE, DELETE).
- One or more **slave (replica/secondary)** nodes maintain synchronized copies of the master's data, and typically serve **read** operations.
- Changes made on the master are propagated to the slaves via a **replication stream** (often based on the master's write-ahead/transaction log, similar in spirit to the log-based recovery mechanisms from Lecture 14).

## 2. Write-to-Master, Read-from-Slave Pattern

This is the core operational pattern that makes master-slave setups useful for scaling:

```
                     WRITES
Application  ─────────────────►  Master
     │                              │
     │                        (replication)
     │                              │
     │                              ▼
     └──────────────────────►  Slave 1, Slave 2, Slave 3...
                READS
```

- The application's data-access layer (or a proxy/load balancer) is responsible for **routing** write queries exclusively to the master, and distributing read queries across the available slave replicas.
- This directly implements the "Step 3: Read Replicas" scaling pattern from Lecture 19, offloading read traffic from a single bottlenecked server.

### Benefits
1. **Read scalability**: Add more slave replicas to handle increasing read traffic, without touching the master.
2. **Reduced load on master**: The master is freed up to focus purely on handling writes efficiently.
3. **Built-in backup/redundancy**: Slaves serve as live, near-real-time backups of the data.
4. **Geographic distribution**: Slaves can be placed in different data centers/regions closer to users, reducing read latency for geographically distributed user bases.

### Challenges
1. **Replication lag**: If using asynchronous replication (Lecture 18), a slave might briefly serve **stale data** — a user's own write might not be immediately visible if their next read is routed to a lagging slave (a classic "read-your-own-writes" consistency problem).
2. **Single write bottleneck**: All writes must still go through one master — this pattern doesn't help with write scaling (that requires sharding, per Lecture 19).
3. **Master is a single point of failure for writes**: If the master goes down, no writes can be processed until a new master is established.

## 3. Failover Handling

**Failover** is the process of promoting a slave to become the new master when the original master fails, to restore write availability.

### Failover Process (typical steps)
1. **Failure Detection**: A monitoring system/health check detects the master is unreachable or unresponsive (e.g., via periodic heartbeat pings).
2. **Election / Selection of New Master**: Among the available slaves, one is selected to become the new master — often the slave with the **most up-to-date data** (least replication lag), to minimize data loss. This may be automatic (via a consensus protocol like Raft/Paxos in modern systems) or manual (DBA-triggered).
3. **Promotion**: The chosen slave is reconfigured to accept writes, becoming the new master.
4. **Reconfiguration of Remaining Slaves**: All other slaves are redirected to replicate from the newly promoted master instead of the old one.
5. **Reconfiguration of Application/Routing Layer**: The application (or a proxy/service discovery layer) is updated to send write traffic to the new master's address.

### Types of Failover
- **Manual Failover**: A DBA/operator manually performs the steps above — slower but more controlled, avoiding accidental "split-brain" scenarios.
- **Automatic Failover**: A dedicated orchestration tool (e.g., a consensus-based cluster manager) automatically detects failure and performs promotion — faster recovery, but riskier if misconfigured (e.g., false-positive failure detection triggering an unnecessary failover).

### The Split-Brain Problem
A dangerous scenario where, due to a network partition, **both** the old master (which isn't actually dead, just unreachable to the monitoring system) and a newly-promoted slave believe they are the master simultaneously, and both start accepting writes — leading to **conflicting, diverging data** that's very hard to reconcile afterward. Preventing split-brain typically requires a **quorum-based** consensus mechanism (a majority of nodes must agree before promoting a new master), directly connecting back to the CAP theorem's Consistency-vs-Availability trade-off during a network partition (Lecture 20).

## 4. Tying the Whole Series Together

This final lecture is a natural capstone because master-slave architecture is where nearly every concept from the series converges in a real system:
- **Transactions & ACID** (Lecture 11): The master must still guarantee ACID for each write before it's replicated.
- **Concurrency Control** (Lectures 12–13): Governs how concurrent writes on the master itself are serialized.
- **Recovery** (Lecture 14): The replication log mechanism closely parallels write-ahead logging used for crash recovery.
- **Indexing** (Lectures 15, 21): Each replica maintains its own indexes (B+ Trees) independently to serve reads efficiently.
- **Replication & Clustering** (Lecture 18): Master-slave is a specific, common instance of the replication patterns discussed there.
- **Scaling** (Lecture 19): Read replicas are the "read-scaling" step of the broader scaling journey.
- **CAP Theorem** (Lecture 20): Failover and split-brain avoidance are direct real-world manifestations of the Consistency-vs-Availability trade-off during partitions.

## Summary
Master-slave architecture routes all writes to a single master and distributes reads across multiple synchronized slave replicas, directly implementing the read-scaling pattern introduced earlier in the series while introducing new operational challenges: replication lag, a single write bottleneck, and the need for robust **failover handling** (failure detection, new-master election, promotion, and reconfiguration) to survive master failure — all while carefully avoiding the dangerous **split-brain** scenario through quorum-based consensus. As the closing lecture of the placement series, this topic serves as a practical synthesis of transactions, concurrency, recovery, indexing, replication, and the CAP theorem into one coherent, real-world distributed database pattern.

## 5. Comparison with Related Architectures

| Architecture | Write Scaling | Read Scaling | Complexity |
|---|---|---|---|
| Single Server | None | None | Lowest |
| Master-Slave | None (single write bottleneck) | Good (add more slaves) | Moderate |
| Master-Master | Moderate (multiple write points, conflict risk) | Good | High |
| Sharded (Master-Slave per shard) | Excellent | Excellent | Highest |

## 6. Common Interview Follow-Up: Combining Master-Slave with Sharding

In large-scale production systems, master-slave replication and sharding are often **combined**: the dataset is sharded across multiple independent clusters, and **each individual shard** internally runs its own master-slave replication setup for read scaling and fault tolerance within that shard — giving both write scalability (via sharding) and read scalability + durability (via per-shard replication).

---

## Placement Interview Questions — Lecture 22

**Q1. Explain the master-slave (write-to-master, read-from-slave) pattern.**
The master node handles all write operations; changes are replicated to one or more slave nodes, which primarily serve read operations, offloading read traffic and improving overall read scalability while the master focuses on writes.

**Q2. What are the main benefits of master-slave architecture?**
Read scalability (add more slave replicas), reduced load on the master, built-in backup/redundancy (slaves as live copies), and the ability to geographically distribute read replicas closer to users for lower latency.

**Q3. What are the main drawbacks of master-slave architecture?**
Replication lag can cause stale reads on slaves, all writes remain bottlenecked through a single master (no write scaling), and the master is a single point of failure for writes until failover completes.

**Q4. What is failover, and what are the typical steps involved?**
Failover promotes a slave to become the new master when the original master fails. Steps: failure detection (health checks/heartbeats), election/selection of the most up-to-date slave, promotion of that slave to master, reconfiguration of remaining slaves to replicate from the new master, and reconfiguration of the application/routing layer to send writes to the new master.

**Q5. Differentiate between manual and automatic failover.**
Manual: a DBA/operator performs the failover steps by hand — slower but more controlled. Automatic: an orchestration tool detects failure and performs promotion automatically — faster recovery, but riskier if failure detection produces false positives.

**Q6. What is the split-brain problem, and how is it prevented?**
A scenario where, due to a network partition, both the old (still-alive but unreachable) master and a newly-promoted slave believe they're the master simultaneously, both accepting writes and causing diverging, conflicting data. Prevented using quorum-based consensus mechanisms, requiring a majority of nodes to agree before promoting a new master.

**Q7. How does master-slave architecture relate to the CAP theorem?**
Failover and split-brain avoidance are direct real-world instances of the CAP theorem's Consistency-vs-Availability trade-off during a network partition — prioritizing consistency (waiting for quorum before promoting a new master, risking temporary unavailability) versus prioritizing availability (promoting quickly, risking split-brain and inconsistency).

**Q8. What is "read-your-own-writes" consistency, and how does it relate to master-slave replication?**
It's the expectation that a user should immediately see their own recent writes; in async master-slave replication, if a user's read is routed to a lagging slave, they might not see their own just-completed write — commonly solved by routing a user's own reads to the master briefly after they write, or using sticky sessions.

**Q9. Why doesn't master-slave architecture help with write scalability?**
Because ALL writes must still go through the single master node — adding more slaves only helps distribute read load, not write load; write scaling requires a fundamentally different approach like sharding (horizontal partitioning).

**Q10. How would you combine master-slave replication with sharding in a large-scale system?**
Shard the dataset across multiple independent clusters for write scalability, and within each individual shard, run a separate master-slave replication setup for read scalability and fault tolerance — combining both techniques to achieve scalability on both the read and write dimensions simultaneously.

**Q11. As a capstone question: walk through how transactions, concurrency control, recovery, indexing, and replication all interact within a single master-slave production database.**
The master processes each write as an ACID transaction (Lecture 11), using concurrency control protocols like Strict 2PL (Lecture 13) to safely interleave concurrent writes; each committed change is recorded via write-ahead logging, which both powers crash recovery (Lecture 14) AND serves as the basis for the replication stream sent to slaves (Lecture 18); each slave independently maintains its own B+ Tree indexes (Lecture 21) to serve read queries efficiently; and if the master fails, a quorum-based failover process (informed by CAP theorem trade-offs, Lecture 20) promotes a slave to keep the system operational.
