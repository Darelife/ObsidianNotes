# Lecture 19: Database Scaling Patterns | How to Perform Step-by-Step Scaling

As an application's data volume and traffic grow, a database needs a deliberate scaling strategy. This lecture walks through the progression from a single server to a fully distributed, sharded database.

## 1. Vertical Scaling (Scaling Up)

**Vertical scaling** means increasing the capacity of a **single** existing server — adding more CPU cores, more RAM, faster SSDs, etc.

### Pros
- **Simplicity**: No architectural changes needed; the application logic remains completely unchanged.
- No distributed-systems complexity (no network partitions, no data consistency issues across nodes).

### Cons
- **Hard physical/cost ceiling**: There's a maximum amount of CPU/RAM a single machine can have, and the cost grows non-linearly (very high-end hardware is disproportionately expensive).
- **Single point of failure**: If that one powerful server goes down, the entire system goes down.
- Eventually **insufficient** for very large-scale applications (billions of rows, huge query volume).

## 2. Horizontal Scaling (Scaling Out)

**Horizontal scaling** means adding **more servers/nodes** to distribute the load, rather than making one server bigger.

### Pros
- Practically **unlimited scaling** — just keep adding more (commodity) machines.
- **Better fault tolerance**: losing one node out of many has a much smaller overall impact.
- Generally more **cost-effective** at scale (many cheap machines vs. one extremely expensive one).

### Cons
- Significantly more **architectural complexity**: requires data partitioning strategies, distributed transaction handling, network overhead, and careful consistency management (see CAP theorem, Lecture 20).
- Application logic often needs to be aware of the distribution (e.g., routing queries to the correct shard).

## 3. The Step-by-Step Scaling Journey

A typical progression as an application's load grows:

### Step 1: Single Server
Everything (application + database) on one machine. Fine for small applications/prototypes.

### Step 2: Vertical Scaling
As load grows, upgrade the database server's hardware (more RAM/CPU/faster disks). Cheapest and simplest first move.

### Step 3: Read Replicas (Read Scaling via Replication)
When read traffic dominates (common in most applications — many more reads than writes), add **read replicas** (master-slave replication from Lecture 18). The application routes read queries to replicas and write queries to the master, distributing read load across multiple machines while keeping writes centralized.

### Step 4: Caching Layer
Introduce an in-memory cache (e.g., Redis, Memcached) in front of the database to absorb repeated read queries for frequently accessed data, reducing load on the database entirely for "hot" data.

### Step 5: Partitioning / Sharding (Write Scaling)
When even the master server can no longer handle write volume (or the total dataset no longer fits on a single machine), the data itself must be **split (partitioned)** across multiple independent database servers, each holding only a **subset** of the data.

## 4. Partitioning Strategies

### a. Horizontal Partitioning (Sharding)
Splits a table's **rows** across multiple database instances, based on a chosen **shard/partition key**.

- **Range-based Sharding**: Rows are split based on ranges of the shard key (e.g., users A–M on Shard 1, N–Z on Shard 2). Simple, but can lead to uneven load ("hotspots") if data isn't uniformly distributed across the range.
- **Hash-based Sharding**: A hash function is applied to the shard key to determine which shard a row belongs to. Generally distributes data more evenly, but makes range queries (e.g., "all users between A and M") inefficient since related data is scattered across shards.
- **Directory-based Sharding**: A separate lookup service/table maps each shard key to its specific shard, offering maximum flexibility (can rebalance shards without changing the hashing/range logic) at the cost of an extra lookup indirection and a potential single point of failure for the directory itself.

### b. Vertical Partitioning
Splits a table's **columns** across different database instances/tables, typically grouping columns by access pattern (e.g., frequently-accessed columns in one table, rarely-accessed large text/blob columns in another) — differs from sharding, which splits rows, not columns.

## 5. Challenges Introduced by Sharding

- **Cross-shard joins/queries** become expensive or require application-level aggregation, since related data may live on different physical servers.
- **Distributed transactions** across shards need special protocols (e.g., two-phase commit) to maintain atomicity.
- **Rebalancing** data when adding/removing shards can be operationally complex (consistent hashing helps minimize the amount of data that needs to move).
- **Choosing the right shard key** is critical and hard to change later — a poor choice can lead to unbalanced load ("hot shards").

