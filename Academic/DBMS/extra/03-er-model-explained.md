# Lecture 3: ER Model Explained || ER Diagram Notations

## 1. What is the ER Model?

The **Entity-Relationship (ER) Model** is a high-level conceptual data model used to describe the data and relationships of a specific domain, independent of any particular DBMS. It is primarily a **design tool**, used before creating actual database tables, to visually map out what data needs to be stored and how different pieces of data relate to each other.

## 2. Building Blocks of the ER Model

### a. Entity
An entity is a real-world object (or concept) that is distinguishable from other objects and about which we want to store data. E.g., `Student`, `Course`, `Employee`.

- **Entity Set**: A collection of entities of the same type sharing the same properties. E.g., all students form the `Student` entity set.
- Represented as a **rectangle** in ER diagrams.

### b. Attribute
An attribute is a property or characteristic of an entity. E.g., a `Student` entity has attributes `roll_no`, `name`, `dob`.

Represented as an **oval/ellipse** connected to its entity.

**Types of attributes:**
1. **Simple (Atomic) Attribute** — cannot be divided further (e.g., `age`).
2. **Composite Attribute** — can be divided into sub-parts (e.g., `Address` → `street`, `city`, `pincode`).
3. **Single-valued Attribute** — has only one value for an entity (e.g., `roll_no`).
4. **Multi-valued Attribute** — can have multiple values (e.g., `phone_numbers`). Represented with a **double oval**.
5. **Derived Attribute** — value can be derived from other attributes (e.g., `age` derived from `date_of_birth`). Represented with a **dashed oval**.
6. **Key Attribute** — uniquely identifies each entity instance (e.g., `roll_no`). Represented with an **underlined** label.

### c. Relationship
A relationship describes an association among two or more entities. E.g., a `Student` *enrolls in* a `Course`.

- **Relationship Set**: A collection of similar relationships.
- Represented as a **diamond** connecting related entities.

**Degree of a relationship** — number of entity sets participating:
- **Unary (Degree 1)**: A single entity relates to itself (e.g., an `Employee` *manages* another `Employee`).
- **Binary (Degree 2)**: Two entities involved (most common, e.g., `Student`–`Course`).
- **Ternary (Degree 3)**: Three entities involved (e.g., `Doctor`–`Patient`–`Medicine`).

## 3. Structural Constraints — Cardinality Ratios

Cardinality defines the number of instances of one entity that can/must be associated with instances of another entity via a relationship.

1. **One-to-One (1:1)** — One instance of entity A relates to exactly one instance of entity B.
   *Example*: A `Person` has one `Passport`.

2. **One-to-Many (1:N)** — One instance of A relates to many instances of B, but each instance of B relates to only one instance of A.
   *Example*: One `Department` has many `Employees`, but each `Employee` belongs to one `Department`.

3. **Many-to-One (N:1)** — The reverse view of 1:N.

4. **Many-to-Many (M:N)** — Multiple instances of A relate to multiple instances of B.
   *Example*: A `Student` can enroll in many `Courses`, and a `Course` can have many `Students`.

## 4. Participation Constraints

Participation describes whether *every* entity instance must take part in the relationship.

1. **Total Participation** — every entity in the entity set MUST participate in the relationship. Represented by a **double line** connecting entity to relationship.
   *Example*: Every `Employee` must belong to some `Department` (total participation of Employee).

2. **Partial Participation** — participation is optional; some entities may not be involved in the relationship. Represented by a **single line**.
   *Example*: Not every `Employee` manages a project (partial participation).

## 5. Keys in ER Model
- **Super Key**: Any set of attributes that uniquely identifies an entity.
- **Candidate Key**: A minimal super key (no redundant attributes).
- **Primary Key**: The candidate key chosen to uniquely identify entities in practice.

## 6. Notations Summary Table

