# Lecture 18: Clustering and Replication in DBMS

As applications grow, a single database server eventually becomes a bottleneck or a single point of failure. Clustering and replication are the two core techniques used to build databases that scale and stay available.

## 1. Database Clustering

**Clustering** refers to connecting multiple database server instances (nodes) together so they work as a **single logical unit**, sharing the workload and/or storage.

### Why Cluster?
1. **High Availability**: If one node fails, other nodes in the cluster continue serving requests — no single point of failure.
2. **Load Balancing**: Incoming queries/connections are distributed across multiple nodes, improving overall throughput.
3. **Scalability**: Additional nodes can be added to the cluster to handle increased load.

### Types of Clustering
1. **Shared-Disk Clustering**: All nodes in the cluster access a **common, shared storage** system. Any node can process any request since they all see the same data. (e.g., Oracle RAC — Real Application Clusters.)
2. **Shared-Nothing Clustering**: Each node has its **own independent storage**; data is partitioned/distributed across nodes (often paired with sharding). No node shares memory or disk with another — communication happens strictly over the network. Generally scales better since there's no shared-resource bottleneck. (e.g., Cassandra, most modern distributed databases.)

## 2. Database Replication

**Replication** refers to maintaining **multiple copies (replicas)** of the same data across different nodes/servers, kept synchronized with each other, primarily for redundancy, fault tolerance, and read scalability.

### a. Master-Slave (Primary-Replica) Replication
- One node is designated the **master (primary)** — it handles all **write** operations.
- One or more nodes are **slaves (replicas)** — they receive copies of the data from the master (via replication logs) and typically handle **read** operations.
- **Benefits**: Offloads read traffic from the master, improving read scalability; provides a standby copy for failover.
- **Drawback**: Writes are still bottlenecked through a single master; if the master fails, a slave must be promoted (failover), which can involve some downtime or data loss depending on replication lag.

This pattern is expanded in detail in Lecture 22 (Master-Slave Architecture).

### b. Master-Master (Multi-Master) Replication
- Multiple nodes can **each accept writes**, and changes are propagated to all other master nodes.
- **Benefit**: No single write bottleneck; higher write availability.
- **Drawback**: Higher risk of **write conflicts** (two masters updating the same record differently at nearly the same time) — requires conflict resolution strategies (e.g., last-write-wins, vector clocks, application-level merge logic).

### c. Synchronous vs. Asynchronous Replication

1. **Synchronous Replication**: The master waits for **acknowledgment from replica(s)** that the data has been written, before confirming the transaction as committed to the client.
   - **Pro**: Strong consistency — a replica is always guaranteed up to date if the master's commit succeeded.
   - **Con**: Higher write latency (must wait on network round-trips to replicas); if a replica is slow/unreachable, it can block the write entirely.

2. **Asynchronous Replication**: The master commits the transaction and responds to the client **immediately**, without waiting for replicas to confirm — data is propagated to replicas in the background.
   - **Pro**: Lower write latency, better write throughput.
   - **Con**: **Replication lag** — replicas may briefly hold stale data; if the master crashes before replicating, recent writes could be lost.

3. **Semi-Synchronous Replication** (a middle ground): The master waits for acknowledgment from **at least one** replica (not all) before committing — balances durability and latency.

## 3. Clustering vs. Replication — How They Relate

Clustering and replication are complementary, not mutually exclusive:
- **Replication** is about *having multiple copies* of data for durability/read-scaling.
- **Clustering** is about *coordinating multiple server nodes* to act as one system, which often relies on replication internally (in shared-nothing clusters) to keep each node's data copy in sync.

Most production distributed database deployments combine both: a **cluster** of nodes, where data is **replicated** across a subset of those nodes for fault tolerance, and also **sharded/partitioned** across another subset for horizontal write scalability (Lecture 19 explores sharding/partitioning in depth).

## Summary
**Clustering** groups multiple database server nodes together (shared-disk or shared-nothing) to achieve high availability, load balancing, and scalability. **Replication** maintains synchronized copies of data across nodes — via master-slave (single write point, multiple read replicas) or master-master (multiple write points, conflict-prone) topologies — using either synchronous (consistent but slower) or asynchronous (fast but eventually consistent) propagation. Together, these techniques form the foundation for building databases that can survive node failures and serve traffic at a scale no single server could handle alone — themes that continue directly into the next two lectures on scaling patterns and the CAP theorem.

## 4. Replication Lag and Read-Your-Own-Writes Consistency (Additional Depth)

**Replication lag** is the time delay between a write occurring on the master and that write being visible on a replica. In asynchronous replication, this lag can cause a subtle but common bug: a user submits a write (e.g., posts a comment), then immediately refreshes and doesn't see it — because their read request happened to be routed to a replica that hadn't yet received the update. This is called a violation of **read-your-own-writes consistency**, and is commonly solved by:
- Routing a user's own reads to the master for a short period after they write.
- Using "sticky sessions" that pin a user to the same replica they last wrote through.
- Tracking a version/timestamp and ensuring the read replica is at least that recent before serving the request.

## 5. Quorum-Based Replication (Additional Concept)

Some distributed databases (e.g., Cassandra, DynamoDB) use **quorum-based** replication: a write is only considered successful once acknowledged by a minimum number `W` of replicas, and a read only trusted once a minimum number `R` of replicas agree. If `W + R > N` (total replicas), strong consistency can be guaranteed even without waiting for ALL replicas — a tunable middle ground between full synchronous and full asynchronous replication.

---

## Placement Interview Questions — Lecture 18

**Q1. What is the difference between clustering and replication?**
Clustering groups multiple database server nodes to work together as a single logical system (for availability, load balancing, scalability). Replication maintains multiple synchronized copies of the same data across nodes for redundancy and read scaling. Clustering often relies on replication internally.

**Q2. Differentiate between shared-disk and shared-nothing clustering.**
Shared-disk: all nodes access a common shared storage system. Shared-nothing: each node has its own independent storage, with data partitioned across nodes — generally scales better since there's no shared-resource bottleneck.

**Q3. Explain master-slave replication and its main benefit/drawback.**
One master node handles all writes; one or more slave nodes replicate data from the master and typically serve reads. Benefit: offloads read traffic, improves read scalability. Drawback: writes remain bottlenecked through the single master, and failover is needed if the master fails.

**Q4. What is the difference between master-slave and master-master replication?**
Master-slave: only the master accepts writes. Master-master: multiple nodes can accept writes, offering higher write availability but introducing the risk of write conflicts that need resolution.

**Q5. Differentiate between synchronous and asynchronous replication.**
Synchronous: the master waits for replica acknowledgment before confirming a write as committed — stronger consistency, higher latency. Asynchronous: the master commits and responds immediately, replicating in the background — lower latency, but replicas may briefly serve stale data (replication lag).

**Q6. What is replication lag, and what problems can it cause?**
The delay between a write on the master and its visibility on a replica. Can cause a user to not see their own recent write if routed to a lagging replica (read-your-own-writes violation), or generally serve stale data to readers.

**Q7. What is quorum-based replication, and how does it balance consistency and performance?**
Writes require acknowledgment from a minimum number (W) of replicas, and reads require agreement from a minimum number (R) of replicas; if W + R > total replicas (N), strong consistency can be guaranteed without waiting for every single replica — a tunable middle ground between full synchronous and full asynchronous replication.

**Q8. How would you resolve a write conflict in master-master replication?**
Common strategies include last-write-wins (based on timestamps), vector clocks (to detect and merge concurrent updates), or application-level custom merge logic tailored to the specific data being written.
