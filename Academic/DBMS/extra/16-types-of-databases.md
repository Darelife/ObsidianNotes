# Lecture 16: Types of Databases

Databases can be categorized by their underlying **data model** — the fundamental way they structure, store, and let you query data. This lecture surveys the major categories.

## 1. Relational Databases (RDBMS)

Data organized into tables (relations) with rows and columns, related to each other via foreign keys, and queried using **SQL**. Strong schema enforcement, ACID transaction guarantees, and mature tooling.

- **Examples**: MySQL, PostgreSQL, Oracle Database, Microsoft SQL Server, SQLite.
- **Best for**: Structured data with clear relationships, applications requiring strong consistency (banking, e-commerce order processing, ERP systems).

## 2. Object-Oriented Databases (OODBMS)

Data is stored as **objects**, similar to how objects are represented in object-oriented programming languages (with attributes and methods bundled together), rather than as flat tables. Supports concepts like inheritance, encapsulation, and complex/nested data types natively, avoiding the "object-relational impedance mismatch" that occurs when mapping OOP objects to relational tables.

- **Examples**: db4o, ObjectDB, Versant.
- **Best for**: Applications with deeply complex, nested object hierarchies (CAD/CAM systems, multimedia applications) where the relational model's flat-table structure is awkward.

## 3. NoSQL Databases

An umbrella term for databases that depart from the rigid relational/tabular model, generally trading strict consistency and schema rigidity for horizontal scalability and flexibility (explored in depth in Lecture 17). Major sub-categories:

### a. Key-Value Stores
Simplest model — data stored as a collection of key-value pairs, similar to a giant hash map. Extremely fast lookups by key.
- **Examples**: Redis, DynamoDB, Riak.

### b. Document Databases
Data stored as semi-structured **documents** (typically JSON/BSON), where each document can have a different structure/schema. Good for hierarchical, nested data.
- **Examples**: MongoDB, CouchDB.

### c. Column-Family (Wide-Column) Databases
Data organized into column families rather than rows, optimized for queries over large datasets that access specific columns across many rows (analytics, time-series).
- **Examples**: Apache Cassandra, HBase.

### d. Graph Databases
Data represented as **nodes** (entities) and **edges** (relationships), optimized for traversing complex, highly interconnected relationships.
- **Examples**: Neo4j, Amazon Neptune.

## 4. Hierarchical Databases

One of the earliest database models. Data is organized in a **tree-like structure**, where each **child record has exactly one parent**, forming strict parent-child (1:N) relationships.

- **Navigation**: Access is via traversing the tree from the root down.
- **Limitation**: Cannot naturally represent many-to-many relationships without duplicating data, since each child can have only one parent.
- **Examples**: IBM's IMS (Information Management System), the Windows Registry.
- **Best for**: Naturally hierarchical data — file systems, organizational charts.

## 5. Network Databases

An evolution of the hierarchical model that relaxes the "one parent" restriction — a **child record can have multiple parent records**, represented as a **graph** rather than a strict tree, using explicit "set" relationships between record types.

- **Advantage over hierarchical**: Can natively represent many-to-many relationships.
- **Examples**: Integrated Data Store (IDS), IDMS.
- **Limitation**: Navigating the database requires following explicit pointers/paths programmatically — no declarative query language like SQL — making it complex to use and largely superseded by the relational model.

## 6. Comparative Summary Table

| Database Type | Structure | Schema Flexibility | Best Use Case |
|---|---|---|---|
| Relational | Tables (rows/cols) | Rigid | Structured transactional data |
| Object-Oriented | Objects | Flexible (per class) | Complex nested objects |
| Key-Value (NoSQL) | Key → Value pairs | Very flexible | Caching, session storage |
| Document (NoSQL) | JSON-like documents | Flexible | Content management, catalogs |
| Column-Family (NoSQL) | Column families | Flexible | Big data analytics, time-series |
| Graph (NoSQL) | Nodes + Edges | Flexible | Social networks, recommendation engines |
| Hierarchical | Tree (1 parent/child) | Rigid | File systems, org charts |
| Network | Graph (multi-parent) | Rigid | Legacy complex relationship systems |

## Summary
Database types differ fundamentally in how they model relationships between data: the **relational model** uses tables and foreign keys with strict schema and ACID guarantees; **object-oriented databases** mirror OOP object structures; the various **NoSQL** families (key-value, document, column-family, graph) each optimize for a specific access pattern at the cost of relational rigor; and the older **hierarchical** and **network** models represent data as trees or graphs of explicit parent-child pointers, largely historical predecessors to the relational model. Choosing the right type depends heavily on the shape of your data and your consistency/scalability requirements — a theme picked up directly in the next lecture comparing SQL and NoSQL.

## 7. Time-Series and Multi-Model Databases (Additional Categories)

- **Time-Series Databases**: Optimized specifically for data points indexed by time (sensor readings, stock prices, application metrics). Examples: InfluxDB, TimescaleDB. Optimize heavily for high-volume sequential writes and time-range queries.
- **Multi-Model Databases**: Support more than one data model (e.g., both document and graph) within a single, unified backend. Examples: ArangoDB, Couchbase (in some configurations).

## 8. Relational vs. Non-Relational — The Underlying Philosophy

The relational model prioritizes **data integrity and consistency through a rigid, mathematically-grounded structure**, trading off some flexibility and horizontal scalability. NoSQL models prioritize **flexibility, scale, and availability**, generally trading off some of the strict consistency and relational query power that SQL databases guarantee. Neither is universally "better" — the right choice always depends on the specific access patterns, consistency requirements, and scale of the application (explored further in Lecture 17).

---

## Placement Interview Questions — Lecture 16

**Q1. What are the main categories of database types?**
Relational, Object-Oriented, NoSQL (key-value, document, column-family, graph), Hierarchical, and Network databases.

**Q2. Differentiate between hierarchical and network database models.**
Hierarchical: strict tree structure where each child has exactly one parent (1:N relationships only). Network: relaxes this — a child can have multiple parents, forming a graph rather than a strict tree, allowing native many-to-many relationships.

**Q3. What is the main limitation of the hierarchical database model?**
It cannot naturally represent many-to-many relationships without data duplication, since each child record can have only one parent.

**Q4. Explain the four major types of NoSQL databases with examples.**
Key-Value (Redis, DynamoDB) — simplest, hash-map-like. Document (MongoDB, CouchDB) — semi-structured JSON documents. Column-Family (Cassandra, HBase) — optimized for wide-column analytical queries. Graph (Neo4j, Neptune) — nodes and edges for highly connected data.

**Q5. What is an object-oriented database, and what problem does it solve?**
A database storing data as objects (with attributes and methods bundled together), similar to OOP languages — solves the "object-relational impedance mismatch" that occurs when mapping complex OOP objects onto flat relational tables.

**Q6. Which database type would you choose for a social network's "friends of friends" recommendation feature, and why?**
A graph database (e.g., Neo4j) — because it's optimized for traversing highly interconnected relationships efficiently, unlike relational databases where deep relationship traversal requires expensive, repeated joins.

**Q7. Why were hierarchical and network models largely replaced by the relational model?**
Because navigating hierarchical/network databases required explicit, hand-coded traversal of pointers/paths in application code, with no declarative query language — the relational model's SQL offered a much simpler, more flexible, and more maintainable way to query and relate data.

**Q8. What is a time-series database, and when would you use one?**
A database optimized for data indexed by time (e.g., sensor readings, metrics), optimized for high-volume sequential writes and time-range queries — used in IoT, monitoring/observability, and financial tick-data systems.
