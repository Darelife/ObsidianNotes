# Lecture 7: Relational Model in DBMS

## 1. What is the Relational Model?

Proposed by **E. F. Codd** in 1970, the Relational Model represents data as a collection of **relations** (tables), where each relation is a set of **tuples** (rows). It is the most widely used data model in commercial DBMS today (MySQL, PostgreSQL, Oracle, SQL Server).

## 2. Core Terminology

| Relational Term | Informal Equivalent |
|---|---|
| Relation | Table |
| Tuple | Row / Record |
| Attribute | Column / Field |
| Domain | Set of allowable values for an attribute |
| Degree | Number of attributes (columns) in a relation |
| Cardinality | Number of tuples (rows) in a relation |

### a. Relation
A relation `R` is formally a subset of the Cartesian product of the domains of its attributes: `R ⊆ D1 × D2 × ... × Dn`. Practically, it's a table with a fixed set of named columns and any number of rows.

### b. Tuple
A single row of a relation representing one entity instance, e.g., `(101, 'Rahul', 'CS')`.

### c. Attribute and Domain
An **attribute** is a named column; its **domain** is the set of legal values it can take (e.g., domain of `age` might be positive integers between 0–120).

## 3. Properties of Relations

1. Each cell holds a single **atomic** value (no repeating groups) — this is a foundational assumption tying directly into 1NF (Lecture 9).
2. Each attribute has a **distinct name** within a relation.
3. The order of tuples is **irrelevant** (a relation is a set, not a sequence).
4. The order of attributes is (conceptually) **irrelevant**.
5. **No two tuples are identical** — every relation must have at least one key.

## 4. Keys in the Relational Model

### a. Super Key
Any attribute (or set of attributes) that can uniquely identify a tuple in a relation. A relation can have many super keys, including the full set of all its attributes.

### b. Candidate Key
A **minimal** super key — no attribute can be removed from it without losing the uniqueness property. A relation may have multiple candidate keys.
*Example*: In a `Student` table with `(roll_no, email, name)`, both `roll_no` and `email` might independently be candidate keys if both are guaranteed unique.

### c. Primary Key
The candidate key **selected by the designer** to be the principal means of identifying tuples. Cannot contain NULL values (entity integrity rule).

### d. Alternate Key
Candidate keys that are **not** chosen as the primary key.

### e. Foreign Key
An attribute (or set) in one relation that references the **primary key** of another (or the same) relation, used to represent relationships between relations.

### f. Composite Key
A candidate/primary key made up of **two or more attributes** together (neither attribute alone is unique).

## 5. Integrity Constraints

1. **Domain Constraint** — every value of an attribute must be from its defined domain (correct data type/range).
2. **Entity Integrity Constraint** — no attribute participating in the primary key can be NULL, since NULL primary key values would fail to uniquely identify a tuple.
3. **Referential Integrity Constraint** — a foreign key value must either match an existing primary key value in the referenced relation, or be entirely NULL. This ensures relationships between tables remain consistent (no "dangling references").
4. **Key Constraint** — enforces uniqueness of candidate/primary key values across all tuples.

## 6. Relational Algebra (Brief Preview)

The relational model comes paired with **relational algebra**, a procedural query language of operations like:
- **Selection (σ)** — filters rows based on a condition.
- **Projection (π)** — selects specific columns.
- **Union, Intersection, Difference** — set operations between compatible relations.
- **Cartesian Product (×)** — combines every tuple of one relation with every tuple of another.
- **Join (⋈)** — combines related tuples from two relations based on a common attribute (used heavily to reconstruct relationships defined via foreign keys).

## 7. Why the Relational Model Won

- **Simplicity**: Tables are intuitive and map naturally to how humans think about structured data.
- **Strong mathematical foundation**: rooted in set theory and predicate logic, enabling formal query optimization.
- **Declarative querying**: SQL lets users specify *what* they want, not *how* to get it — the DBMS's query optimizer figures out the execution plan.
- **Data independence**: Changes to physical storage structures don't require changes to how relations are queried (as discussed in Lecture 2).

## Summary
The relational model organizes data into tables (relations) made of rows (tuples) and columns (attributes), governed by a system of keys — super, candidate, primary, foreign — and enforced by integrity constraints (domain, entity, referential). It is the destination format that ER diagrams get translated into, and the topic of the very next lecture is exactly that translation process.

