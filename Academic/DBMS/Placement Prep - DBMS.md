# DBMS — Placement Prep

Consolidated from: [[01-Introduction-to-DBMS]] through [[22-Master-Slave-Architecture-and-Failover]], plus standard SQL interview-query coverage.

## Table of Contents
0. [DBMS Fundamentals & File System Limitations](#0-dbms-fundamentals--file-system-limitations)
1. [Architecture: 3-Schema Model, Data Independence, DBA](#1-architecture-3-schema-model-data-independence-dba)
2. [ER Model: Entities, Attributes, Relationships](#2-er-model-entities-attributes-relationships)
3. [Extended ER (EER): Generalization, Aggregation, Categorization](#3-extended-er-eer-generalization-aggregation-categorization)
4. [ER Design Methodology, Traps & Modeling Patterns](#4-er-design-methodology-traps--modeling-patterns)
5. [The Relational Model & Keys](#5-the-relational-model--keys)
6. [ER-to-Relational Mapping](#6-er-to-relational-mapping)
7. [SQL Essentials & Query Practice](#7-sql-essentials--query-practice)
8. [Functional Dependencies & Normalization](#8-functional-dependencies--normalization)
9. [Transactions & ACID](#9-transactions--acid)
10. [Concurrency Control: Anomalies & Serializability](#10-concurrency-control-anomalies--serializability)
11. [Concurrency Control Protocols: Locking & Timestamps](#11-concurrency-control-protocols-locking--timestamps)
12. [Deadlocks & Crash Recovery (ARIES)](#12-deadlocks--crash-recovery-aries)
13. [Indexing](#13-indexing)
14. [B-Trees & B+ Trees](#14-b-trees--b-trees)
15. [Types of Databases & NoSQL](#15-types-of-databases--nosql)
16. [SQL vs NoSQL, CAP & PACELC](#16-sql-vs-nosql-cap--pacelc)
17. [Clustering & Replication](#17-clustering--replication)
18. [Database Scaling: Partitioning & Sharding](#18-database-scaling-partitioning--sharding)
19. [Master-Slave, Failover & Distributed Transactions](#19-master-slave-failover--distributed-transactions)
20. [Quick-Fire Interview Q&A](#20-quick-fire-interview-qa)

---

## 0. DBMS Fundamentals & File System Limitations

**Bytes → Data → Information**: raw bytes (uninterpreted bits) → data (decoded, typed, but contextless: `25`, `98.5`) → information (data + context + semantics: "John scored 98.5% in DBMS"). $\text{Information} = \text{Data} + \text{Context} + \text{Semantics}$.

**Why file-processing systems (flat files, one file per app) fail at scale**:
1. **Redundancy & inconsistency** — same fact (e.g. address) duplicated across files; updating one copy leaves others stale.
2. **Querying inflexibility** — no SQL; ad-hoc questions need custom line-by-line parsing code.
3. **Data isolation** — different files/formats (CSV, binary, XML) make cross-file joins a manual nightmare.
4. **Integrity problems** — constraints (`Balance ≥ 0`) hardcoded per-application; a new app can bypass them.
5. **Atomicity problems** — a multi-step update (debit A, credit B) can crash mid-way with no rollback; money vanishes.
6. **Concurrent access anomalies** — OS file locking (`flock`) is whole-file granularity, causing lost updates or severe contention.
7. **Security** — OS enforces only file-level R/W/X permissions, not row/column-level access control.

**DBMS solves all of the above** via: centralized data dictionary/catalog, declarative query language, declarative integrity constraints (`CHECK`, `FK`, `UNIQUE`), fine-grained concurrency control (row/page locks, MVCC), log-based atomic recovery (WAL/ARIES), and fine-grained RBAC (`GRANT`/`REVOKE`).

**Controlled redundancy is intentional, not eliminated**: foreign keys, secondary indexes (duplicate the indexed column next to a pointer), denormalized read replicas (avoid expensive distributed joins), and full replication (HA/DR) all trade storage + update complexity for read speed and fault tolerance.

**Why DBMS engines build their own Buffer Pool Manager instead of relying on the OS page cache**: (1) WAL ordering — log records must hit disk *before* the dirty page they describe, a dependency the generic OS cache doesn't understand; (2) page-eviction intelligence — DBMS knows access patterns (sequential scan vs point lookup) and uses Clock-Sweep/LRU-K/2Q instead of naive LRU to avoid scan-induced thrashing; (3) explicit `fsync()` control tied to transaction commit, vs the OS's async timer-driven flushing.

---

## 1. Architecture: 3-Schema Model, Data Independence, DBA

**ANSI-SPARC Three-Schema Architecture** — separates *what users see* from *what's stored*:

| Level | Describes | Example |
|---|---|---|
| **External** (View) | Per-user/app customized views; hides irrelevant/sensitive columns; can include derived fields | Student sees `Roll_No, Name, CGPA`; Finance sees `Roll_No, Fee_Paid` |
| **Conceptual** (Logical) | What data exists + relationships + constraints, hardware-independent, defined via DDL | `CREATE TABLE Students(...)` |
| **Internal** (Physical) | How data is physically stored: file organization, block size, page layout, indexes, compression | InnoDB B+Tree, 16KB pages |

**Data Independence** — changing one level without touching the level above it:
- **Logical Data Independence**: modify the Conceptual schema (e.g. split `Employees` into `Emp_Personal` + `Emp_Work`) without breaking External views/app code — achieved via `CREATE VIEW` reconstructing the old shape. **Harder** to achieve (business-logic ripple effects).
- **Physical Data Independence**: change the Internal schema (switch heap scan → B+Tree index, move HDD → NVMe) without touching the Conceptual schema or any SQL query. **Easier/standard** DBMS feature.
- Sharding a table across nodes is a **Internal/Physical** schema concern — a distributed query router (Vitess/Citus/CockroachDB) hides it behind a unified conceptual schema, so resharding 16→32 nodes needs zero app code changes.
- Dropping a column a view depends on **invalidates the view**; the DBA restores logical independence by `CREATE OR REPLACE VIEW` to recompute/coalesce the missing column from its replacement(s).
- **Data Dictionary / System Catalog**: stores schema, types, privileges, physical paths, index metadata — the backbone that lets external queries resolve down to internal storage blocks, and lets the optimizer type-check and choose access paths.

**DBA responsibilities**: schema definition/migration, security & access authorization (`GRANT`/`REVOKE`, row/column policies), performance tuning (`EXPLAIN`, index create/drop, buffer pool sizing), backup/PITR/DR/failover orchestration, capacity planning, integrity/audit compliance (GDPR/HIPAA/PCI-DSS).

---

## 2. ER Model: Entities, Attributes, Relationships

**Entity** = a distinct real-world thing; **Entity Set** = a homogeneous collection of entities (all `Student` rows).

**Strong vs Weak Entities**:

| | Strong | Weak |
|---|---|---|
| Key | Has a Primary Key | No PK; only a **Partial Key** (discriminator) |
| Existence | Independent | Existence-dependent on an **Owner** entity |
| Notation | Single rectangle | Double rectangle, linked via a double-diamond **Identifying Relationship** |
| Composite PK | Own key | Owner's PK + Partial Key |

**Attribute types**: Simple/atomic (`SSN`) · Composite (`Address → Street,City,Zip`) · Single- vs Multi-valued (double ellipse, `Phone_Numbers`) · Derived (dashed ellipse, `Age` from `DOB`) · Key (underlined).

**Degree of a relationship** = number of participating entity sets: unary/recursive (1, e.g. `Manages`), binary (2, most common), ternary (3, e.g. `Supplier–Part–Project`), N-ary.

**Structural constraints**:
- **Cardinality ratio**: 1:1, 1:N, N:1, M:N.
- **Participation**: **Total** (double line — every entity instance *must* participate, e.g. every `LOAN` has a `CUSTOMER`) vs **Partial** (single line — may not participate).
- **(min,max) notation**: pair attached to each entity's line into the relationship. `min=0`→partial, `min≥1`→total; `max`=upper bound (or N). E.g. `EMPLOYEE(1,1) ==Works_For== DEPARTMENT(0,N)`.

**Chen notation cheat sheet**: Rectangle=strong entity · Double rectangle=weak entity · Diamond=relationship · Double diamond=identifying relationship · Ellipse=attribute · Underlined ellipse=key · Dashed-underline=partial key · Double ellipse=multivalued · Dashed ellipse=derived · Double line=total participation.

**M:N relationship vs Weak Entity** — both look "dependent" but differ fundamentally: in an M:N binary relationship both entities can exist **independently** with zero relationship rows; a weak entity **cannot exist at all** without its owner, and its discriminator is only unique *within* one owner's scope.

**Ternary → Binary decomposition is lossy in general.** $R(A,B,C)$ can only be split into $R_1(A,B), R_2(B,C)$ without loss if a functional dependency ($B\to A$ or $B\to C$) exists. Otherwise, joining the binary pieces back can produce **spurious tuples** — the three pairwise facts don't prove the original triple co-occurred (classic doctor/patient/medicine counterexample).

---

## 3. Extended ER (EER): Generalization, Aggregation, Categorization

**Specialization** (top-down: superclass → subclasses by distinguishing attributes) vs **Generalization** (bottom-up: synthesize a superclass from common attributes of existing entity types). Same hierarchy, opposite design direction.

**Two orthogonal constraint axes**:

| Axis | Options | Meaning |
|---|---|---|
| **Disjointness** | Disjoint (`d`) | subclasses mutually exclusive — entity in ≤1 subclass |
| | Overlapping (`o`) | entity can belong to ≥2 subclasses simultaneously |
| **Completeness** | Total (`==`) | every superclass entity must belong to some subclass |
| | Partial (`--`) | superclass entity may belong to none |

Combined: `(d,total)`=exactly one subclass (Account→Savings/Checking) · `(d,partial)`=at most one · `(o,total)`=at least one, possibly many (Patient→Inpatient/Outpatient) · `(o,partial)`=zero, one, or many.

**Attribute inheritance**: subclass inherits all superclass attributes + relationships, PK is inherited from superclass, plus its own local attributes.

**Translating specialization to relational tables** — 3 strategies:

| Strategy | Schema | NULLs | JOIN cost | FK integrity |
|---|---|---|---|---|
| Single table + discriminator | One wide table, `Type_Flag` column | High (non-applicable subclass columns) | None (fastest) | Weak — can't `NOT NULL` subclass columns |
| Table per subclass (normalized) | `Super(PK,shared)` + `Sub1(PK FK, attrs)` | Zero | Needs JOIN | Best — clean FK/NOT NULL |
| Table per concrete class | No superclass table; each `Sub(PK, shared, own)` | Zero | Fast per-subclass, needs `UNION ALL` for superclass-wide queries | Poor — no single place to enforce global PK uniqueness |

**Categorization (Union Type)**: a subclass inheriting from **multiple, unrelated** superclasses with different key structures (e.g. `OWNER` = `PERSON ∪ BANK ∪ COMPANY`, each with its own PK type) — distinct from Generalization, where all subclasses share one superclass/key space.

**Aggregation**: lets a **relationship set** (+ its participants) be treated as a single higher-level entity that itself participates in another relationship — needed because plain ER forbids relationships-of-relationships. Example: `(EMPLOYEE WORKS_ON PROJECT)` aggregated, then `REQUIRES EQUIPMENT`. Relationally this must be modeled as an explicit composite-key table (`Assignment(Emp_ID, Project_ID)`) with the dependent table's FK referencing that *composite* key — skipping this and using two independent FKs on the dependent table allows invalid (Emp, Project) combos to sneak in.

---

## 4. ER Design Methodology, Traps & Modeling Patterns

**5-step formulation pipeline**: (1) requirements/domain analysis → (2) noun/verb parsing (nouns→entities/attributes, verbs→relationships) → (3) keys/cardinality/participation → (4) refine with EER (generalization/aggregation) → (5) anomaly/edge-case validation (fan/chasm traps, insert/delete behavior).

**Noun-parsing heuristic**: independent attributes of its own → Entity; single scalar property → Attribute; multiple values per instance → Multivalued Attribute or junction entity.

**Fan Trap**: two 1:N relationships fan out from the same central entity, making it ambiguous which instance of the "far" entity actually relates to which. Fix: insert the correct intermediate entity in the chain (`DIVISION→DEPARTMENT→EMPLOYEE`, not `DIVISION→EMPLOYEE` and `EMPLOYEE→DEPARTMENT` in parallel).

**Chasm Trap**: a relationship path implied by the model doesn't actually exist for some instances, due to partial participation along an optional intermediate link. Fix: add a direct relationship between the outer entities instead of relying on the optional path.

**Attribute vs Entity decision tree** (e.g. is `Car_Engine` an attribute, weak entity, or strong entity?): atomic value + no own relationships → Attribute; has sub-parts but tied wholly to one parent, never referenced elsewhere → Composite Attribute; has its own attributes but no global PK and can't survive parent deletion → Weak Entity; can be manufactured/inventoried/sold/transferred independently → Strong Entity.

**Temporal/historical tracking pattern**: convert a point-in-time attribute into a `JOB_HISTORY`-style weak entity keyed on `(Owner_PK, Start_Date)`, total participation from owner; use `End_Date IS NULL` (or an `Is_Current` flag) to find the current row.

**Modeling patterns from real systems** (Facebook case study):
- **Polymorphic associations** (reactions on both Posts and Comments) — avoid sparse-FK anti-pattern (`Post_ID NULL`, `Comment_ID NULL` + `CHECK`). Prefer either separate concrete tables (`Post_Reactions`, `Comment_Reactions` — clean FKs, SQL-native) or a superclass "interactable object" table both Post and Comment reference (uniform schema, good for notification fan-out).
- **Symmetric self-referencing M:N** (friendship graph) — avoid storing both `(A,B)` and `(B,A)`: enforce **canonical ordering** `CHECK(User_ID_1 < User_ID_2)`, insert with `MIN/MAX`, query with `LEAST/GREATEST`.
- **Nested/threaded comments** — three representations:

| Model | Insert cost | Subtree read | Reparent cost |
|---|---|---|---|
| Adjacency List (`Parent_ID`) | O(1) | Expensive recursive CTE | Cheap |
| Path Enumeration (`"1/4/12"`) | O(1) | Fast prefix `LIKE '1/4/%'` | Must rewrite all descendants |
| Closure Table (`Ancestor,Descendant,Depth`) | Multi-row write | Fastest, cleanest | Localized |

---

## 5. The Relational Model & Keys

Proposed by E.F. Codd (1970). Table = **Relation**, Row = **Tuple**, Column = **Attribute**, Column count = **Degree**, Row count = **Cardinality**. $r(R) \subseteq D_1 \times D_2 \times \dots \times D_n$.

**Six properties of a valid relation**: tuples unordered, attributes unordered, all values atomic (1NF assumption), all tuples distinct (set semantics — no duplicate rows), `NULL` = unknown/missing/inapplicable, fixed attribute domains.

**Key hierarchy** (nested, broadest → narrowest): **Super Key** (any attribute set that uniquely IDs a tuple) ⊇ **Candidate Key** (a *minimal* super key — no proper subset is still a super key) ⊇ {**Primary Key** (the CK chosen, must be `NOT NULL`+unique), **Alternate Key** (unchosen CKs)}. **Foreign Key** references another relation's PK/CK to enforce inter-table integrity.

- If every attribute of $R(A_1,\dots,A_n)$ is independently a candidate key, total super keys $= 2^n - 1$ (every non-empty subset contains a CK).
- Union of two super keys is **always** a super key (adding attributes preserves uniqueness). Intersection of two super keys is **not** guaranteed to be one (counterexample: $CK_1=\{A,B\}$, $CK_2=\{B,C\}$, intersection $\{B\}$ alone doesn't determine $R$).

**Integrity constraints**:
| Constraint | Rule |
|---|---|
| **Domain** | Value must belong to the attribute's declared domain/type/`CHECK` range |
| **Entity** | No attribute of a **Primary Key** may be `NULL` (an unidentified entity breaks tuple identity) |
| **Referential** | Every non-null FK value in $R_1$ must match some existing PK value in $R_2$ |

**`ON DELETE` / `ON UPDATE` actions**: `RESTRICT`/`NO ACTION` (default, reject if children exist) · `CASCADE` (delete/update children too) · `SET NULL` · `SET DEFAULT`.

---

## 6. ER-to-Relational Mapping

| ER Element | Relational Translation | Primary Key |
|---|---|---|
| Strong Entity | Standalone table | Entity's key attribute |
| Weak Entity | Table with Owner-PK as FK | Owner PK + Partial Key (composite) |
| 1:1 Relationship | FK placed on the **totally-participating** side, `UNIQUE` constraint | Natural table PK |
| 1:N Relationship | FK on the **N-side** table (referencing 1-side PK) | N-side's natural PK |
| M:N Relationship | New **junction table** holding both PKs + relationship attrs | Composite $(PK_A, PK_B)$ |
| Multivalued Attribute | New table: `(Parent_PK, Value)`, `ON DELETE CASCADE` | Composite of both columns |
| Composite Attribute | Flattened into individual scalar columns — **no** new table | N/A |

1:1 with total participation on **both** sides → merge into a single table.

**Minimum table count**: given $E$ strong entities, $W$ weak entities, $B_{M:N}$ binary M:N relationships, $M$ multivalued attributes → $\text{tables} = E + W + B_{M:N} + M$ (1:1 and 1:N cost zero extra tables — they fold into existing ones).

**Anti-pattern — folding 1:N into an array column** (`Department(Dept_ID, Employee_IDs[])`): violates 1NF (non-atomic value), can't B+Tree-index individual array elements efficiently, can't enforce `FK ON DELETE CASCADE` per element, and joins need `UNNEST()` (full scan instead of index join).

**Recursive 1:N (self-referencing FK)**: `Employees(Emp_ID PK, Manager_ID FK→Emp_ID)`. `ON DELETE SET NULL` orphans direct reports; `ON DELETE CASCADE` recursively deletes the entire reporting subtree; `ON DELETE RESTRICT` blocks the delete while any report still points to the manager.

---

## 7. SQL Essentials & Query Practice

### Command Categories
| Category | Commands |
|---|---|
| **DDL** | `CREATE`, `ALTER`, `DROP`, `TRUNCATE` (schema-level; often auto-commits) |
| **DML** | `SELECT`, `INSERT`, `UPDATE`, `DELETE` |
| **DCL** | `GRANT`, `REVOKE` |
| **TCL** | `COMMIT`, `ROLLBACK`, `SAVEPOINT` |

`DELETE` (DML, row-by-row, logged, can `WHERE`+rollback) vs `TRUNCATE` (DDL, deallocates pages, resets auto-increment, minimal logging, can't easily filter) vs `DROP` (removes the table object entirely).

### Logical Query Execution Order (not written order!)
```
FROM → JOIN → WHERE → GROUP BY → HAVING → SELECT → DISTINCT → ORDER BY → LIMIT/OFFSET
```
This is *why* a column alias defined in `SELECT` can't be used in `WHERE` (WHERE runs before SELECT) but *can* be used in `ORDER BY` (runs after). It's also why `WHERE` filters rows before aggregation and `HAVING` filters groups after.

### Joins
| Join | Returns |
|---|---|
| `INNER JOIN` | Only matching rows on both sides |
| `LEFT (OUTER) JOIN` | All left rows + matched right (else `NULL`) |
| `RIGHT (OUTER) JOIN` | All right rows + matched left (else `NULL`) |
| `FULL OUTER JOIN` | Union of LEFT and RIGHT (all rows, `NULL` where unmatched) |
| `CROSS JOIN` | Cartesian product — every row × every row (degree = sum, cardinality = product) |
| `SELF JOIN` | A table joined to itself via aliases — for hierarchies/recursive relations (e.g. `Employee e1 JOIN Employee e2 ON e1.Manager_ID = e2.Emp_ID`) |

**Join algorithms** (optimizer picks based on stats/indexes): **Nested Loop** (for each outer row scan inner — good with a small outer + index on inner) · **Hash Join** (build hash table on smaller relation, probe with the other — good for large unsorted equi-joins) · **Merge Join** (both inputs sorted on join key, linear merge — good if already sorted/indexed).

### Aggregates & Grouping
`COUNT`, `SUM`, `AVG`, `MIN`, `MAX` ignore `NULL`s (except `COUNT(*)`). `GROUP BY` partitions rows before aggregation; `HAVING` filters *groups* (post-aggregation) where `WHERE` can't (WHERE runs pre-aggregation, can't reference aggregate results).

### Subqueries vs Joins vs CTEs
- **Scalar/correlated subquery**: re-evaluated per outer row — can be slow (`O(N×M)`) unless the optimizer rewrites it into a join/semi-join.
- **`EXISTS`/`IN`**: `EXISTS` short-circuits on first match, generally preferred over `IN` for large/correlated subqueries (and `NOT IN` has a NULL-poisoning footgun: if the subquery returns any `NULL`, `NOT IN` returns zero rows — use `NOT EXISTS` instead).
- **CTE (`WITH ... AS (...)`)**: names a subquery for readability/reuse in the same statement; `WITH RECURSIVE` walks trees/graphs (e.g. an org chart via `Manager_ID`, or the adjacency-list comment tree from §4).
- **Window functions vs `GROUP BY`**: `GROUP BY` collapses rows into one row per group; window functions (`OVER (...)`) compute per-row values *without* collapsing rows.

### Window Functions
```sql
ROW_NUMBER() OVER (PARTITION BY dept ORDER BY salary DESC)   -- unique sequential rank, no ties
RANK()       OVER (PARTITION BY dept ORDER BY salary DESC)   -- ties share rank, gaps after
DENSE_RANK() OVER (PARTITION BY dept ORDER BY salary DESC)   -- ties share rank, no gaps
LAG(col, 1)  OVER (ORDER BY date)                            -- previous row's value
LEAD(col, 1) OVER (ORDER BY date)                             -- next row's value
SUM(amt) OVER (PARTITION BY user ORDER BY date)                -- running total
```

**Classic interview pattern — Nth highest salary**:
```sql
SELECT DISTINCT salary FROM Emp ORDER BY salary DESC LIMIT 1 OFFSET N-1;
-- or, ties-aware:
SELECT salary FROM (SELECT salary, DENSE_RANK() OVER (ORDER BY salary DESC) rnk FROM Emp) t WHERE rnk = N;
```

**Find duplicate rows**:
```sql
SELECT col, COUNT(*) FROM T GROUP BY col HAVING COUNT(*) > 1;
```

### Views, Constraints, `EXPLAIN`
- **View** = a named, stored `SELECT` — not materialized by default (recomputed per query) unless a **Materialized View** is used (physically stored, refreshed on a schedule/trigger — trades staleness for read speed).
- **`EXPLAIN`/`EXPLAIN ANALYZE`**: shows the chosen execution plan (scan type — seq scan vs index scan/index-only scan, join algorithm, estimated vs actual rows/cost) — the primary tool for diagnosing a slow query and deciding whether an index is missing or unused.
- A `WHERE` clause that wraps an indexed column in a function (`WHERE YEAR(date) = 2026`) usually **defeats the index** (non-sargable) — rewrite as a range (`date >= '2026-01-01' AND date < '2027-01-01'`) to keep the index usable.

---

## 8. Functional Dependencies & Normalization

**Why normalize** — un-normalized designs (e.g. `Emp_Dept(Emp_ID, Emp_Name, Dept_ID, Dept_Name, Dept_Head)`) cause three anomalies:
1. **Insertion anomaly**: can't add a new Department until some Employee is assigned to it (PK can't be null).
2. **Deletion anomaly**: deleting the last employee in a department silently deletes the department's data too.
3. **Update anomaly**: a fact stored redundantly (Dept_Head) must be updated in every row that repeats it, or the DB goes inconsistent.

**Functional Dependency** $X \to Y$: for any two tuples, $t_1[X]=t_2[X] \implies t_1[Y]=t_2[Y]$. **Trivial** if $Y \subseteq X$.

**Armstrong's Axioms** (sound & complete): Reflexivity ($Y \subseteq X \implies X\to Y$) · Augmentation ($X\to Y \implies XZ \to YZ$) · Transitivity ($X\to Y, Y\to Z \implies X\to Z$). Derived: Union, Decomposition, Pseudo-transitivity.

**Attribute Closure $X^+$ algorithm**: start $X^+ = X$; repeatedly, for any FD $Y\to Z$ with $Y \subseteq X^+$, add $Z$ to $X^+$; stop when no change. $X$ is a **super key** iff $X^+ = $ all attributes of $R$; a **candidate key** if additionally no proper subset of $X$ has that property.

**Canonical Cover ($F_c$)**: minimal equivalent FD set — singleton RHS, no extraneous LHS/RHS attributes, no redundant FDs. Required before 3NF synthesis (Bernstein's Algorithm) to avoid producing redundant tables.

### Normal Forms Ladder

| NF | Condition | Fixes |
|---|---|---|
| **1NF** | Every attribute is atomic/scalar — no repeating groups/multivalued/composite cells | Split multivalued cells into separate rows |
| **2NF** | 1NF + no **partial dependency** (a non-prime attribute depending on a *proper subset* of a composite candidate key) | Split off the partially-dependent attribute into its own table keyed on the determining subset |
| **3NF** | 2NF + for every $X\to A$: $X$ is a super key **OR** $A$ is prime (member of some CK) | Split off transitive dependencies (non-key → non-key chains) |
| **BCNF** | For every non-trivial $X\to A$: $X$ **must** be a super key — no exception for prime $A$ | Repeatedly split on any violating FD |

- Any relation with only **single-attribute candidate keys** is automatically in 2NF (no composite CK ⇒ no proper non-empty subset exists ⇒ no partial dependency possible).
- If every attribute of $R$ is independently a CK (all determinants are super keys), $R$ is automatically in **BCNF**.

**Lossless-Join Decomposition** — splitting $R$ into $R_1, R_2$ is lossless iff $R_1 \bowtie R_2 = R$, which holds iff the common attributes $(R_1 \cap R_2)$ functionally determine **at least one** of $R_1$ or $R_2$ in full (i.e. $(R_1\cap R_2)$ is a super key of at least one side). Otherwise the join reintroduces **spurious tuples**.

**Dependency Preservation**: $(F_1 \cup \dots \cup F_k)^+ = F^+$ — every original FD can still be *checked* within a single decomposed table, without a cross-table join. Matters because enforcing a non-preserved FD needs an expensive join-based `CHECK` on every write.

**3NF vs BCNF trade-off**:

| | 3NF | BCNF |
|---|---|---|
| Rule | $X$ super key **OR** $A$ prime | $X$ **must** be super key |
| Lossless join | Always achievable | Always achievable |
| Dependency preservation | **Always guaranteed** | **Not always guaranteed** |

- **BCNF decomposition can lose a dependency.** Classic example: $R(A,B,C,D)$, $F=\{AB\to C, C\to D, D\to A\}$ — decomposing on the BCNF-violating $C\to D$ then $C\to A$ ultimately splits $A$ and $B$ into different tables, so $AB\to C$ can no longer be checked in any single relation — it's lost.
- **Bernstein's 3NF Synthesis** (bottom-up: build one relation per canonical-cover FD, add a key-relation if none contains a CK) guarantees *both* properties; top-down BCNF chopping only guarantees lossless join.

---

## 9. Transactions & ACID

**Transaction**: a logical unit of work moving the DB from one consistent state to another.

**State diagram**: `Active` → (`Partially Committed` → `Committed`) or (`Failed` → `Aborted`).
- **Partially Committed**: final operation executed in memory, but log buffers not yet `fsync`'d to disk.
- **Committed**: log flushed to durable storage — permanent.
- **Failed**: hardware error, deadlock, or constraint violation blocks further progress.
- **Aborted**: rolled back to pre-transaction state; may restart (system error) or be killed (bad input).

### ACID
| Property | Guarantee | Threat | Enforced by |
|---|---|---|---|
| **Atomicity** | All-or-nothing | Partial crash mid-transaction | Undo logging / shadow paging |
| **Consistency** | Valid state → valid state (schema + business invariants) | Buggy logic bypassing constraints | Joint: DBMS constraint checks + app logic |
| **Isolation** | Concurrent txns behave as if serial | Interleaving anomalies (dirty/non-repeatable/phantom reads, lost updates) | Concurrency control (2PL / timestamps / MVCC) |
| **Durability** | Committed data survives crashes | Crash losing RAM-resident writes | WAL + redo logs, `fsync` before ack |

### Buffer Management Policies (how Atomicity + Durability are jointly achieved)
| Axis | Option | Meaning |
|---|---|---|
| **Steal / Atomicity** | STEAL | dirty (uncommitted) pages *can* be flushed early to free buffer space → needs **Undo** log |
| | NO-STEAL | uncommitted pages never hit disk → no Undo needed, but memory-hungry |
| **Force / Durability** | FORCE | all modified pages flushed *before* commit returns → high commit latency |
| | NO-FORCE | commit returns as soon as the **log** is flushed; dirty pages linger in RAM → needs **Redo** log |

Modern engines use **STEAL / NO-FORCE** (best performance) which is exactly why **WAL** exists: undo info must reach disk before its dirty page does (to support STEAL); redo info must reach disk before commit is acknowledged (to support NO-FORCE).

**Cascading Abort**: $T_1$ writes $X$ (uncommitted) → $T_2$ reads $X$ → $T_1$ aborts → $T_2$'s read was dirty, so $T_2$ must abort too → chain reaction. Avoided by **Cascadeless Schedules** (Strict 2PL): only ever read committed data.

**MVCC** (PostgreSQL, InnoDB, Oracle): writes create a *new version* of a tuple (timestamped) instead of overwriting in place; reads see the latest version committed at-or-before their own snapshot start. Result: **"Readers never block writers, writers never block readers."**

---

## 10. Concurrency Control: Anomalies & Serializability

### The Four Classic Anomalies
| Anomaly | Cause | SQL isolation level that first prevents it |
|---|---|---|
| **Dirty Read** | $T_2$ reads $T_1$'s uncommitted write; $T_1$ then aborts | READ COMMITTED |
| **Non-Repeatable Read** | $T_1$ re-reads $X$, sees a different value because $T_2$ committed a write in between | REPEATABLE READ |
| **Phantom Read** | $T_1$ re-runs a range query, gets a different *row set* because $T_2$ inserted/deleted a matching row | SERIALIZABLE |
| **Lost Update** | $T_1,T_2$ both read $X$, both write; the second write clobbers the first's update entirely | READ COMMITTED (usually) |

### SQL Isolation Levels
| Level | Dirty Read | Non-Repeatable Read | Phantom Read |
|---|---|---|---|
| READ UNCOMMITTED | Possible | Possible | Possible |
| READ COMMITTED | Prevented | Possible | Possible |
| REPEATABLE READ | Prevented | Prevented | Possible |
| SERIALIZABLE | Prevented | Prevented | Prevented |

### Schedules & Conflict Serializability
Two operations **conflict** iff: different transactions + same data item + at least one is a `WRITE`.

**Precedence (Serialization) Graph**: node per transaction; edge $T_i \to T_j$ if a conflicting operation of $T_i$ precedes the corresponding one of $T_j$ in the schedule.

$$\text{Schedule is Conflict Serializable} \iff \text{its Precedence Graph is acyclic}$$
Acyclic → topological sort gives an equivalent serial order. Cyclic → not conflict serializable.

- **Conflict serializability check**: $O(V+E)$ via cycle detection (DFS/topo-sort).
- **View serializability** (allows blind writes without a preceding read) is strictly weaker/more permissive but checking it is **NP-Complete** (reduces to 3-SAT) — so real engines enforce conflict serializability (via 2PL) instead.

**Eliminating phantoms under SERIALIZABLE**: predicate locking (lock the `WHERE` condition itself — too slow in practice) vs **Next-Key Locking** (InnoDB) — combines a record lock with a **gap lock** covering the key-space interval around it, so a concurrent `INSERT` into that gap blocks until the range-scanning transaction finishes.

---

## 11. Concurrency Control Protocols: Locking & Timestamps

### Lock Types
**Shared (S)**: multiple readers OK. **Exclusive (X)**: sole holder, blocks everyone else. Compatibility: S+S=OK, anything with X=blocked.

### Two-Phase Locking (2PL)
Growing Phase (acquire only) → Lock Point → Shrinking Phase (release only). **Guarantees Conflict Serializability** — proof: a cycle in the precedence graph would force `LockPoint(T1) < LockPoint(T1)`, a contradiction.

| Variant | Rule | Guarantees | Deadlock-free? |
|---|---|---|---|
| **Basic 2PL** | Standard growing/shrinking | Conflict serializability | No |
| **Strict 2PL** | Hold all **X** locks until commit/abort | + Cascadeless (no dirty reads) | No |
| **Rigorous 2PL** | Hold **all** locks (S and X) until commit/abort | Commit order = serialization order | No |
| **Conservative (Static) 2PL** | Acquire *all* locks upfront before executing, or wait | Deadlock-free by construction | **Yes** |

**Multiple Granularity Locking (MGL) & Intent Locks**: to lock a whole table without scanning every tuple for conflicts, a transaction first acquires **Intent** locks (IS/IX/SIX) on all ancestor nodes (DB→Table→Page→Tuple) before locking the leaf — any other transaction can then check compatibility at the coarse level in O(1) instead of scanning down.

### Timestamp Ordering (optimistic, no locks)
Every transaction gets a unique increasing $TS(T_i)$; every data item tracks $R\text{-}TS(X)$, $W\text{-}TS(X)$.
- **Read** $T_i(X)$: reject/abort if $TS(T_i) < W\text{-}TS(X)$ (reading a value already overwritten by someone newer); else read, bump $R\text{-}TS(X)$.
- **Write** $T_i(X)$: reject/abort if $TS(T_i) < R\text{-}TS(X)$ or $TS(T_i) < W\text{-}TS(X)$ (would violate a newer read or write); else write, set $W\text{-}TS(X)=TS(T_i)$.
- **Thomas' Write Rule**: optimization — if $TS(T_i) < W\text{-}TS(X)$, just **silently skip** the obsolete write instead of aborting $T_i$ (a newer value already committed, so the stale write is a no-op) — allows view-serializable-but-not-conflict-serializable schedules.
- **Starvation risk**: a long transaction can be repeatedly aborted by younger ones. Fix: on restart, keep the transaction's **original** timestamp rather than issuing a fresh one, so it eventually becomes "old enough" to win.

### Protocol Comparison
| Protocol | Strategy | Deadlock? | Cascading rollback? |
|---|---|---|---|
| Basic/Strict 2PL | Pessimistic locks | Yes | Basic: yes / Strict: no |
| Conservative 2PL | Pre-declare all locks | **No** | No |
| Basic Timestamp Ordering | Optimistic | No | Yes |
| Thomas' Write Rule | Optimistic, skips stale writes | No | Yes |

---

## 12. Deadlocks & Crash Recovery (ARIES)

### Deadlock Prevention (timestamp-based, avoids ever entering a cycle)
$T_i$ requests a lock held by $T_j$:

| Scheme | $T_i$ older than $T_j$ | $T_i$ younger than $T_j$ | Style |
|---|---|---|---|
| **Wait-Die** | $T_i$ **waits** | $T_i$ **dies** (aborts, restarts) | Non-preemptive |
| **Wound-Wait** | $T_i$ **wounds** (aborts) $T_j$ | $T_i$ **waits** | Preemptive |

Wound-Wait tends to produce **fewer total aborts** under high contention (an old transaction sweeps through by wounding, rather than repeatedly dying and retrying).

### Deadlock Detection: Wait-For Graph (WFG)
Node = transaction; edge $T_i \to T_j$ = "$T_i$ waiting on a lock $T_j$ holds." **A cycle in the WFG ⟺ deadlock.** On detection, pick a **victim** to abort — criteria: least work done / fewest locks held / cheapest rollback, with an aging counter to avoid repeatedly starving the same victim.

### Failure Classification
Transaction failure (logic error, deadlock abort) · System crash / soft failure (power/OS/RAM loss, disk intact) · Disk failure / hard failure (physical media corruption).

### Log-Based Recovery
Log record forms: `<Ti, Start>`, `<Ti, X, Old, New>`, `<Ti, Commit>`, `<Ti, Abort>`.

| Modification style | When disk writes happen | Log needs | Recovery needs |
|---|---|---|---|
| **Deferred** (NO-STEAL) | Only after commit | Redo values only | REDO only |
| **Immediate** (STEAL) | Any time before commit | Undo **and** Redo values | UNDO uncommitted + REDO committed |

**Checkpointing**: periodically snapshots dirty-page/active-transaction state so crash recovery doesn't have to scan the log from the beginning of time. **Fuzzy checkpointing** (used in production) doesn't stop-the-world — it snapshots the Dirty Page Table + Transaction Table while background threads keep flushing asynchronously, avoiding the latency spikes of a strict/frozen checkpoint.

**ARIES recovery — 3 phases**:
1. **Analysis** (forward from last checkpoint): rebuild the Dirty Page Table + Transaction Table as of crash time.
2. **Redo** ("repeat history", forward from min RecLSN): re-apply *every* logged update — committed or not — to reconstruct exact crash-time state.
3. **Undo** (backward): roll back all still-active ("loser") transactions, writing **Compensation Log Records (CLRs)** so a second crash mid-undo doesn't redo the undo infinitely.

---

## 13. Indexing

Without an index: full table scan = $O(N)$ block I/Os. An index (B+Tree / hash) gets this to $O(\log N)$ or $O(1)$.

**Dense vs Sparse**:
| | Dense | Sparse |
|---|---|---|
| Entries | One per search-key value (every row) | One per data **block** |
| Requirement | None | Data file **must be physically sorted** on the key |
| Space | More | Less |
| Index-only queries | Possible | Not directly |

**Primary / Clustered / Secondary**:
| | Primary Index | Clustered Index | Secondary Index |
|---|---|---|---|
| Data file order | Sorted on PK | Sorted on a non-key attribute | Unsorted |
| Density | Sparse | Sparse | **Must be dense** |
| Max per table | 1 | 1 | Many |

A table can have only **one** clustered/primary index because the underlying data can only be physically sorted one way; secondary indexes are separate structures pointing back in, so you can have many.

**Multi-level indexing**: when even the (sparse) index itself is too big for memory, build a sparse index *over the index* — recursively, until the top level fits in one block. Worked example (1M records, 4KB blocks, 200B records, 16B key + 8B pointer): flat binary search on data = 16 I/Os; a 2-level index = **3 I/Os** (top block + inner block + data block) — ~81% fewer I/Os.

**Write amplification**: inserting one row into a table with $k$ secondary indexes triggers up to $k+1$ random writes. **Change Buffer** (InnoDB): if the target secondary-index page isn't in the buffer pool, defer/cache the change in memory and merge it in later when that page is naturally read in — converts random I/O into batched sequential work.

**Why secondary indexes store the Primary Key (not a physical RID)** as their pointer: a physical `(page,slot)` RID would need updating in *every* secondary index whenever the clustered B+Tree reorganizes (page splits, row moves). Storing the logical PK means physical reorganization costs **zero** secondary-index updates — at the cost of an extra lookup hop ("bookmark lookup": secondary index → PK → clustered index → row).

---

## 14. B-Trees & B+ Trees

Disk access ≈ 100,000× slower than RAM. A binary tree (fan-out 2) is too tall; B-Trees/B+Trees maximize **fan-out** ($m \approx 100$–$1000$) to keep height ≤ 3–4, so even billions of rows need only a few disk reads.

**B-Tree**: internal nodes store **both** keys and data pointers. **B+Tree**: internal nodes store **only** keys + child pointers; all data pointers live in **leaf** nodes, which are additionally linked via a doubly-linked list.

| | B-Tree | B+ Tree |
|---|---|---|
| Data pointers | In internal + leaf nodes | **Only** in leaves |
| Fan-out | Lower (data ptr space overhead) | Higher |
| Point query | Can short-circuit at an internal node | Always walks to a leaf, but that's uniform/predictable |
| Range query | Slow — in-order traversal | **Fast** — reach start leaf, then walk the linked list |

Why DBs use B+Trees: uniform leaf-level data layout + linked leaves make range scans (`BETWEEN`, `ORDER BY` on the indexed column) sequential instead of tree-walk-heavy.

**Insertion**: descend to target leaf, insert sorted; on overflow (> $m-1$ keys), split at the median — leaf split **copies** the median up (keeps a copy in the right leaf, since it's real data); internal split **pushes** the median up (doesn't duplicate — internal nodes hold no data).

**Deletion**: on underflow (< $\lceil m/2 \rceil - 1$ keys), first try **borrowing** from a sibling with slack, else **merge** with a sibling and pull the separator key down from the parent.

**Order formulas** ($B$=block size, $K$=key size, $P$=pointer size, $D$=data-pointer size):
$$m_{internal} \le \frac{B+K}{P+K} \qquad m_{leaf} \le \frac{B - P_{next}}{K+D}$$
Worked: 8KB blocks, 16B keys, 8B block ptr, 12B record ptr → internal order $m{=}342$ (341 keys/node), leaf capacity 292 records/leaf. A 3-level tree (root→internal→leaf) then indexes $342 \times 292 \times 342 \approx 34$ million records in **3 I/Os**.

**Production refinements**: **Prefix compression** (shared key prefixes stored once per node → higher effective fan-out) · **Lehman & Yao B-link trees** (PostgreSQL) — a high-key right-sibling pointer lets readers traverse lock-free even while concurrent writers split sibling nodes · **Slotted-page layout** for variable-length rows — a slot array of offsets at the page's top grows downward, actual variable-length tuple bytes grow upward from the bottom; deletes/updates locally defragment without invalidating external B+Tree pointers into the page.

---

## 15. Types of Databases & NoSQL

**RDBMS**: tables, schema-on-write, SQL, strict ACID (PostgreSQL, MySQL, Oracle, SQL Server).
**OODBMS / ORDBMS**: OODBMS blends OOP concepts (objects/classes/inheritance) directly into the DB (mostly historical, db4o); ORDBMS extends relational tables with user-defined types/arrays/JSON (PostgreSQL `JSONB`) — the pragmatic survivor.

### The Four NoSQL Families
| Type | Model | Examples | Best for |
|---|---|---|---|
| **Document** | Self-describing JSON/BSON | MongoDB, CouchDB | Content/catalogs, schema-on-read |
| **Key-Value** | Hash map, opaque K→V | Redis, Memcached, DynamoDB | Caching, sessions, O(1) lookups |
| **Wide-Column** | Sparse column families | Cassandra, HBase, ScyllaDB | Massive sequential writes, time-series |
| **Graph** | Nodes + edges + properties | Neo4j, Neptune | Deep relationship traversal, fraud/recs |

**Legacy models**: Hierarchical (tree, 1 parent only — IBM IMS) · Network (child can have multiple parents, M:N via "sets" — CODASYL/IDMS).

**Index-Free Adjacency** (Graph DBs): each node holds direct physical pointers to its neighbors, so traversing an edge is $O(1)$ regardless of DB size — vs. an RDBMS multi-hop join, which costs $O(k \log N)$ (one indexed lookup per hop). This is why graph DBs crush deep-traversal queries (friend-of-friend-of-friend) that would otherwise chain many joins.

**Why OODBMS lost to RDBMS**: no standard declarative query language (procedural navigational APIs instead of SQL), weak ad-hoc querying (pre-defined navigation paths only), and ORMs (Hibernate) + ORDBMS features (`JSONB`) solved the "impedance mismatch" without sacrificing SQL/ACID/index maturity.

**Choosing a store for a write-heavy time-series workload** (e.g. 10k GPS pings/sec): RDBMS B+Tree page-splits thrash under concurrent random-key writes; wide-column stores (Cassandra) use **LSM trees** — sequential append to an in-memory MemTable + commit log, sharded by `(device_id, timestamp)` — avoiding random I/O entirely.

---

## 16. SQL vs NoSQL, CAP & PACELC

### ACID vs BASE
| ACID (SQL) | BASE (NoSQL) |
|---|---|
| Atomicity, Consistency, Isolation, Durability | **B**asically **A**vailable, **S**oft state, **E**ventual consistency |

**Schema-on-write** (SQL: DDL validated at insert time) vs **schema-on-read** (NoSQL: structure interpreted by the app at query time).

| Dimension | SQL | NoSQL |
|---|---|---|
| Scaling | Vertical (scale-up) | Horizontal (scale-out, sharded) |
| Transactions | Strict ACID | BASE / eventual |
| Bottleneck | Disk I/O + write locks, single node | Cross-node network latency, consistency lag |
| Best for | Financial/complex-relationship data | High-velocity feeds, huge scale, caching |

Modern convergence (`JSONB` in Postgres; multi-doc ACID in MongoDB 4.0+) blurs the surface API, but underlying architecture differs: RDBMS = single-node in-memory-speed ACID via B+Trees; NoSQL multi-document transactions require **2PC across shards** ($O(N^2)$ message cost) — genuinely slower at scale, not just a marketing distinction.

### Tunable Consistency (Cassandra-style)
$$R + W > N \implies \text{strong (read-your-write) consistency}$$
$N$=replication factor, $W$=nodes that must ack a write, $R$=nodes queried on read. If $R+W>N$, the read and write quorums must overlap by ≥1 node, guaranteeing the read sees the latest write. $N{=}3, W{=}R{=}\text{QUORUM}(2)$: $2+2=4>3$ → strong. $N{=}3, W{=}R{=}1$: $2 \not> 3$ → eventual/stale reads possible.

### LSM Trees vs B+ Trees (why NoSQL writes so much faster)
B+Tree writes = random disk seek to the exact leaf page. **LSM Tree** writes = sequential append to an in-memory sorted **MemTable** + commit log; on flush, becomes an immutable sorted **SSTable** on disk; background compaction merges SSTables. Trade sequential-write speed for slower point reads (mitigated with **Bloom filters** to skip SSTables that can't contain the key).

### CAP Theorem
Formulated by Brewer (2000), proven by Gilbert & Lynch (2002). Three guarantees, pick at most 2 **during a partition**:
- **Consistency** (linearizability — every read sees the latest write, or errors).
- **Availability** — every non-failing node returns a non-error response (possibly stale).
- **Partition Tolerance** — system keeps working despite dropped/delayed inter-node messages.

**Partitions are physically inevitable in any distributed system**, so $P$ is never really optional — the real choice during a partition is **C vs A**. "CA" is a meaningless label for anything actually distributed (only a single-node/non-partitioned system can claim it).

**2-node partition proof**: $G_1, G_2$ both hold $v_0$; link severed. Client writes $v_1$ to $G_1$. Either $G_1$ applies it and returns success (chooses **A**, but a concurrent read of $G_2$ returns stale $v_0$ — **C violated**) or $G_1$ blocks/errors (chooses **C**, but failed to answer a valid request — **A violated**). Can't have both. QED.

| | CP | AP |
|---|---|---|
| On partition | Blocks/errors until healed | Returns local (possibly stale) state |
| Examples | HBase, MongoDB (primary), CockroachDB | Cassandra, DynamoDB, Couchbase |
| Best for | Banking ledgers, reservations | Feeds, carts, like-counters |

### PACELC (Abadi, 2012) — extends CAP to the *normal* (non-partitioned) case
$$P \implies A \text{ vs } C \quad\big|\quad \text{Else} \implies L \text{ vs } C$$
Even with no partition, there's still a Latency-vs-Consistency trade-off (multi-region consensus commits cost latency).

| System | Partition | Normal | Class |
|---|---|---|---|
| MongoDB, HBase | C | C | **PC/EC** |
| Cassandra, DynamoDB, async MySQL | A | L | **PA/EL** |

Fintech pattern: use a **PC/EC** store (CockroachDB) for the balance ledger (correctness > latency, no double-spend) and a **PA/EL** store (Cassandra/DynamoDB) for activity/audit logs (throughput > strict consistency).

---

## 17. Clustering & Replication

**Shared-Disk** (Oracle RAC): multiple compute nodes, one shared SAN/NAS — easy to add nodes, but the storage network + distributed lock manager become the bottleneck. **Shared-Nothing** (Cassandra, CockroachDB): each node owns its own CPU/RAM/disk, communicates only via network messages — near-infinite horizontal scale, but needs real sharding + distributed query routing.

**Synchronous vs Asynchronous replication**:
| | Synchronous | Asynchronous |
|---|---|---|
| Commit latency | High (waits for replica ack) | Low (returns immediately) |
| Data-loss risk (RPO) | Zero | Master can die before replicating a committed write |
| If replica down | Master **blocks writes** | Master keeps accepting writes |

**Semi-synchronous**: waits for just **one** replica to ack before returning — balances the two.

**RTO vs RPO**: **RTO** (Recovery Time Objective) = max acceptable *downtime*. **RPO** (Recovery Point Objective) = max acceptable *data loss window*. Banking-grade: RPO≈0, RTO<30s.

**Split-Brain**: a network partition splits the cluster into isolated groups, each of which may independently promote its own "master," diverging state irreparably once healed. **Fix — Quorum Consensus**: any promotion/write needs a strict majority $Q > N/2$; a minority partition can't reach quorum and steps down to read-only.

**Replication formats**: **Statement-based** (replay SQL text — cheap, but breaks on non-deterministic functions like `NOW()`/`RAND()`/`UUID()`, which evaluate differently on replicas) · **Row-based** (replay exact row deltas — deterministic, safe, but bandwidth-heavy for bulk updates) · **Mixed** (statement by default, auto-switches to row-based when a non-deterministic statement is detected).

**Read-Your-Own-Writes on async replicas**: either **sticky-session pin** reads to the master for a short window after any write by that user, or track a monotonic **LSN/timestamp** returned on write and route the follow-up read only to a replica whose applied LSN has caught up (else wait, or fall back to master).

---

## 18. Database Scaling: Partitioning & Sharding

**Vertical scaling** (bigger single box) vs **Horizontal scaling** (more commodity nodes): vertical hits a hard physical ceiling with exponential cost; horizontal is near-unlimited but linear-cost and operationally complex (needs partitioning/sharding).

**Vertical Partitioning** (split columns by access pattern — hot auth columns in one table, cold blob columns in another) vs **Horizontal Partitioning / Sharding** (split rows across independent physical nodes).

### Sharding Strategies
| Strategy | Mechanism | Pro | Con |
|---|---|---|---|
| **Range-based** | Contiguous key ranges per shard | Simple, efficient range queries | Hotspotting (new writes cluster on one shard) |
| **Hash-based** | `hash(key) % N` | Even distribution, no hotspots | Range queries become expensive scatter-gather |
| **Consistent Hashing** | Nodes + keys mapped onto a circular hash ring; a key belongs to the first node clockwise | Adding/removing a node only remaps ~$1/N$ of keys (not ~100%) | Needs **virtual nodes** to avoid uneven bucket sizes |

**Sharding drawbacks**: cross-shard JOINs (must fetch and join over the network — slow), cross-shard transactions (need 2PC — latency-heavy), scatter-gather queries (any query without the shard key hits every node).

**Celebrity/hotspot mitigation**: salt the sharding key (`(Post_ID, Random(1,N))`) to spread one hot post's writes across $N$ buckets, accepting a scatter-gather read cost; or buffer writes through a queue (Kafka/Redis Stream) and batch-flush.

### Two-Phase Commit (2PC)
```
Coordinator --PREPARE--> Participants
Coordinator <--VOTE_COMMIT/ABORT-- Participants
Coordinator --GLOBAL_COMMIT--> Participants   (only if ALL voted commit)
```
**Blocking failure mode**: if the coordinator crashes *after* all participants vote commit but *before* sending `GLOBAL_COMMIT`, participants sit holding locks indefinitely in limbo — this is why modern distributed DBs use non-blocking consensus (**Raft**/**Paxos**) with leader election instead of raw 2PC.

**Zero-downtime resharding pipeline**: dual-write (old + new shards simultaneously) → backfill historical data → verify/reconcile via checksums → gradually shift read traffic (canary: 1%→10%→100%) → stop dual-writing, decommission old shards.

---

## 19. Master-Slave, Failover & Distributed Transactions

**Topology**: writes → Master; reads → load-balanced across Slaves (read replicas), fed by a replication stream (WAL/binlog).

**Failover sequence**: (1) heartbeat failure detection → (2) consensus leader election (Raft/Paxos/ZooKeeper/Orchestrator) → (3) promote the slave with the **highest LSN/GTID** (most up-to-date, avoids rolling back committed data) → (4) reroute clients (VIP takeover / proxy routing table update, e.g. ProxySQL/HAProxy).

**GTID (Global Transaction ID)** = `UUID:Transaction_ID`, consistent across all nodes — lets a failover tool identify exactly which transactions each candidate slave has applied, and promote the most-caught-up one for RPO≈0.

**Cascading (Relay) Replication**: instead of one master streaming binlogs directly to 500 slaves (saturates the master's NIC), the master replicates to a few **relay slaves**, which fan out to the leaf slaves — cuts master-side replication traffic by >99%.

**Failure-mode cheat sheet**:
| Failure | Cause | Mitigation |
|---|---|---|
| Split-brain | Partition isolates master | Quorum majority ($Q>N/2$) |
| Stale reads | Async replication lag | Session pinning / GTID-aware routing |
| Cascading reconnect storm | Master dies, all slaves reconnect at once | Relay/cascade replication tiers |

**System-design synthesis — high-TPS fintech payments**: ledger/balances on a **PC/EC** NewSQL store (CockroachDB/YugabyteDB, multi-region Raft, strict ACID) with **UUIDv7** (time-ordered) primary keys to avoid B+Tree page-split fragmentation from random UUIDv4s; transaction history on **PA/EL** wide-column (Cassandra/DynamoDB) for O(1) append-heavy writes; Redis for idempotency-key caching; hash-sharded on `User_ID` with consistent hashing + salting for merchant hotspots; semi-sync replication with QUORUM write concern, multi-region 5-node Raft (RTO<10s, RPO=0); continuous WAL streaming to encrypted multi-region object storage for DR.

---

## 20. Quick-Fire Interview Q&A

- **Why isn't zero redundancy the goal of a DBMS?** Controlled redundancy (FKs, secondary indexes, denormalized reads, replicas) trades storage/update cost for read speed and fault tolerance — total normalization would kill join/replication performance.
- **Logical vs Physical data independence — which is harder?** Logical (changing the conceptual schema without breaking apps) — it touches business logic/queries, not just a standard DBMS feature like an index swap.
- **Weak entity vs M:N relationship — the one-line difference?** A weak entity *cannot exist at all* without its owner; entities in an M:N relationship can each exist with zero relationship rows.
- **Why can a ternary relationship not always be split into three binaries?** Without a functional dependency tying two of the three entities, the binary pairs don't prove the original triple co-occurred — joining them back produces spurious tuples.
- **Super key vs candidate key?** Super key = any uniquely-identifying attribute set; candidate key = a *minimal* super key (no proper subset still works).
- **Why must PK be NOT NULL but FK can be NULL?** Entity integrity: a NULL PK means an unidentified/nonexistent tuple, breaking relational identity. A NULL FK just means "no relationship yet" (valid, e.g. an unassigned employee).
- **1NF → 2NF → 3NF → BCNF, one-line rule for each?** 1NF: atomic values. 2NF: no non-prime attribute depends on a *part* of a composite key. 3NF: no non-prime attribute depends on another non-prime (transitively). BCNF: every determinant must be a super key, full stop — no prime-attribute exception.
- **Does 3NF decomposition always preserve dependencies? BCNF?** 3NF: yes, always (lossless + dependency-preserving guaranteed). BCNF: lossless yes, dependency-preserving **not always** — chopping on a violating FD can split a compound determinant across tables so it can no longer be checked anywhere.
- **STEAL/NO-STEAL vs FORCE/NO-FORCE — what do they protect?** STEAL/NO-STEAL is about **Atomicity** (can uncommitted pages hit disk early? if so, need Undo). FORCE/NO-FORCE is about **Durability** (must all pages hit disk before commit acks? if not, need Redo). Production uses STEAL+NO-FORCE, hence WAL.
- **Dirty read vs non-repeatable read vs phantom read?** Dirty = reading *uncommitted* data. Non-repeatable = same row, value changed between two reads (a committed UPDATE happened in between). Phantom = same range query, different *row count/set* (a committed INSERT/DELETE happened in between).
- **Why is 2PL guaranteed to be conflict serializable?** A cycle in the precedence graph would force each transaction's lock point to precede itself around the cycle — a logical contradiction — so no cycle can exist under 2PL.
- **Basic 2PL vs Strict 2PL vs Conservative 2PL?** Basic: standard grow/shrink, still allows cascading aborts. Strict: hold X-locks till commit — no dirty reads/cascading aborts, but can deadlock. Conservative: acquire *all* locks upfront — deadlock-free, but hard to predict needed locks in advance.
- **Wait-Die vs Wound-Wait?** Wait-Die: older waits, younger dies (non-preemptive) — more total aborts under contention. Wound-Wait: older wounds (aborts) younger, younger waits (preemptive) — fewer total aborts.
- **How does the WFG detect deadlock?** Edge $T_i \to T_j$ = "$T_i$ waits for a lock $T_j$ holds"; a **cycle** in this graph is necessary and sufficient for deadlock.
- **ARIES's three phases, one line each?** Analysis (rebuild crash-time state from the checkpoint forward), Redo (repeat history — reapply every logged change, committed or not), Undo (roll back losers backward, logging CLRs to survive a repeated crash).
- **Why must secondary indexes be dense while primary/clustering indexes can be sparse?** Sparse only works if the data file is physically sorted on that key (so you can scan forward from the nearest index entry) — secondary index keys aren't the sort order of the file, so every value needs its own entry.
- **Why do InnoDB secondary indexes store the PK instead of a physical row address?** So that clustered B+Tree reorganization (page splits, row moves) costs zero updates to secondary indexes — at the cost of an extra lookup hop (bookmark lookup).
- **B-Tree vs B+Tree, the one fact that matters?** B+Tree keeps all data pointers in linked leaf nodes only — internal nodes are pure routing — which makes range scans a fast linked-list walk instead of a tree traversal.
- **R + W > N — what does it actually guarantee?** That any write-quorum and read-quorum set of nodes must overlap by at least one node, so a read always sees the most recent committed write on that overlapping replica.
- **Why can't a distributed system be "CA"?** Network partitions are a physical inevitability at scale — a real distributed system must tolerate them (P), so the actual live trade-off is always C vs A during a partition, not "avoid P."
- **CAP vs PACELC — what does PACELC add?** CAP only describes behavior *during* a partition. PACELC adds the *normal-operation* trade-off (Latency vs Consistency) for when there's no partition at all — most of the time.
- **Why do LSM trees write faster than B+Trees?** B+Tree writes are random seeks to the specific leaf page on disk. LSM trees only ever append sequentially (MemTable + commit log), deferring the expensive sorted-merge work to background compaction.
- **2PC's fatal flaw?** It's a **blocking** protocol — if the coordinator dies between collecting all commit votes and broadcasting the global commit, participants are stuck holding locks indefinitely with no way to know the outcome. Modern systems replace it with Raft/Paxos-based consensus.
- **Consistent hashing's one advantage over `hash % N`?** Adding/removing a node only remaps ~$1/N$ of keys instead of nearly all of them, avoiding a full cluster data migration on every resize.
- **Why prefer `NOT EXISTS` over `NOT IN` for a subquery that might return NULLs?** `NOT IN` against a result set containing even one `NULL` returns **zero rows** for the whole query (three-valued logic poisoning) — `NOT EXISTS` has no such trap.
- **Why does `WHERE YEAR(date) = 2026` skip the index on `date`?** Wrapping an indexed column in a function makes the predicate non-sargable — the optimizer can't map it to a B+Tree range scan, so it falls back to a full scan. Rewrite as an explicit date range instead.
