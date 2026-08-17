# Lecture 19: Database Scaling Patterns

---

## 1. Vertical vs. Horizontal Scaling (Scale-Up vs. Scale-Out)

As enterprise database traffic and data storage requirements grow beyond the capacity of a single server, systems must scale.

```
       VERTICAL SCALING (SCALE-UP)           HORIZONTAL SCALING (SCALE-OUT)
          +-------------------+             +-------+  +-------+  +-------+
          |      BIG NODE     |             | Node1 |  | Node2 |  | Node3 |
          | 128 Cores, 1TB RAM|             +-------+  +-------+  +-------+
          +-------------------+             (Add commodity instances dynamically)
```

### Comparative Analysis:

| Scale Factor | Vertical Scaling (Scale-Up) | Horizontal Scaling (Scale-Out) |
| :--- | :--- | :--- |
| **Strategy** | Add CPU cores, RAM, NVMe SSDs to existing server | Add more commodity database instances to cluster |
| **Hardware Limit** | Hard physical limit (Max RAM/CPU per motherboard) | Virtually unlimited theoretical scale |
| **Downtime / Complexity**| Simple; zero application code changes required | High complexity; requires partitioning & sharding |
| **Cost Curve** | Exponential cost curve (High-end enterprise hardware) | Linear cost curve (Standard cloud infrastructure) |

---

## 2. Partitioning Strategies: Vertical vs. Horizontal Partitioning

```
                             PARTITIONING TYPES
                             /                \
                            /                  \
             Vertical Partitioning         Horizontal Partitioning (Sharding)
             (Splitting Columns)            (Splitting Rows across Nodes)
```

---

### 2.1 Vertical Partitioning (Column Splitting)
* **Definition**: Splitting a wide table into smaller tables containing subsets of columns based on access frequency.
* **Example**: Splitting `Users(ID, Name, Email, PasswordHash, Bio, AvatarBlob)`:
  * Table 1 (Hot Data): `User_Auth(ID, Name, Email, PasswordHash)` $\rightarrow$ Kept in fast RAM buffer pool.
  * Table 2 (Cold Data): `User_Profile(ID, Bio, AvatarBlob)` $\rightarrow$ Stored on cheaper disk storage.

---

### 2.2 Horizontal Partitioning & Sharding (Row Splitting)
* **Definition**: Splitting a table by dividing rows across multiple independent physical database instances (Shards).

```
TABLE "ORDERS" (10 Million Rows)
  +-------------------------------------------------------------+
  | Shard Node 1 (US East)   | Rows with Order_ID 1 to 3,333,333|
  | Shard Node 2 (US West)   | Rows with Order_ID 3,333,334 to 6.6M
  | Shard Node 3 (EU Central)| Rows with Order_ID 6,666,667 to 10M
  +-------------------------------------------------------------+
```

---

## 3. Sharding Strategies

```
                            SHARDING STRATEGIES
                            /        |        \
                           /         |         \
                    Range-Based    Hash-Based  List/Directory-Based
```

### 3.1 Range-Based Sharding
* Partitioning rows based on contiguous ranges of a key value (e.g., `Date` or `User_ID 1-10000`).
* **Pros**: Simple; enables efficient range queries within a single shard (`WHERE Date BETWEEN '2026-01-01' AND '2026-01-31'`).
* **Cons**: Prone to **Hotspotting** (e.g., all new user registrations hit the highest range node concurrently).

---

### 3.2 Hash-Based Sharding
* Partitioning rows by applying a hash function to the **Sharding Key**:
  $$\text{Target Shard} = \text{Hash}(Key) \pmod N$$
* **Pros**: Evenly distributes write and read traffic across all cluster nodes, preventing hotspots.
* **Cons**: Range queries become extremely expensive, requiring **Scatter-Gather** scans across every single node in the cluster.

---

### 3.3 Consistent Hashing (Dynamic Resharding)

Traditional modulo hashing ($\text{Hash}(K) \pmod N$) has a fatal flaw: changing cluster size from $N$ to $N+1$ nodes invalidates the location of nearly **100% of existing keys**, requiring a total cluster data migration.

**Consistent Hashing** maps both node IP addresses and data keys to a $360^\circ$ circular Hash Ring ($0 \dots 2^{32}-1$).

```
                    CONSISTENT HASHING RING (2^32 - 1)
                               Node A [0]
                              /          \
                       Key 1 *            * Node B [90]
                            /              \
                    Node D [270] --------- Node C [180]
```

* **Key Assignment Rule**: A key is stored on the first node encountered when moving clockwise around the ring.
* **Adding a Node ($N+1$)**: Adding Node $E$ requires remapping only $\frac{1}{N+1}$ of the total keys. $90\%$ of keys remain completely undisturbed.
* **Virtual Nodes (VNodes)**: Physical nodes are mapped to multiple virtual positions on the ring to prevent uneven data distribution buckets.

