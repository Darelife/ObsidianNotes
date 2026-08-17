# Lecture 22: Master-Slave Architecture, Failover & Placement Preparation Wrap-Up

---

## 1. Advanced Master-Slave Topology Architecture

In enterprise database architectures, **Master-Slave (Primary-Replica)** deployment separates Read and Write query pathways to maximize throughput, guarantee high availability, and isolate analytical reporting workloads from transactional operations.

```
                                  APPLICATION LAYER
                                 /                 \
                     (Write Requests)            (Read Requests)
                           /                         \
                          v                           v
              +-----------------------+   +-----------------------+
              |     MASTER NODE       |   |  READ LOAD BALANCER   |
              |   (Primary / Writes)  |   +-----------------------+
              +-----------------------+         /           \
                          |                    v             v
             Replication Stream (WAL/Binlog) +--------+   +--------+
                          |                  | Slave1 |   | Slave2 |
                          +----------------->| (Read) |   | (Read) |
                                             +--------+   +--------+
```

---

## 2. Dynamic Failover & Leader Election Mechanics

When the Master node crashes or becomes unreachable due to hardware failure, the system must perform an automatic **Failover** to promote a Slave replica to become the new Primary Master node without causing split-brain or data loss.

```
                     FAILOVER EXECUTION SEQUENCE
+-----------------------------------------------------------------------+
| STEP 1: Heartbeat Failure Detection (Health Checks / Timeout)         |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
| STEP 2: Consensus Quorum Leader Election (Raft / Paxos / ZooKeeper)   |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
| STEP 3: Promote Most Up-to-Date Slave (Highest LSN / WAL position)    |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
| STEP 4: Client Connection Re-Routing (VIP Rotation / DNS / Vitess)    |
+-----------------------------------------------------------------------+
```

---

### Key Components of Automated Failover:

1. **Heartbeat Monitoring & Health Checks**: Nodes exchange periodic heartbeat ping messages (e.g., every 1000 ms). If a Master fails to respond for $N$ consecutive heartbeat cycles, it is declared dead.
2. **Consensus Leader Election**: Consensus algorithms (Raft, Paxos, Zookeeper, Orchestrator) run a voting round to select the candidate slave with the **highest LSN (Log Sequence Number)** to prevent rolling back committed writes.
3. **VIP Rotation & DNS Switch**:
   * **Virtual IP (VIP) Takeover**: The newly promoted master node claims the cluster's virtual IP address using ARP packets.
   * **Connection Router Proxy**: Proxy servers (e.g., ProxySQL, HAProxy) update internal routing tables, directing incoming SQL `WRITE` traffic to the newly promoted Master instance.

---

## 3. High Availability Replication Topology Configurations

```
1. Active-Passive (Hot Standby)   2. Active-Active (Multi-Master)
   - 1 Master accepts Writes         - Multiple nodes accept Writes
   - 1 Standby replicates, no reads  - Requires conflict resolution (CRDTs)
```

---

## 4. Master-Slave Architecture Failure Modes & Mitigations

| Failure Mode | Root Cause | Architectural Mitigation |
| :--- | :--- | :--- |
| **Split-Brain Anomaly** | Network partition isolates Master node. | Quorum Consensus majority voting ($Q > N/2$). |
| **Replication Lag Stale Reads**| Heavy write workload stalls asynchronous replication. | Read-your-own-writes session pinning or GTID tracking. |
| **Cascading Slave Failures** | Master crashes; thousands of slaves attempt to re-connect simultaneously. | Intermediate Cascade Replication (Master $\rightarrow$ Relay Slave $\rightarrow$ Leaf Slaves). |

---

## 5. Comprehensive DBMS Placement Preparation Checklist

Congratulations on completing the 22-Lecture DBMS Series! Below is your quick-reference revision map for top-tier placement interviews.

```
+-------------------------------------------------------------------------------+
|                       DBMS PLACEMENT PREPARATION MAP                          |
+-------------------------------------------------------------------------------+
| [ ] L1-L2:  DBMS vs File System, 3-Schema Architecture, Data Independence      |
| [ ] L3-L6:  ER Modeling, Cardinalities, Extended ER, Facebook Case Study       |
| [ ] L7-L8:  Relational Model, Keys (PK/FK/CK/SK), ER-to-Relational Rules        |
| [ ] L9-L10: Normalization (1NF, 2NF, 3NF, BCNF), FDs, Lossless & Preservation |
| [ ] L11-L14: Transactions, ACID, 2PL, Timestamps, Deadlocks, ARIES Recovery   |
| [ ] L15,L21: Indexing, B-Trees, B+ Trees, Order Calculation, Page Layouts     |
| [ ] L16-L17: NoSQL vs SQL, CAP Theorem, BASE, Wide-Column, Graph Databases     |
| [ ] L18-L20: Clustering, Replication, Sharding, Consistent Hashing, PACELC    |
| [ ] L22:    Master-Slave Failover, Consensus Leader Election, Consensus       |
+-------------------------------------------------------------------------------+
```

