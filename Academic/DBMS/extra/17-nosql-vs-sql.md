# Lecture 17: NoSQL vs SQL Databases

## 1. SQL (Relational) Databases — Recap

SQL databases store data in structured **tables** with predefined **schemas**, related via foreign keys, and queried using Structured Query Language. They emphasize **ACID compliance** (Lecture 11) — strong consistency and transactional reliability.

*Examples*: MySQL, PostgreSQL, Oracle, SQL Server.

## 2. NoSQL Databases — Recap

"NoSQL" (originally "non-SQL," now often read as "not only SQL") databases depart from the rigid relational model to support flexible, semi-structured, or unstructured data, generally optimizing for horizontal scalability and high write/read throughput over strict consistency. Sub-types (key-value, document, column-family, graph) were covered in Lecture 16.

*Examples*: MongoDB, Cassandra, Redis, Neo4j.

## 3. Core Structural Differences

| Aspect | SQL (Relational) | NoSQL |
|---|---|---|
| **Schema** | Fixed, predefined schema — every row must conform | Dynamic/flexible schema — documents/records can vary in structure |
| **Data Model** | Tables with rows and columns | Key-value, document, column-family, or graph |
| **Relationships** | Modeled via foreign keys and JOINs | Typically denormalized/embedded; fewer/no joins |
| **Query Language** | SQL (standardized, declarative) | Varies by database (MongoDB Query Language, CQL, Cypher, etc.) — no single standard |
| **Scaling** | Primarily **vertical** (bigger server: more CPU/RAM) | Primarily **horizontal** (more servers, sharding) |
| **Transactions** | Strong ACID guarantees | Often relaxed to **BASE** (Basically Available, Soft state, Eventually consistent), though many modern NoSQL DBs now offer some ACID support too |

## 4. Consistency Models: ACID vs. BASE

### ACID (SQL)
As covered in Lecture 11: Atomicity, Consistency, Isolation, Durability — strong guarantees, prioritizing correctness over availability during network partitions.

### BASE (typical NoSQL default)
- **Basically Available**: The system guarantees availability, even in the presence of failures (may return slightly stale data rather than an error).
- **Soft State**: The state of the system may change over time, even without new input, as data propagates/replicates across nodes.
- **Eventually Consistent**: Given enough time without new updates, all replicas will **eventually** converge to the same value — but at any given moment, different nodes might return different (temporarily inconsistent) results.

This distinction is directly related to the **CAP theorem** (covered in Lecture 20) — NoSQL systems commonly prioritize Availability and Partition tolerance over strict Consistency.

## 5. When to Use SQL

- Data is highly structured with clear, stable relationships (e.g., financial transactions, inventory management, HR systems).
- Strong consistency and complex multi-table queries/joins are essential.
- The application requires complex transactions spanning multiple tables (e.g., banking).
- Data volume, while large, fits comfortably on scaled-up hardware or standard relational clustering solutions.

## 6. When to Use NoSQL

- Data is unstructured, semi-structured, or its schema evolves rapidly (e.g., user-generated content, IoT sensor data, product catalogs with varying attributes).
- The application needs to scale horizontally across many commodity servers to handle massive read/write volume (e.g., large-scale social media, real-time analytics).
- High availability is more critical than strict, immediate consistency (e.g., shopping cart services, session caches).
- Data is naturally graph-like (social networks, recommendation engines) — favors graph databases specifically.

## 7. Hybrid Reality

Modern systems often use **polyglot persistence** — combining multiple database types within a single application architecture, each chosen for the workload it serves best. For example, an e-commerce platform might use:
- A relational database for order processing and payments (needs ACID).
- A document database for the product catalog (flexible schema).
- A key-value store (Redis) for session/cache data (speed).
- A graph database for the "customers who bought this also bought" recommendation engine.

## Summary
SQL databases offer rigid schemas, strong ACID transactional guarantees, and a standardized query language, making them ideal for structured, relationship-heavy, consistency-critical applications. NoSQL databases trade strict consistency for schema flexibility and horizontal scalability, following the looser BASE model, making them well-suited for large-scale, rapidly-evolving, or highly available systems. The choice isn't binary — modern architectures frequently combine both (polyglot persistence) to match each part of a system to the database type best suited for its specific access pattern.

## 8. Normalization vs. Denormalization Philosophy

SQL databases generally favor **normalized** schemas (Lectures 9–10) to minimize redundancy, relying on JOINs to reconstruct related data at query time. NoSQL databases (especially document stores) often favor **denormalized**, embedded data structures — duplicating related data directly within a document to avoid needing joins entirely, trading storage space and write-complexity for faster, simpler reads.

## 9. Scaling Philosophy Recap (Bridge to Lecture 19)

SQL databases traditionally scale **vertically** (bigger server) more easily than horizontally, due to the complexity of maintaining ACID transactions and JOIN operations across distributed nodes. NoSQL databases are generally designed from the ground up for **horizontal scaling** (sharding across many commodity servers), which is a major reason they became popular for internet-scale applications.

---

## Placement Interview Questions — Lecture 17

**Q1. What are the key differences between SQL and NoSQL databases?**
Schema (fixed vs. flexible), data model (tables vs. key-value/document/column-family/graph), relationships (foreign keys + joins vs. denormalized/embedded), query language (standardized SQL vs. varies), scaling (vertical vs. horizontal), and transaction guarantees (ACID vs. typically BASE).

**Q2. Explain the BASE model and how it differs from ACID.**
BASE = Basically Available, Soft state, Eventually consistent — prioritizes availability and allows temporary inconsistency that resolves over time. ACID prioritizes strict consistency and durability guarantees at all times, even if it means reduced availability during failures.

**Q3. When would you choose a NoSQL database over a relational database?**
When data is unstructured/semi-structured or its schema evolves rapidly, when you need massive horizontal scalability for very high read/write volume, when high availability matters more than immediate consistency, or when data is naturally graph-like.

**Q4. When would you choose a relational database over NoSQL?**
When data is highly structured with stable, well-defined relationships, when strong consistency and complex multi-table transactions are essential (e.g., banking), or when you need robust, standardized querying (SQL) with mature tooling.

**Q5. What is polyglot persistence?**
Using multiple different database types within a single application architecture, each chosen to best serve a specific part of the system's workload — e.g., relational DB for orders, document DB for product catalog, Redis for caching, graph DB for recommendations.

**Q6. Why do NoSQL databases often favor denormalization over normalization?**
Because NoSQL systems are typically optimized for fast, simple reads at scale, and avoiding joins (by embedding related data directly within a document/record) achieves this — at the cost of extra storage and more complex write-time consistency maintenance.

**Q7. Is it true that NoSQL databases never support ACID transactions? Explain.**
No — this is a common misconception. While many NoSQL databases were originally designed BASE-first, several modern NoSQL databases (e.g., MongoDB with multi-document transactions, certain Cassandra configurations) now offer varying degrees of ACID support, though often more limited or with performance trade-offs compared to traditional RDBMSs.

**Q8. How does eventual consistency work in practice?**
When a write occurs, it's propagated to all replicas asynchronously; different replicas might briefly return different (stale) values, but given enough time without new writes, all replicas converge to the same final value.