| Symbol | Meaning |
|---|---|
| Rectangle | Entity |
| Oval | Attribute |
| Double Oval | Multi-valued attribute |
| Dashed Oval | Derived attribute |
| Underlined text | Key attribute |
| Diamond | Relationship |
| Lines | Connects entities/attributes to relationships |
| Double line | Total participation |
| 1, N, M labels near lines | Cardinality ratio |

## Summary
The ER model provides a structured vocabulary — entities, attributes, and relationships — to capture what a database needs to represent before any tables are created. Cardinality (1:1, 1:N, M:N) and participation constraints (total vs. partial) capture the business rules governing how entities relate. This model is intentionally implementation-agnostic; it will later be systematically translated into relational tables (covered in Lecture 8).

## 7. Strong vs. Weak Entities (Preview — Detailed in Lecture 5)

- **Strong Entity**: Has a primary key attribute of its own and can be uniquely identified independently.
- **Weak Entity**: Cannot be uniquely identified by its own attributes alone; depends on a related **strong (owner) entity** for identification. Represented with a **double rectangle**, connected to its owner via an **identifying relationship** (double diamond). It has a **partial key/discriminator** instead of a full primary key.

## 8. Roles in a Relationship

When an entity set participates more than once in the same relationship (recursive relationship), each participation is labeled with a distinct **role name** to disambiguate. E.g., in an `Employee` "Supervises" `Employee` relationship, one role is "Supervisor" and the other is "Subordinate."

## 9. Total vs. Partial Participation — Additional Notes

Participation constraints directly affect whether a **NOT NULL** constraint should be applied to the foreign key during relational mapping (Lecture 8) — total participation typically implies NOT NULL on the foreign key column.

---

## Placement Interview Questions — Lecture 3

**Q1. What is the difference between an entity and an entity set?**
An entity is a single real-world object instance (e.g., one specific student). An entity set is the collection of all entities of the same type sharing the same attributes (e.g., all students).

**Q2. Differentiate between simple, composite, derived, and multi-valued attributes with examples.**
Simple: atomic, can't be divided (e.g., `age`). Composite: divisible into sub-parts (e.g., `Address` → street, city). Derived: computed from other attributes (e.g., `age` from `dob`). Multi-valued: can hold multiple values (e.g., `phone_numbers`).

**Q3. What is a key attribute? What's the difference between a candidate key and a primary key at the ER level?**
A key attribute uniquely identifies each entity instance. Candidate key: any minimal attribute set that can serve as a unique identifier. Primary key: the specific candidate key chosen for actual identification.

**Q4. What are the different types of cardinality ratios? Give one real example of each.**
1:1 — Person↔Passport. 1:N — Department↔Employees. M:N — Student↔Course (enrollment).

**Q5. Explain total vs. partial participation with examples.**
Total: every entity instance MUST participate (e.g., every Employee must belong to a Department). Partial: participation is optional (e.g., not every Employee manages a Project).

**Q6. What is the degree of a relationship? Give examples of unary, binary, and ternary relationships.**
Degree = number of entity sets involved. Unary: Employee manages Employee (self-relationship). Binary: Student enrolls in Course. Ternary: Doctor prescribes Medicine to Patient.

**Q7. How do you represent a weak entity and its identifying relationship in an ER diagram?**
Weak entity: double rectangle. Identifying relationship: double diamond. It has a partial key (discriminator), underlined with a dashed line, rather than a full primary key.

**Q8. What's the difference between a super key and a candidate key?**
A super key is any attribute set that uniquely identifies a tuple (can include extra, redundant attributes). A candidate key is a *minimal* super key — no attribute can be removed without losing uniqueness.

**Q9. Why do we need role names in ER diagrams?**
When the same entity set participates multiple times in a single relationship (a recursive relationship), role names distinguish each participation's purpose (e.g., "Manager" vs. "Subordinate" roles for Employee in a Supervises relationship).

**Q10. How would you model a multi-valued attribute using notations?**
Using a double oval directly connected to the entity; during relational mapping, it becomes a separate table with a foreign key back to the owning entity.
