# Lecture 9: Normalization in DBMS — Part 1 (Functional Dependencies, 1NF, 2NF)

## 1. Why Normalization?

Even after correctly mapping an ER diagram to tables, a schema can still have poorly designed tables that mix unrelated facts together, causing three categories of problems, collectively called **anomalies**.

## 2. Anomalies in Unnormalized Tables

Consider a single flat table: `StudentCourse(roll_no, student_name, course_id, course_name, instructor)`.

1. **Insertion Anomaly**: Can't add a new course to the catalog until at least one student enrolls in it, because `course_name`/`instructor` only exist alongside a `roll_no`.
2. **Deletion Anomaly**: If the last student enrolled in a course drops it, deleting that row also accidentally deletes all information about the course itself.
3. **Update Anomaly**: If an instructor's name changes, you must update it in *every single row* where that course appears — miss one, and the data becomes inconsistent.

**Normalization** is the systematic process of decomposing tables to eliminate these anomalies by minimizing redundancy, guided by **functional dependencies**.

## 3. Functional Dependencies (FDs)

A functional dependency `X → Y` ("X determines Y") means: for any two tuples, if they agree on the values of attribute set `X`, they must also agree on the values of attribute set `Y`. `X` is called the **determinant**.

*Example*: `roll_no → student_name` — knowing the roll number fully determines the student's name.

### Types of FDs
1. **Trivial FD**: `X → Y` where `Y` is a subset of `X` (e.g., `{roll_no, name} → name`). Always true, adds no information.
2. **Non-trivial FD**: `Y` is NOT a subset of `X`.
3. **Full Functional Dependency**: `Y` is functionally dependent on the *entire* composite key `X`, not on any proper subset of it.
4. **Partial Functional Dependency**: `Y` depends on only **part** of a composite key `X`, not the whole thing. This is the root cause that 2NF eliminates.
5. **Transitive Functional Dependency**: `X → Y` and `Y → Z` implies `X → Z`, where `Y` is not a key. This is the root cause that 3NF eliminates (Lecture 10).

### Armstrong's Axioms (Inference Rules for FDs)

Given a set of FDs, these axioms let us derive all other FDs implied by them (the "closure"):
1. **Reflexivity**: If `Y ⊆ X`, then `X → Y`.
2. **Augmentation**: If `X → Y`, then `XZ → YZ` for any `Z`.
3. **Transitivity**: If `X → Y` and `Y → Z`, then `X → Z`.

Derived/secondary rules (provable from the above): Union, Decomposition, Pseudo-transitivity.

## 4. Normal Forms — The General Idea

Normal forms are a series of **increasingly strict rules**. A table satisfies a higher normal form only if it already satisfies all lower ones. Progressing through normal forms is called **normalization**; each step decomposes a table into smaller tables joined via foreign keys.

## 5. First Normal Form (1NF)

A relation is in 1NF if:
1. Every attribute contains only **atomic (indivisible) values** — no multi-valued or composite attributes stored directly in a cell.
2. Each column contains values of a **single type**.
3. Each row is **unique** (has a primary key).
4. The order of rows/columns doesn't matter.

*Violation example*: `Student(roll_no, name, phone_numbers)` where `phone_numbers = "9876543210, 9123456789"` in one cell — violates atomicity.

