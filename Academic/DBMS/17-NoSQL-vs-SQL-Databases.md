# Lecture 17: NoSQL vs SQL Databases

---

## 1. Structural Comparison: SQL vs. NoSQL

The choice between Relational (SQL) and Non-Relational (NoSQL) databases is one of the most critical architectural decisions in software system design.

```
       SQL DATABASES                           NoSQL DATABASES
  +-----------------------+               +-----------------------+
  | Rigid Schema          |               | Flexible Schema       |
  | Structured Tables     |               | Unstructured / Semi   |
  | Vertical Scaling      |               | Horizontal Scaling    |
  | ACID Compliance       |               | BASE Guarantee        |
  | Powerful SQL Joins    |               | Denormalized / No Join|
  +-----------------------+               +-----------------------+
```

---

## 2. In-Depth Comparison Framework

### 2.1 Schema-on-Write vs. Schema-on-Read

* **Schema-on-Write (SQL)**: The database schema is formally defined using DDL before data insertion. The database engine validates data against strict column types, length constraints, and foreign keys *at write time*.
* **Schema-on-Read (NoSQL)**: Data is inserted without pre-defining a rigid schema structure. The application code interprets the document structure and handles missing/extra fields *at read time*.

---

### 2.2 ACID vs. BASE Paradigm

```
                   ACID (SQL)                    BASE (NoSQL)
        +----------------------------+  +----------------------------+
        | Atomicity                  |  | Basically Available        |
        | Consistency (Strict)       |  | Soft-state                 |
        | Isolation                  |  | Eventual consistency       |
        | Durability                 |  |                            |
        +----------------------------+  +----------------------------+
```

#### BASE Properties Defined:
1. **Basically Available**: System guarantees availability of data according to CAP theorem (will respond to requests), but responses may be stale or temporarily inconsistent across nodes.
2. **Soft-State**: The state of the database may change over time, even without explicit user write operations, due to background node synchronization and replication updates.
3. **Eventual Consistency**: The data will eventually reach a consistent state across all replica nodes if no new updates are made to the item over a time interval.

---

### 2.3 Detailed Comparison Taxonomy Matrix

| Feature / Dimension | Relational Databases (SQL) | NoSQL Databases |
| :--- | :--- | :--- |
| **Data Model** | Relational Tables (Tuples & Columns) | Key-Value, Document, Column-Family, Graph |
| **Schema Structure** | Rigid, predefined (Schema-on-Write) | Dynamic, flexible (Schema-on-Read) |
| **Scaling Architecture**| **Vertical** (Scale-Up: CPU/RAM) | **Horizontal** (Scale-Out: Sharded Nodes) |
| **Transaction Model** | Strict **ACID** guarantees | **BASE** (Eventual Consistency standard) |
| **Query Engine** | Standardized SQL with complex `JOIN`s | API commands / Mongo MQL / MapReduce / CQL |
| **Primary Bottleneck** | Disk I/O & write locks on single node | Cross-node network latency & consistency lag |
| **Best For** | Banking, Payments, Complex Relationships | Big Data analytics, High-velocity feeds, Caching |

---

## 3. Decision Framework: When to Choose SQL vs. NoSQL

```
                           DECISION TREE PIPELINE
                                     |
                Is strict ACID & Financial correctness required?
                                /         \
                             (YES)        (NO)
                             /               \
                    [ CHOOSE SQL ]        Is data structure highly dynamic
                                          or scaling beyond 100k writes/sec?
                                                   /          \
                                                (YES)         (NO)
                                                /                \
                                       [ CHOOSE NoSQL ]    [ CHOOSE SQL ]
```

---

## 4. Conceptual Practice Exercises

1. Explain a scenario where Eventual Consistency causes a transient anomaly in a social media like-counter, and why this is acceptable compared to a banking ledger.
2. How does horizontal sharding in NoSQL make cross-document multi-table `JOIN` operations practically impossible?
3. Compare the storage space overhead of storing 1,000,000 JSON documents (NoSQL) with repeated field key names vs. a normalized SQL table header.

---

