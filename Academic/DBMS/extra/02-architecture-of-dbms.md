# Lecture 2: Architecture of DBMS

## 1. Why an Architecture?

A DBMS needs to serve many different users at once — application developers, end users, and database administrators — each with a different view of "what the data looks like." The **three-schema architecture** (also called the ANSI/SPARC architecture) was proposed to separate *what data looks like to the user* from *how data is physically stored*, so that changes on one level don't ripple through the entire system.

## 2. The Three-Schema Architecture

### a. External Level (View Level)
- The **closest level to the users**.
- Describes the part of the database that a particular user group is interested in, hiding the rest.
- Multiple **external schemas** (views) can exist for the same underlying database — e.g., a billing clerk sees only patient billing data, while a doctor sees medical history.
- Implemented via **views** in SQL.

### b. Conceptual Level (Logical Level)
- Describes **what data is stored** in the database and the **relationships** among that data, for the community of users as a whole.
- It is a single, unified representation of the entire database — independent of any specific application.
- Includes entities, data types, relationships, constraints (but not physical storage details).
- Managed typically by the **DBA** during database design.

### c. Internal Level (Physical Level)
- Describes **how data is actually stored** on the storage medium: file organization, indexing structures, storage allocation, compression, and access paths.
- Closest to the physical hardware.
- Concerned with performance-oriented decisions like B+ Tree indexes, hashing, and block sizes.

```
   ┌─────────────────────────────┐
   │   External Level (Views)     │  <- Multiple user views
   ├─────────────────────────────┤
   │   Conceptual Level            │  <- Community-wide logical schema
   ├─────────────────────────────┤
   │   Internal Level (Physical)   │  <- Storage & access structures
   └─────────────────────────────┘
```

## 3. Data Independence

Data independence is the capacity to change the schema at one level without having to change the schema at the next higher level. This is the key motivation behind the three-schema architecture.

### a. Logical Data Independence
The ability to change the **conceptual schema** without affecting **external schemas** (application programs and user views).

*Example*: Adding a new column `email` to the `Students` table shouldn't break existing applications that don't use that column.

Logical data independence is **harder to achieve** because application programs are often heavily dependent on the logical structure of the data they access.

### b. Physical Data Independence
The ability to change the **internal schema** (storage structure, indexes, file organization) without affecting the **conceptual schema**.

*Example*: Switching a table's storage from a heap file to a B+ Tree indexed file, or moving data to a different disk, shouldn't require rewriting any queries or application logic.

Physical data independence is **easier to achieve** since most applications only interact with the conceptual/logical view, never directly with physical storage.

## 4. Role of the Database Administrator (DBA)

The DBA is responsible for the overall control and management of the database system. Key responsibilities include:

1. **Schema Definition** — creating the conceptual schema.
2. **Storage Structure & Access Method Definition** — defining the internal schema (indexes, file organization).
3. **Schema and Physical Organization Modification** — altering schemas as requirements evolve.
4. **Granting Authorization for Data Access** — controlling who can read/write which data.
5. **Routine Maintenance** — backups, performance tuning, monitoring disk usage, applying patches.
6. **Ensuring Data Integrity and Security** — enforcing constraints, setting up recovery mechanisms.

## 5. Mappings Between Levels

- **External/Conceptual Mapping**: Defines correspondence between a particular external view and the conceptual schema — allows the DBMS to transform requests on a view into requests against the conceptual schema.
- **Conceptual/Internal Mapping**: Defines correspondence between the conceptual schema and the internal (physical) schema — allows the DBMS to find the actual stored record corresponding to a conceptual record.

## Summary
The three-schema architecture (External → Conceptual → Internal) is the conceptual backbone that lets a DBMS separate *how users see data*, *what the data logically is*, and *how it's physically stored*. This separation delivers data independence — physical independence (easy) and logical independence (harder) — and is what allows databases to evolve (new indexes, new storage engines, new schema fields) without breaking every application built on top. The DBA is the human operator responsible for defining, maintaining, and securing all three levels.

## 6. Centralized vs. Client-Server vs. Distributed Architecture (Additional Context)

Beyond the three-schema (logical) architecture, DBMSs are also classified by their **physical/deployment architecture**:

1. **Centralized Architecture** — a single system houses the entire DBMS (database + application + user interface), typically accessed via dumb terminals. Simple but not scalable and a single point of failure.
2. **Client-Server Architecture** — the DBMS engine runs on a dedicated **server**; **clients** (application programs) send requests over a network and receive results. Can be further split into:
   - **2-Tier Architecture**: Client directly communicates with the DBMS server (application logic lives on the client).
   - **3-Tier Architecture**: An intermediate **application/business logic layer** sits between the client and the database server, improving scalability, security, and maintainability (most modern web apps use this).
3. **Distributed Architecture** — data is spread across multiple physical sites/servers (ties into Lectures 18–19 on clustering/replication/sharding).

## 7. Data Dictionary / System Catalog

The DBMS maintains a **data dictionary (system catalog)** — metadata about the database itself: table names, column names/types, constraints, indexes, user permissions, storage statistics. The query optimizer consults this catalog heavily when deciding execution plans.

---

## Placement Interview Questions — Lecture 2

**Q1. Explain the three-schema architecture with a diagram.**
External (view) level → what individual users see; Conceptual (logical) level → the community-wide unified schema; Internal (physical) level → how data is actually stored on disk. Each level is separated by a mapping, enabling data independence.

**Q2. What is data independence? Explain its two types.**
Data independence is the ability to change a schema at one level without affecting schemas at higher levels. **Logical data independence**: changing the conceptual schema without affecting external schemas/applications (harder to achieve). **Physical data independence**: changing the internal (storage) schema without affecting the conceptual schema (easier to achieve).

**Q3. Why is logical data independence harder to achieve than physical data independence?**
Because application programs are often written referencing the logical/conceptual structure directly (table/column names, relationships), so changes to that structure (e.g., splitting a table) can break existing queries/programs. Physical changes (like adding an index or changing file organization) don't affect the logical view at all.

**Q4. What is the role of a DBA?**
Schema definition, storage structure/access method definition, schema modification, granting authorization, routine maintenance (backup, tuning), and ensuring integrity/security.

**Q5. What is a view in the context of the three-schema architecture?**
A view corresponds to the external level — a virtual table representing a customized subset/transformation of the conceptual schema data, tailored to a specific user or application's needs.

**Q6. Differentiate between 2-tier and 3-tier architecture.**
2-tier: client talks directly to the database server. 3-tier: client talks to an application/business-logic server, which talks to the database server — adds a layer of abstraction, better scalability and security.

**Q7. What is a data dictionary / system catalog?**
Metadata storage within the DBMS describing table structures, constraints, indexes, and permissions — used internally by the query optimizer and enforced by the DBMS engine.

**Q8. Give a real-world example of physical data independence.**
Changing a table's underlying storage from a heap file to a B+ Tree index, or moving the database to different disk hardware, without needing to rewrite any SQL queries.

**Q9. Can a conceptual schema have multiple external schemas? Can an external schema span multiple conceptual entities?**
Yes to both — one conceptual schema can support many different external views (views), and a single external view can combine/join data drawn from multiple entities in the conceptual schema.
