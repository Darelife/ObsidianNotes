# Lecture 16: Types of Databases

---

## 1. Overview of Database Taxonomies

Databases are categorized based on their data organization models, storage representations, query interfaces, and scaling objectives.

```
                            DATABASE TYPES TAXONOMY
                            /          |          \
                           /           |           \
                 Relational (RDBMS)  NoSQL       Legacy / Specialized
                 (SQL / Tables)     (4 Types)    (Hierarchical, Network, OODBMS)
```

---

## 2. Comprehensive Classification Matrix

### 2.1 Relational Databases (RDBMS)
* **Model**: Tables (Relations), Tuples, Schema-on-write, SQL querying.
* **Core Characteristics**: Strict ACID compliance, foreign key relations, mathematical relation algebra.
* **Examples**: PostgreSQL, MySQL, Oracle DB, Microsoft SQL Server, SQLite.

---

### 2.2 Object-Oriented (OODBMS) & Object-Relational (ORDBMS)
* **OODBMS**: Blends database capabilities with Object-Oriented Programming (OOP) concepts (Objects, Classes, Polymorphism, Encapsulation, Inheritance).
  * *Examples*: db4o, ObjectStore.
* **ORDBMS**: Hybrid model extending standard RDBMS tables with user-defined composite types, array structures, and object methods.
  * *Examples*: PostgreSQL (supports `JSONB`, user-defined types, array columns).

---

### 2.3 NoSQL Databases (Not Only SQL)
NoSQL databases sacrifice strict ACID properties or relational joins in exchange for high write/read throughput, horizontal elasticity, dynamic schemas, and scale-out distributed performance.

#### The Four Major NoSQL Categories:

```
                            NoSQL DATABASE TYPES
                            /     |        |     \
                           /      |        |      \
               Document Store  Key-Value  Column  Graph DB
               (JSON/BSON)     (KV Hash)  (Wide)  (Nodes/Edges)
```

1. **Document Stores**:
   * Data stored as self-describing semi-structured documents (JSON, BSON, XML).
   * Primary keys index nested document trees.
   * *Examples*: MongoDB, CouchDB.

2. **Key-Value Stores**:
   * Simplest model: Associative array / Hash map mapping opaque unique Keys to arbitrary binary Values.
   * Optimized for $O(1)$ sub-millisecond point lookups and in-memory caching.
   * *Examples*: Redis, Memcached, DynamoDB.

3. **Wide-Column (Column-Family) Stores**:
   * Data stored in sparse multi-dimensional maps organized by column families rather than row blocks.
   * High performance for massive sequential write workloads and analytical aggregation over millions of rows.
   * *Examples*: Apache Cassandra, HBase, ScyllaDB.

4. **Graph Databases**:
   * Data stored as **Nodes** (Entities), **Edges** (Relationships), and **Properties** (Key-Value attributes attached to nodes/edges).
   * Operates index-free adjacency traversals without expensive relational SQL `JOIN`s.
   * *Examples*: Neo4j, Amazon Neptune, ArangoDB.

---

### 2.4 Legacy Database Models

#### 1. Hierarchical Database Model (Tree Structure)
* Data organized in an inverted **Tree** structure.
* Parent-Child relationships ($1:N$). A child node can have only **ONE Parent**.
* *Example*: IBM IMS (Information Management System).

#### 2. Network Database Model (Graph/Set Structure)
* Extended hierarchical model allowing a child record to have **MULTIPLE Parents** ($M:N$ relationships supported via Set types).
* *Example*: IDMS (Integrated Database Management System), CODASYL model.

---

## 3. Comparative Taxonomy Matrix