---

## 4. Drawbacks & Complexities of Sharding

1. **Cross-Shard Joins**: Joining tables located on different physical shard nodes requires fetching full dataset rows over the network, rendering multi-table SQL joins prohibitively slow.
2. **Cross-Shard Transactions**: Enforcing ACID transactions across multiple shard nodes requires complex Distributed Commit Protocols (Two-Phase Commit - 2PC), which incur severe network latency penalties.
3. **Scatter-Gather Queries**: Queries that do not include the explicit Sharding Key must be broadcast to every single shard node in parallel, returning aggregated results.

---

## 5. Conceptual Practice Exercises

1. Select an optimal Sharding Key for a global Ride-Sharing application (Uber) tracking active rides. Evaluate `Ride_ID` vs. `Rider_ID` vs. `Geohash_Location`.
2. Compute the percentage of keys that must be re-located when expanding a hash cluster from 9 to 10 nodes under traditional Modulo Hashing vs. Consistent Hashing.
3. What is a Denormalization Strategy used to eliminate cross-shard joins?

---

## 6. Hard Placement & Interview Questions (FAANG Level)

### Q1: In a distributed system design interview, how do you handle the "Celebrity / Hotspot Problem" in hash-sharded social media tables (e.g., Elon Musk having 180M followers generating massive concurrent comment writes on a single post)?

**Answer:**
* **Problem**: Even with Hash-Based Sharding, all comments targeting a post authored by a celebrity share the same `Post_ID` as the sharding key. Thus, 100% of write traffic for that post targets a **single shard node**, crashing the instance.

* **Architectural Mitigations**:
  1. **Compound Salted Sharding Key**:
     * Transform the sharding key from `Post_ID` to `(Post_ID, Salt)` where `Salt = Random(1, N)`.
     * Distributes comments across $N$ virtual shard buckets.
     * *Read Trade-off*: Reading comments for the post requires a scatter-gather query across the $N$ salted shard buckets, which are then aggregated by application gateways.
  2. **In-Memory Write Aggregation Buffer**:
     * Route high-velocity post writes to a distributed message queue (Kafka / Redis Stream).
     * Background worker threads batch-aggregate writes before performing bulk updates on the shard node.

---

### Q2: Explain the Two-Phase Commit (2PC) protocol used for Distributed Transactions across sharded nodes, and detail why 2PC is a blocking protocol vulnerable to coordinator failures.

**Answer:**

```
COORDINATOR                        PARTICIPANT SHARDS
    |                                      |
    |---- Phase 1: PREPARE --------------->|
    |<--- VOTE_COMMIT / VOTE_ABORT --------|
    |                                      |
    |---- Phase 2: GLOBAL_COMMIT --------->|
    |<--- ACK -----------------------------|
```

* **Protocol Phases**:
  1. **Phase 1 (Prepare / Voting Phase)**: Coordinator sends `PREPARE` request to all participant shards. Shards acquire local locks, write undo/redo logs, and vote `VOTE_COMMIT` or `VOTE_ABORT`.
  2. **Phase 2 (Commit / Execution Phase)**: If ALL shards vote `VOTE_COMMIT`, coordinator writes commit record to its log and sends `GLOBAL_COMMIT`. Shards execute write and release locks.

* **Why 2PC is a Blocking Protocol (Coordinator Failure Vulnerability)**:
  * If the Coordinator node crashes *after* participants vote `VOTE_COMMIT` but *before* broadcasting `GLOBAL_COMMIT`, participant shards are left in an **indeterminate blocked state**.
  * Participants hold Exclusive ($X$) locks on local data items indefinitely, blocking all other incoming system transactions until the coordinator recovers.
  * Modern distributed databases use non-blocking consensus algorithms (Raft or Paxos) to elect a new leader and resolve blocked state.

---

### Q3: What is Resharding (Re-sharding), and how do systems execute zero-downtime resharding on live production systems storing Petabytes of data?
**Answer:**
* **Zero-Downtime Resharding Execution Pipeline**:
  1. **Dual-Writing**: Modify the application routing layer to write new/updated data to **BOTH** old shard cluster and new shard cluster concurrently.
  2. **Backfill Historical Data**: Background ETL jobs copy historical data from old shards to new shards for records created before dual-writing began.
  3. **Data Verification & Reconciliation**: Run continuous checksum comparison jobs (`MD5`/`SHA256` hashing on row batches) to verify 100% parity between old and new shard nodes.
  4. **Switch Read Traffic**: Shift read traffic gradually from old shards to new shards (Canary deployment: 1% $\rightarrow$ 10% $\rightarrow$ 100%).
  5. **Disable Dual-Writing**: Stop writing to old shards and decommission legacy nodes.