## 8. Relational Model Constraints Recap Table

| Constraint | Rule |
|---|---|
| Domain | Attribute values must come from a defined, valid domain |
| Entity Integrity | Primary key attributes cannot be NULL |
| Referential Integrity | Foreign key must match an existing referenced primary key, or be NULL |
| Key Constraint | No two tuples can have the same primary key value |

## 9. NULL Values in the Relational Model

A NULL represents "unknown," "not applicable," or "missing" data. Special handling rules:
- Any arithmetic operation involving NULL yields NULL.
- Comparisons with NULL using `=` or `<>` yield UNKNOWN (not TRUE/FALSE) — this is why SQL uses `IS NULL` / `IS NOT NULL` instead of `= NULL`.
- Aggregate functions (except `COUNT(*)`) generally ignore NULL values.

## 10. Relational Algebra Operations — Slightly Expanded

- **Selection (σ)**: `σ_condition(R)` — filters rows.
- **Projection (π)**: `π_columns(R)` — selects columns, and by definition of a relation being a *set*, automatically removes duplicate rows.
- **Union (∪), Intersection (∩), Set Difference (−)**: require both relations to be **union-compatible** (same number of attributes, matching domains).
- **Cartesian Product (×)**: combines every row of R1 with every row of R2 — degree adds, cardinality multiplies.
- **Join (⋈)**: a Cartesian product followed by a selection condition — types include Theta Join, Equi-Join, and Natural Join (which additionally removes duplicate columns).
- **Rename (ρ)**: renames a relation or its attributes.

---

## Placement Interview Questions — Lecture 7

**Q1. Define relation, tuple, attribute, domain, degree, and cardinality.**
Relation = table. Tuple = row. Attribute = column. Domain = set of allowed values for an attribute. Degree = number of attributes (columns). Cardinality = number of tuples (rows).

**Q2. Differentiate between super key, candidate key, primary key, and alternate key.**
Super key: any attribute set that uniquely identifies a tuple (may have redundant attributes). Candidate key: a minimal super key. Primary key: the candidate key chosen for actual identification. Alternate key: candidate keys not chosen as primary.

**Q3. What is a foreign key, and what purpose does it serve?**
An attribute (or set) in one table that references the primary key of another (or the same) table, used to represent and enforce relationships between tables.

**Q4. Explain the four types of integrity constraints in the relational model.**
Domain constraint (valid value ranges), Entity integrity (PK can't be NULL), Referential integrity (FK must match an existing PK or be NULL), Key constraint (uniqueness of key values).

**Q5. Can a table have multiple candidate keys but only one primary key? Explain with an example.**
Yes. E.g., a Student table might have both `roll_no` and `email` as unique, independently-qualifying candidate keys, but only one (say `roll_no`) is chosen as the primary key; `email` becomes an alternate key.

**Q6. Why can't a primary key contain NULL values?**
Because the primary key's purpose is to uniquely identify each tuple; a NULL value provides no identifying information and could theoretically match multiple "unknown" tuples, violating the entity integrity constraint.

**Q7. What happens to referential integrity when you try to delete a row referenced by a foreign key elsewhere?**
By default, most DBMSs reject the delete (or, depending on defined behavior, cascade the delete, set the FK to NULL, or set it to a default value) — controlled via `ON DELETE CASCADE`, `SET NULL`, `RESTRICT`, etc.

**Q8. What's the difference between a composite key and a compound/composite attribute?**
A composite key is a primary/candidate key made of two or more attributes together (relational model concept). A composite attribute is an ER-model attribute that can be broken into sub-parts (e.g., Address). They are unrelated concepts despite similar naming.

**Q9. Explain the basic relational algebra operations.**
Selection (σ) filters rows; Projection (π) selects columns; Union/Intersection/Difference are set operations; Cartesian Product (×) combines all row pairs; Join (⋈) combines related rows based on a condition.

**Q10. Why does comparing a value to NULL using `=` not work as expected in SQL?**
Because NULL represents "unknown," not a comparable value — any comparison with NULL evaluates to UNKNOWN, not TRUE or FALSE, so `column = NULL` never returns rows. `IS NULL` must be used instead.