| Model | Primary Data Structure | Schema Flexibility | Query Language | Primary Use Case |
| :--- | :--- | :--- | :--- | :--- |
| **Relational (RDBMS)** | Tables (Rows/Columns) | Rigid (Schema-on-write) | SQL | Financial systems, ERP, transactional core |
| **Document Store** | JSON / BSON Documents | High (Schema-on-read) | MongoDB MQL, N1QL | Content management, e-commerce catalogs |
| **Key-Value Store** | Hash Table (Key-Value) | Schemaless | Key Lookups / Commands | Caching, session management, leaderboards |
| **Wide-Column Store** | Sparse Column Families | High | CQL | Time-series, IoT telemetry, high-write logs |
| **Graph Database** | Nodes, Edges, Properties | High | Cypher, Gremlin | Social graphs, fraud detection, recommendation |
| **Hierarchical** | Tree Nodes | Rigid | Low-level pointers | Legacy mainframe record management |

---

## 4. Conceptual Practice Exercises

1. Compare how a friendship relationship is stored in a Relational Database table vs. a Graph Database node edge.
2. Why are Key-Value stores like Redis exceptionally faster than RDBMS engines for counting active web session tokens?
3. What is the fundamental difference between Schema-on-Write and Schema-on-Read?

---

## 5. Hard Placement & Interview Questions (FAANG Level)

### Q1: In a system design interview for a global Logistics tracking platform (tracking package coordinates every 5 seconds across 10,000 delivery vehicles), which database type would you select and why? Evaluate RDBMS vs Column-Family Store vs Document Store.

**Answer:**
* **Workload Characteristics**: Massive write-heavy time-series updates (10,000 updates/sec), sequential append operations, high volume, low transaction complexity (no complex cross-table joins needed).
* **Evaluation**:
  1. **RDBMS (MySQL/PostgreSQL)**: Will suffer disk I/O bottlenecks. InnoDB B+ Tree page splits under high concurrent writes lead to write amplification and high lock contention.
  2. **Document Store (MongoDB)**: Good for package metadata, but appending millions of geographic location logs per day leads to storage fragmentation and un-optimized write paths.
  3. **Wide-Column Store (Apache Cassandra)**: **WINNER**. Uses Log-Structured Merge (LSM) Trees. Writes are sequential memory appends to MemTable and commit log, bypassing random disk I/O. Shards data seamlessly across cluster nodes using `(Vehicle_ID)` as partition key and `(Timestamp)` as clustering key.

---

### Q2: What is "Index-Free Adjacency" in Graph Databases (Neo4j), and why does it outperform SQL `JOIN`s for deep multi-hop graph traversals ($O(1)$ vs $O(N \log N)$)?
**Answer:**
* **Relational SQL Join Cost**: To traverse 4 hops (User $\rightarrow$ Friends $\rightarrow$ Liked Pages $\rightarrow$ Page Owners), an RDBMS executes 3 nested B+ Tree index lookups. For $N$ records, each hop incurs $O(\log N)$ lookup latency $\implies O(k \log N)$ cost per query step.
* **Index-Free Adjacency (Graph DB)**:
  * Each node entity directly maintains physical memory pointers to its adjacent neighbor nodes on disk.
  * Traversing an edge requires dereferencing a memory pointer ($O(1)$ constant time).
  * Traversal latency depends ONLY on the number of traversed sub-graph edges, completely independent of the total size of the global database ($O(k)$ where $k$ is the local degree).

---

### Q3: Explain why Object-Oriented Databases (OODBMS) failed to replace RDBMS in mainstream enterprise application development, despite the "Impedance Mismatch" problem.
**Answer:**
* **Impedance Mismatch**: The friction caused by converting object graphs in OOP languages (Java, C++) into flat relational tables in RDBMS.
* **Why OODBMS Failed**:
  1. **Lack of Standard Query Language**: RDBMS offered declarative SQL; OODBMS used language-specific procedural APIs, breaking data independence.
  2. **Weak Ad-hoc Query Capability**: RDBMS allows dynamic ad-hoc joins across any columns; OODBMS required pre-navigational class pointer paths.
  3. **Rise of ORMs and Hybrid Engines**: Frameworks (Hibernate, Entity Framework) and ORDBMS features (PostgreSQL `JSONB`) solved object mapping without losing SQL maturity, transaction isolation, and index ecosystem.