---

## 6. Conceptual Practice Exercises

1. Trace the sequence of ARP packet broadcasts during a Virtual IP (VIP) failover event in a database cluster.
2. Explain why promoting a slave node that lags behind the dead master by 5 seconds requires manual data reconciliation or GTID point-in-time recovery.
3. Compare Paxos vs Raft consensus algorithms for database leader election.

---

## 7. Hard Placement & Interview Questions (FAANG Level)

### Q1: In an enterprise Master-Slave setup using MySQL Global Transaction Identifiers (GTID) and semi-synchronous replication, what happens when a Master crashes midway through executing a transaction commit? How does a proxy like Orchestrator ensure Zero Data Loss (RPO=0) during failover?

**Answer:**
1. **GTID Mechanics**: Each transaction is assigned a globally unique identifier `GTID = UUID:Transaction_ID` preserved across all nodes.
2. **Commit Phase Crash**:
   * If the Master node crashes after committing locally, but *before* sending GTID event to semi-synchronous slaves:
     * Slaves have NOT acknowledged the transaction. The client receives a network timeout / connection error.
   * If the Master crashes *after* at least one slave received and written the GTID event to its Relay Log:
     * The GTID event exists on the slave node.
3. **Orchestrator Failover Action (RPO=0)**:
   * Orchestrator inspects all available slave nodes and identifies the candidate node with the highest executed GTID set (`Executed_Gtid_Set`).
   * It applies remaining un-applied relay logs on the candidate slave to bring it to state parity.
   * Candidate slave is promoted to Master.
   * Errant transactions on old master (if it later wakes up) are errant GTID instances rolled back using binary log utilities before re-joining as a slave.

---

### Q2: What is "Cascading Replication" (Relay Replication), and why is it necessary when scaling read replicas from 5 to 500 nodes?
**Answer:**
* **Problem with Single Master to 500 Slaves**:
  * In a direct 1-to-500 master-slave topology, the Master node must transmit binary log network packets 500 times over the network for every single write transaction.
  * The network bandwidth overhead of sending replication streams saturates the Master's network card, drastically degrading write throughput.
* **Cascading Replication Solution**:
  ```
  [ MASTER ] ------ Replicates to ------> [ RELAY SLAVE 1 ] & [ RELAY SLAVE 2 ]
                                                |
                                        Replicates to
                                                v
                                   [ 250 LEAF SLAVE NODES ]
  ```
  * Master only transmits binary logs to 2 intermediate **Relay Slaves**.
  * Relay slaves fan out the binary log streams to the remaining 500 leaf read replicas.
  * Reduces network packet transmission overhead on Master node by $99.6\%$.

---

### Q3: Complete placement interview scenario: Design the database storage tier for a global Fintech Payment Gateway handling $50,000$ transactions per second (TPS) across multi-region data centers. Define the complete tech stack, indexing choices, partitioning, replication, and disaster recovery strategy.

**Answer:**

#### 1. Database Engine & Paradigm Choice:
* **Core Ledger / Balances**: Distributed SQL / NewSQL (e.g., CockroachDB / YugabyteDB) or Sharded PostgreSQL with Vitess. Enforces strict ACID compliance, multi-region Raft consensus, and PC/EC PACELC guarantees.
* **Transaction History / Audit Logs**: Cassandra (Wide-Column) or DynamoDB (PA/EL) for append-only $O(1)$ write scaling.
* **Caching & Idempotency Layer**: Redis Cluster for $O(1)$ sub-millisecond payment idempotency key validation.

#### 2. Indexing Strategy:
* **Primary Key**: UUIDv7 (Time-ordered 128-bit UUIDs) to prevent B+ Tree page split fragmentation caused by random UUIDv4 keys.
* **Secondary Indexes**: Composite Index on `(User_ID, Transaction_Date DESC)` for fast user statement pagination queries.

#### 3. Sharding & Partitioning:
* **Sharding Key**: Hash-based sharding on `User_ID` using **Consistent Hashing** with virtual nodes to evenly distribute throughput.
* **Hotspot Mitigation**: Salting accounts belonging to high-volume merchant entities (e.g., Amazon account) into $N$ sub-buckets.

#### 4. Replication & High Availability (HA):
* Multi-Region 5-Node Raft Consensus deployment across 3 AWS Availability Zones.
* Semi-Synchronous replication with `QUORUM` write concerns.
* **RTO $< 10 \text{ seconds}$** (automatic Raft leader re-election); **RPO $= 0$** (zero data loss).

#### 5. Disaster Recovery (DR):
* Continuous point-in-time write-ahead log (WAL) streaming to encrypted multi-region S3 buckets.
* Daily automated snapshot restores verified via automated staging environment pipelines.