*Fix*: Split into a separate table `Student_Phone(roll_no, phone_number)` with one phone number per row (same pattern as Rule 4 in Lecture 8's ER mapping).

## 6. Second Normal Form (2NF)

A relation is in 2NF if:
1. It is already in 1NF.
2. It has **no partial functional dependency** — i.e., every non-key attribute is fully functionally dependent on the **entire** primary key (relevant only when the primary key is composite).

*Violation example*: `Enrollment(roll_no, course_id, student_name, grade)` with composite PK `(roll_no, course_id)`.
- `student_name` depends only on `roll_no` (part of the key) — **partial dependency**, violates 2NF.
- `grade` depends on the full `(roll_no, course_id)` — fine.

*Fix — decompose*:
- `Student(roll_no, student_name)`
- `Enrollment(roll_no, course_id, grade)`

Now `student_name` is fully dependent on the (now single-attribute) key of its own table, removing the partial dependency.

### Note
If a table's primary key is a **single attribute** (not composite), it is automatically in 2NF as long as it's already in 1NF, since partial dependency can only occur relative to a composite key.

## Summary
Normalization fixes insertion, deletion, and update anomalies caused by mixing unrelated facts in one table, using functional dependencies as the formal tool to detect redundancy. **1NF** enforces atomic values. **2NF** builds on 1NF by removing partial dependencies — non-key attributes that depend on only part of a composite primary key — by splitting them into their own table. The next lecture (Part 2) continues with 3NF, BCNF, and lossless decomposition to remove transitive dependencies and stricter anomalies.

## 7. Closure of a Set of Attributes

The **closure** of an attribute set `X` (denoted `X+`) is the set of all attributes that are functionally determined by `X`, given a set of FDs — computed by repeatedly applying Armstrong's axioms. Closure is used to:
- Determine whether a given attribute set is a candidate key (if `X+` = all attributes of the relation, and no subset of X also achieves this, X is a candidate key).
- Test whether a specific FD is implied by a given set of FDs.

## 8. Extraneous Attributes and Minimal Cover

A set of FDs can often be simplified into an equivalent, minimal set called the **canonical cover** (or minimal cover) — removing redundant FDs and extraneous attributes from the left-hand side of FDs — useful for simplifying normalization and BCNF/3NF decomposition algorithms.

## 9. Practice Example — Identifying FD Violations for 2NF

Given `Order_Details(order_id, product_id, product_name, quantity)` with composite PK `(order_id, product_id)`:
- `product_id → product_name` (product_name depends only on product_id, a **part** of the key) → **partial dependency**, violates 2NF.
- `quantity` depends on the full `(order_id, product_id)` combination → fine.

**Fix**: Decompose into `Product(product_id PK, product_name)` and `Order_Details(order_id, product_id, quantity)`.

---

## Placement Interview Questions — Lecture 9

**Q1. What is normalization, and why is it needed?**
Normalization is the systematic process of decomposing tables to eliminate redundancy and anomalies (insertion, deletion, update), guided by functional dependencies, resulting in a set of well-structured, non-redundant relations.

**Q2. Explain insertion, deletion, and update anomalies with examples.**
Insertion anomaly: can't add a course without an enrolled student. Deletion anomaly: deleting the last student in a course accidentally deletes the course's info too. Update anomaly: changing an instructor's name requires updating every row where that course appears, risking inconsistency if missed.

**Q3. What is a functional dependency? Give an example.**
X → Y means: for any two tuples agreeing on X, they must also agree on Y. E.g., `roll_no → name` — the roll number determines the student's name.

**Q4. Differentiate between partial and transitive functional dependency.**
Partial: a non-key attribute depends on only part of a composite primary key (violates 2NF). Transitive: a non-key attribute depends on another non-key attribute, which in turn depends on the primary key (violates 3NF).

**Q5. State Armstrong's Axioms.**
Reflexivity: if Y ⊆ X, then X → Y. Augmentation: if X → Y, then XZ → YZ. Transitivity: if X → Y and Y → Z, then X → Z.

**Q6. What is the condition for a relation to be in 1NF?**
Every attribute must contain only atomic (indivisible) values, each column has a single data type, and every row is unique (has a primary key).

**Q7. What is the condition for a relation to be in 2NF?**
The relation must be in 1NF, and there must be no partial functional dependency — every non-key attribute must be fully functionally dependent on the entire (potentially composite) primary key.

**Q8. If a table's primary key is a single (non-composite) attribute, is it automatically in 2NF? Why?**
Yes, as long as it's already in 1NF — because partial dependency can only occur relative to a composite key; with a single-attribute key, there's no "part of the key" to be partially dependent on.

**Q9. What is the closure of an attribute set, and what is it used for?**
The set of all attributes functionally determined by a given attribute set, computed via Armstrong's axioms — used to test candidate keys and verify whether specific FDs are implied by a given FD set.

**Q10. Give an example of converting an unnormalized table (violating 1NF) into 1NF.**
`Student(roll_no, name, phones="98765,91234")` (multi-valued in one cell) violates 1NF. Fix: split into `Student(roll_no, name)` and `Student_Phone(roll_no FK, phone)`, one phone number per row.
