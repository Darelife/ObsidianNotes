# Lecture 10: Normalization in DBMS — Part 2 (3NF, BCNF, Lossless Decomposition)

Continuing from Part 1 (1NF, 2NF), this lecture covers the stricter normal forms that eliminate transitive dependencies, along with the formal guarantees needed when decomposing tables.

## 1. Third Normal Form (3NF)

A relation is in 3NF if:
1. It is already in 2NF.
2. For every non-trivial FD `X → Y`, at least one of these holds:
   - `X` is a **super key** of the relation, **OR**
   - `Y` is part of some **candidate key** (i.e., `Y` is a "prime attribute").

In simpler terms: 3NF eliminates **transitive dependencies** of non-prime attributes on the primary key.

*Violation example*: `Employee(emp_id, emp_name, dept_id, dept_name)` with PK `emp_id`.
- `emp_id → dept_id` (direct)
- `dept_id → dept_name` (direct)
- Therefore `emp_id → dept_name` transitively, through `dept_id`, which is **not** a key. This is a transitive dependency — violates 3NF.

*Fix — decompose*:
- `Employee(emp_id, emp_name, dept_id)`
- `Department(dept_id, dept_name)`

Now `dept_name` depends directly on the key of its own table (`dept_id`), with no transitive chain.

## 2. Boyce-Codd Normal Form (BCNF)

BCNF is a **stricter version of 3NF**. A relation is in BCNF if:
For every non-trivial FD `X → Y`, `X` **must** be a super key. (Note: BCNF drops the "or Y is prime" exception that 3NF allows.)

Every relation in BCNF is automatically in 3NF, but not vice versa — some relations satisfy 3NF's relaxed exception while still having anomalies that only BCNF removes.

*Classic violation example*: `Course_Instructor(student_id, course_id, instructor)` where the business rule is: each course is taught by exactly one instructor, but an instructor may teach multiple courses, and a student can be assigned to a course from multiple possible instructors is NOT the case here — instead consider:

`StudentAdvisor(student_id, subject, advisor)` with the rule: each `(student_id, subject)` pair determines exactly one `advisor`, but also, each `advisor` only ever teaches one specific `subject`, so `advisor → subject`.

- Candidate key: `(student_id, subject)`.
- FD: `advisor → subject`. Here `advisor` is **not** a super key, and `subject` **is** a prime attribute (part of the candidate key) — so this actually satisfies 3NF's exception clause but **violates BCNF**, because the determinant `advisor` isn't a super key.

*Fix — decompose*:
- `AdvisorSubject(advisor, subject)`
- `StudentAdvisor(student_id, advisor)`

### 3NF vs. BCNF — Practical Tradeoff
BCNF removes more redundancy than 3NF but **cannot always be achieved without losing a functional dependency** (i.e., sometimes decomposing to BCNF makes it impossible to enforce a business rule directly via the schema without a join). 3NF always allows a **dependency-preserving** decomposition; BCNF does not guarantee this. In practice, 3NF is often considered "good enough" and is the most commonly targeted normal form in industry schema design.

## 3. Lossless (Non-Additive) Join Decomposition

When we decompose a relation `R` into `R1` and `R2`, we must ensure that joining `R1` and `R2` back together (via natural join) reproduces **exactly** the original relation `R` — no spurious/extra rows, and no lost rows. This property is called a **lossless-join decomposition**.

### Condition for Lossless Decomposition
A decomposition of `R` into `R1` and `R2` is lossless if and only if:
```
(R1 ∩ R2) → R1   OR   (R1 ∩ R2) → R2
```
That is, the common attribute(s) between `R1` and `R2` must functionally determine **all** attributes of at least one of the two decomposed relations.

*Why it matters*: If this condition fails, joining `R1 ⋈ R2` back can produce spurious tuples that never existed in the original data — a **lossy decomposition** — which corrupts query results.

## 4. Dependency Preservation

A decomposition is **dependency-preserving** if the union of the functional dependencies enforceable on each of the decomposed relations is equivalent to the functional dependencies that held on the original relation — meaning we can still check all original constraints **without needing to join tables back together**.

- 3NF decomposition algorithms guarantee **both** lossless-join and dependency-preservation.
- BCNF decomposition algorithms guarantee lossless-join but **may sacrifice** dependency preservation.

## 5. Summary Comparison Table

| Normal Form | Removes | Guarantee |
|---|---|---|
| 1NF | Non-atomic values | — |
| 2NF | Partial dependency (on composite key) | — |
| 3NF | Transitive dependency | Lossless + dependency-preserving achievable |
| BCNF | All non-super-key determinants | Lossless achievable, but not always dependency-preserving |

## Summary
3NF eliminates transitive dependencies of non-key attributes through other non-key attributes, while BCNF tightens this further by requiring **every** determinant of a non-trivial FD to be a super key, with no exceptions. Achieving BCNF sometimes sacrifices dependency preservation, which is why 3NF remains the practical sweet spot in most schema designs. Any decomposition, regardless of the target normal form, must satisfy the **lossless-join property** — verified by checking that the common attributes between decomposed relations functionally determine at least one of them — to guarantee that no data is lost or fabricated when tables are joined back together.

