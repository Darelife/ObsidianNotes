# Lecture 8: Transform ER Model to Relational Model || ER-Diagram to Tables

This lecture provides the systematic rule set for converting an ER (or Extended ER) diagram into a set of relational tables — turning the conceptual design (Lectures 3–6) into an implementable schema (Lecture 7).

## Rule 1: Strong Entity → Table

Every strong (regular) entity becomes a table. All simple/single-valued attributes become columns. The entity's key attribute becomes the table's primary key.

*Example*: `Student(roll_no, name, dob)` → `roll_no` is PK.

## Rule 2: Weak Entity → Table with Composite Key

A weak entity becomes a table, but its primary key is a **composite** of its own partial key (discriminator) plus the primary key of its owning (strong) entity, which is also added as a foreign key.

*Example*: `Room` (weak entity, partial key `room_no`) owned by `Building` (PK `building_id`) →
`Room(building_id [FK, part of PK], room_no [part of PK], ...)` — composite primary key `(building_id, room_no)`.

## Rule 3: Composite Attribute → Flattened Columns

A composite attribute's sub-components become individual columns; the composite attribute itself is not stored directly.

*Example*: `Address (street, city, pincode)` → columns `street`, `city`, `pincode` directly in the table.

## Rule 4: Multi-valued Attribute → Separate Table

Since a relational cell can only hold atomic values, a multi-valued attribute becomes its **own table**, with a foreign key referencing the owning entity's primary key, and the composite of (FK, value) typically forms the new table's primary key.

*Example*: `Student` has multi-valued `phone_numbers` → new table `Student_Phone(roll_no [FK], phone_number)`, PK = `(roll_no, phone_number)`.

## Rule 5: Derived Attribute → Not Stored (usually)

Derived attributes are typically **not stored** as columns; they're computed at query time (e.g., via a computed column, view, or application logic) from the attributes they depend on. (Sometimes stored for performance, with the tradeoff of needing to keep it in sync.)

## Rule 6: Binary 1:1 Relationship → Merge or Foreign Key

Three options, chosen based on participation constraints:
- If one side has **total participation**, place the foreign key on that side's table (referencing the other's primary key) — minimizes NULLs.
- If both sides have total participation, the two tables can often be **merged** into a single table.
- If neither side has total participation, a foreign key with NULLs allowed works, or a separate table with a unique FK.

*Example*: `Employee` 1:1 `Passport` — if every Employee has total participation but not every Passport necessarily links back, put `employee_id` as FK+unique in `Passport`.

## Rule 7: Binary 1:N Relationship → Foreign Key on the "Many" Side

The primary key of the "1" side entity is added as a **foreign key** to the "N" side entity's table. No separate relationship table needed.

*Example*: `Department` (1) — `Employee` (N) → add `dept_id [FK]` column to `Employee` table.

## Rule 8: Binary M:N Relationship → New Junction/Bridge Table

A new table is created for the relationship itself, containing foreign keys to **both** participating entities' primary keys. The combination of these two FKs usually forms the composite primary key of the junction table. Any relationship attributes also go into this new table.

*Example*: `Student` M:N `Course` via `Enrolls` (attribute: `grade`) →
`Enrollment(roll_no [FK], course_id [FK], grade)`, PK = `(roll_no, course_id)`.

## Rule 9: Ternary (or n-ary) Relationship → New Table with FKs to All Participants

Similar to M:N, but the junction table has a foreign key to each of the n participating entities.

*Example*: `Doctor`–`Patient`–`Medicine` (`Prescribes`) → `Prescription(doctor_id [FK], patient_id [FK], medicine_id [FK], dosage)`.

## Rule 10: Specialization/Generalization (Extended ER) → Multiple Mapping Strategies

Three common strategies exist for mapping an is-a hierarchy:

1. **Single Table (Table-per-Hierarchy)**: One table for the entire hierarchy, with all subclass-specific attributes included (nullable) plus a "type" discriminator column. Simple, but wastes space with NULLs.
2. **Table per Subclass (Class Table Inheritance)**: One table for the superclass (common attributes) plus one table per subclass (subclass-specific attributes + FK referencing the superclass's PK, which is also the subclass table's PK). Reduces redundancy, requires joins to reconstruct a full subclass record.
3. **Table per Concrete Class**: A separate, fully self-contained table per subclass, duplicating the superclass's common attributes in each. Avoids joins, but duplicates common columns and complicates queries across the whole hierarchy.

*Example (Table per Subclass)*: `Employee(emp_id PK, name, salary)`, `Manager(emp_id PK/FK, bonus)`, `Engineer(emp_id PK/FK, specialization)`.

## Rule 11: Aggregation → Treat the Aggregated Relationship as an Entity