## 5. Hard Placement & Interview Questions (FAANG Level)

### Q1: Modern RDBMS (like PostgreSQL and MySQL 8.0) support `JSONB` data types and document indexing, while modern NoSQL engines (like MongoDB 4.0+) support multi-document ACID transactions. Does this mean the distinction between SQL and NoSQL has completely dissolved? Explain the remaining fundamental architecture trade-offs.

**Answer:**
While convergence has blurred surface API capabilities, **fundamental underlying architecture trade-offs remain unchanged**:

1. **Storage Layout & Memory Architecture**:
   * RDBMS engines rely on contiguous tuple storage and B+ Tree primary page layouts tuned for single-node transaction integrity and buffer pool caching.
   * NoSQL engines (e.g., Cassandra LSM trees, MongoDB WiredTiger) rely on distributed sharding partition keys, memory tables (MemTables), and append-only SSTables designed for distributed scale-out.

2. **Distributed Transaction Latency & Consensus Cost**:
   * Multi-document ACID transactions in MongoDB require **Two-Phase Commit (2PC)** across distributed shard nodes, incurring severe network latency penalties and throughput drop-offs ($O(N^2)$ message complexity).
   * Relational SQL engines execute multi-table ACID transactions locally within a single node at sub-millisecond memory speed.

3. **Query Optimization Intelligence**:
   * SQL query optimizers (e.g., Postgres cost-based optimizer) rewrite multi-table joins, select optimal join algorithms (Hash Join, Nested Loop, Merge Join), and reorder execution paths dynamically based on data distribution statistics.
   * NoSQL engines lack complex cross-partition query cost optimizers, forcing developers to manually implement application-level denormalization.

---

### Q2: What is the "Tunable Consistency" model in Cassandra NoSQL, and how can a system architect enforce strong read-after-write consistency using the formula $R + W > N$?

**Answer:**
* **Tunable Consistency**: Apache Cassandra allows developers to configure the quorum of replica nodes that must acknowledge a read or write operation per query basis.
  * $N$: Replication Factor (Total number of node replicas storing copies of a data partition).
  * $W$: Write Consistency Level (Number of replica nodes that must acknowledge a write before returning success).
  * $R$: Read Consistency Level (Number of replica nodes that must respond to a read request before returning data).

* **Strong Consistency Guarantee Formula**:
  $$R + W > N$$
* **Proof**:
  * If $R + W > N$, the set of nodes written to ($W$) and the set of nodes read from ($R$) MUST overlap by at least **one common replica node** ($\text{Nodes}_W \cap \text{Nodes}_R \neq \emptyset$).
  * That overlapping node will contain the latest vector timestamp of the write, guaranteeing that the read query receives the latest updated value (Strong Consistency).
* **Examples**:
  * $N=3, W=\text{QUORUM } (2), R=\text{QUORUM } (2) \implies 2 + 2 = 4 > 3$ (**Strong Consistency**).
  * $N=3, W=1, R=1 \implies 1 + 1 = 2 \ngtr 3$ (**Eventual Consistency / Stale Reads Possible**).

---

### Q3: How do LSM Trees (Log-Structured Merge Trees) used in NoSQL engines provide $100\times$ faster write throughput than B+ Trees used in traditional SQL engines?

**Answer:**
* **B+ Trees (SQL Engine Writes)**:
  * Performs **Random Disk I/O**.
  * When a row is updated, the DBMS must locate the specific leaf page on disk, read it into memory, modify it, and write it back to disk. Random disk seeks cause hardware latency bottlenecks.
* **LSM Trees (NoSQL Engine Writes)**:
  * Performs **Sequential Append-Only I/O**.
  * All incoming `INSERT`, `UPDATE`, and `DELETE` operations are appended sequentially to an in-memory sorted buffer called a **MemTable** and written to a sequential commit log on disk.
  * When MemTable fills up, it is flushed to disk as an immutable sorted file called an **SSTable** (Sorted String Table).
  * Background compaction threads merge SSTables asynchronously.
* **Trade-off**: LSM Trees trade faster sequential writes for slower read point lookups (mitigated using Bloom Filters).