## 6. Higher Normal Forms (Brief Mention — Beyond BCNF)

Though rarely required in practice/interviews beyond definitions, it's worth knowing these exist:
- **4NF (Fourth Normal Form)**: Eliminates **multi-valued dependencies** — a relation is in 4NF if it's in BCNF and has no non-trivial multi-valued dependency (where two or more independent multi-valued facts about an entity are stored in the same table, causing redundancy).
- **5NF (Fifth Normal Form / Project-Join Normal Form)**: Eliminates redundancy caused by **join dependencies** that aren't implied by candidate keys — deals with cases where a relation can be losslessly decomposed into multiple smaller relations, but no smaller subset of them can reconstruct the original without all being joined together.

## 7. Denormalization — The Practical Counterpoint

**Denormalization** is the deliberate, controlled process of introducing some redundancy back into a normalized schema, typically to improve **read performance** by reducing the number of joins needed for frequently-run queries — common in data warehousing/OLAP systems and read-heavy production systems. It's a conscious trade-off: faster reads at the cost of more storage and more complex write-time consistency maintenance.

## 8. Step-by-Step Worked Normalization Example (1NF → BCNF)

**Original**: `Student_Course(roll_no, s_name, course_id, c_name, instructor, instructor_dept)`
FDs: `roll_no → s_name`; `course_id → c_name, instructor`; `instructor → instructor_dept`.

- **1NF**: Already atomic — OK.
- **2NF**: PK = `(roll_no, course_id)`. `s_name` depends only on `roll_no` (partial) → split out.
  → `Student(roll_no, s_name)`, `Course_Detail(roll_no, course_id, c_name, instructor, instructor_dept)`
- **3NF**: In `Course_Detail`, `instructor_dept` depends on `instructor`, not directly on the key → transitive dependency → split out.
  → `Course(course_id, c_name, instructor)`, `Instructor(instructor, instructor_dept)`, `Enrollment(roll_no, course_id)`
- **BCNF check**: In `Course`, is `course_id` a super key determining everything? Yes. Is `instructor` (if it also determines `c_name` in some business rule) a super key? If not, further decomposition may be needed depending on exact FDs.

---

## Placement Interview Questions — Lecture 10

**Q1. What is the condition for 3NF?**
For every non-trivial FD X → Y in the relation, either X must be a super key, or Y must be part of some candidate key (a prime attribute).

**Q2. What is the condition for BCNF? How does it differ from 3NF?**
For every non-trivial FD X → Y, X must be a super key — no exceptions. BCNF is stricter than 3NF because it removes the "Y is a prime attribute" exception that 3NF allows.

**Q3. Give an example of a relation that is in 3NF but not in BCNF.**
`StudentAdvisor(student_id, subject, advisor)` where `(student_id, subject)` is the candidate key, but `advisor → subject` also holds and `advisor` isn't a super key while `subject` is a prime attribute — satisfies 3NF's exception but violates BCNF.

**Q4. Why isn't BCNF always achievable while preserving all functional dependencies?**
Because decomposing a relation to satisfy BCNF's strict "every determinant must be a super key" rule can sometimes force splitting a table in a way that no single resulting table can enforce a particular original FD without a join — sacrificing dependency preservation.

**Q5. What is a lossless-join decomposition? State its formal condition.**
A decomposition where joining the decomposed relations back together (via natural join) reproduces exactly the original relation, with no spurious or missing rows. Condition: for decomposition of R into R1, R2 — `(R1 ∩ R2) → R1` or `(R1 ∩ R2) → R2` must hold.

**Q6. What is dependency preservation in decomposition?**
A decomposition is dependency-preserving if all original functional dependencies can still be verified/enforced without needing to join the decomposed tables back together.

**Q7. Which normal form guarantees both lossless-join and dependency-preserving decomposition — 3NF or BCNF?**
3NF decomposition algorithms guarantee both. BCNF decomposition guarantees lossless-join but may sacrifice dependency preservation.

**Q8. Why is 3NF often preferred over BCNF in real-world schema design?**
Because 3NF strikes a practical balance — it removes the most significant redundancy-causing dependencies (transitive) while still guaranteeing that all business rules (FDs) can be enforced directly by the schema without needing joins, which BCNF cannot always guarantee.

**Q9. What is denormalization, and when would you use it?**
Deliberately introducing controlled redundancy into a normalized schema to improve read performance (fewer joins), commonly used in read-heavy systems and data warehouses, at the cost of extra storage and write-time consistency complexity.

**Q10. Briefly describe what 4NF and 5NF address.**
4NF removes multi-valued dependencies (independent multi-valued facts stored redundantly in one table). 5NF (Project-Join Normal Form) removes redundancy from join dependencies not implied by candidate keys.