## Summary
Scaling a database typically follows a progression: start with **vertical scaling** (bigger single server) for simplicity, add **read replicas** and a **caching layer** to handle read-heavy workloads, and finally turn to **horizontal partitioning (sharding)** — splitting data by row (horizontal) or by column (vertical) across multiple independent servers — once a single machine can no longer handle the write volume or total data size. Each step trades additional architectural complexity for additional scaling headroom, and sharding in particular introduces hard distributed-systems challenges around cross-shard queries, transactions, and rebalancing that the next lecture's CAP theorem helps formally reason about.

## 6. Consistent Hashing (Additional Depth for Hash-Based Sharding)

A refinement of plain hash-based sharding: instead of a simple `hash(key) % num_shards` mapping (which requires re-hashing almost ALL keys whenever a shard is added/removed), **consistent hashing** maps both shards and keys onto a conceptual hash ring, and each key belongs to the next shard clockwise on the ring. Adding/removing a shard only affects the keys immediately adjacent to it on the ring, minimizing the amount of data that must be redistributed — used heavily in distributed systems like Cassandra, DynamoDB.

## 7. Federation vs. Sharding (Clarifying Distinction)

- **Federation (Functional Partitioning)**: Splitting a database by **feature/domain** rather than by rows — e.g., one database for user data, another for product data, another for orders. Different from sharding, which splits ONE logical table's rows across multiple servers.
- **Sharding**: Splitting a single large table's rows across multiple servers based on a shard key.

Both are horizontal scaling strategies but address different bottlenecks — federation separates by feature, sharding separates by data volume within a feature.

---

## Placement Interview Questions — Lecture 19

**Q1. Differentiate between vertical and horizontal scaling.**
Vertical scaling (scaling up) increases a single server's capacity (more CPU/RAM/disk). Horizontal scaling (scaling out) adds more servers to distribute load. Vertical is simpler but has a hard ceiling; horizontal offers near-unlimited growth but adds distributed-systems complexity.

**Q2. What is sharding? Why is it needed?**
Sharding is horizontal partitioning — splitting a table's rows across multiple independent database servers based on a shard key. Needed when a single server can no longer handle the write volume or total data size, even after vertical scaling and read replicas.

**Q3. Compare range-based, hash-based, and directory-based sharding strategies.**
Range-based: split by ranges of the shard key — simple but can cause hotspots with uneven data distribution. Hash-based: split using a hash function — more even distribution but makes range queries inefficient. Directory-based: a lookup service maps keys to shards — flexible/rebalanceable but adds an indirection layer and potential single point of failure.

**Q4. What is consistent hashing, and what problem does it solve?**
A hashing technique that maps shards and keys onto a conceptual ring, so adding/removing a shard only requires redistributing the keys immediately adjacent to it — avoiding the massive re-hashing that plain modulo-based hash sharding would require when the number of shards changes.

**Q5. What is the difference between horizontal and vertical partitioning?**
Horizontal partitioning (sharding) splits a table's rows across servers. Vertical partitioning splits a table's columns across different tables/servers, typically grouping by access pattern.

**Q6. What challenges does sharding introduce?**
Expensive cross-shard joins/queries, the need for distributed transaction protocols (e.g., two-phase commit) for atomicity across shards, complex rebalancing when adding/removing shards, and the critical, hard-to-change choice of shard key.

**Q7. Describe the typical step-by-step database scaling journey as an application grows.**
Single server → vertical scaling (bigger hardware) → read replicas (offload reads) → caching layer (absorb repeated reads) → sharding/horizontal partitioning (scale writes and total data size).

**Q8. What is the difference between federation and sharding?**
Federation splits a database by feature/domain (e.g., separate DBs for users, products, orders). Sharding splits a single table's rows across multiple servers based on a shard key. Both are horizontal scaling strategies addressing different bottlenecks.

**Q9. Why is choosing the right shard key so critical, and hard to change later?**
A poorly chosen shard key can lead to unbalanced load ("hot shards") where some shards handle disproportionately more traffic/data than others; changing the shard key later typically requires a massive, complex data migration/re-sharding effort across the entire distributed system.