The aggregated relationship set is converted first per normal rules (usually becoming a junction table, per Rule 8). Then the outer relationship connecting to that aggregation is mapped by adding a foreign key from that junction table to the new participant, just as with any other relationship.

## Quick Reference Table

| ER Construct | Relational Mapping |
|---|---|
| Strong entity | Table, own PK |
| Weak entity | Table, composite PK (owner PK + partial key) |
| 1:1 relationship | FK on total-participation side, or merge |
| 1:N relationship | FK on "N" side |
| M:N relationship | New junction table with FKs to both sides |
| Multi-valued attribute | New table, FK to owner |
| Composite attribute | Flattened into separate columns |
| Derived attribute | Usually not stored |
| Specialization | Single table / table-per-subclass / table-per-concrete-class |

## Summary
Translating an ER diagram into relational tables follows a consistent rule set: entities become tables, 1:N relationships become foreign keys on the "many" side, M:N relationships become new junction tables, weak entities get composite keys tied to their owner, and multi-valued/composite attributes get unpacked into their own tables/columns. Extended ER constructs like specialization require choosing among three mapping strategies depending on the tradeoff between storage efficiency and query simplicity desired.

## 12. Mapping Recursive (Self-Referencing) Relationships

For a unary/recursive relationship (e.g., `Employee` manages `Employee`), the mapping follows the same cardinality-based rules as binary relationships, but the foreign key references the **same table**:
- **1:N recursive** (one manager, many subordinates): add a `manager_id [FK]` column to `Employee` referencing `Employee.emp_id`.
- **M:N recursive** (e.g., Friends_With): create a junction table with two FKs, both referencing the same table, e.g., `Friendship(user_id_1 [FK], user_id_2 [FK])`.

## 13. Mapping Total Participation → NOT NULL Constraint

When mapping a 1:N relationship where the "many" side has **total participation**, the foreign key column on that side should be marked `NOT NULL`, since every row on that side must be associated with a parent row.

## 14. Worked Example — Full Mini Schema

**ER Design**: `Student (roll_no PK, name)` — `Enrolls (M:N, attr: grade)` — `Course (course_id PK, title)`; `Course` taught by `Professor (1:N, total on Course side)`.

**Relational Mapping**:
```sql
Student(roll_no PK, name)
Professor(emp_id PK, name)
Course(course_id PK, title, emp_id FK NOT NULL REFERENCES Professor)
Enrollment(roll_no FK REFERENCES Student, course_id FK REFERENCES Course, grade,
           PRIMARY KEY(roll_no, course_id))
```

---

## Placement Interview Questions — Lecture 8

**Q1. How is a strong entity mapped to a relational table?**
It becomes a table directly; simple/single-valued attributes become columns, and the entity's key attribute becomes the table's primary key.

**Q2. How is a weak entity mapped to a relational table?**
It becomes a table whose primary key is a composite of its own partial key (discriminator) plus the primary key of its owning strong entity (also added as a foreign key).

**Q3. How do you map a 1:N relationship to relational tables?**
Add the primary key of the "1" side as a foreign key on the "N" side's table — no separate table is needed.

**Q4. How do you map an M:N relationship to relational tables?**
Create a new junction/bridge table containing foreign keys to both participating entities' primary keys (their combination usually forms the composite primary key), plus any relationship-specific attributes.

**Q5. How do you decide where to place the foreign key in a 1:1 relationship?**
Place it on the side with total participation (to minimize NULLs); if both sides have total participation, the tables can often be merged; if neither has total participation, a nullable FK or separate table with a unique FK works.

**Q6. How is a multi-valued attribute mapped to the relational model?**
It becomes its own table with a foreign key referencing the owning entity's primary key; the combination of (FK, value) typically forms this new table's primary key.

**Q7. What are the three strategies to map a specialization/generalization hierarchy? What are their trade-offs?**
(1) Single table with a discriminator column — simple but wastes space with NULLs. (2) Table per subclass — normalized, requires joins to reconstruct full records. (3) Table per concrete class — avoids joins but duplicates common columns across tables.

**Q8. How would you map a ternary relationship to relational tables?**
Create a new table with foreign keys referencing all three (or n) participating entities' primary keys, plus any relationship attributes.

**Q9. How do you handle composite and derived attributes during relational mapping?**
Composite attributes are "flattened" into their individual sub-component columns. Derived attributes are usually not stored at all — computed at query time from the attributes they depend on (unless stored for performance, requiring extra sync logic).

**Q10. How would you map a self-referencing (recursive) M:N relationship like "Employee reports to Employee" if it were M:N instead of 1:N?**
Create a junction table with two foreign keys, both referencing the same Employee table's primary key (e.g., `Reports_To(subordinate_id FK, manager_id FK)`), similar to how Friends_With is mapped for a symmetric case.
