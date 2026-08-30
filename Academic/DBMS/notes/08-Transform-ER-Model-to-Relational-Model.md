# Lecture 8: Transforming ER Models to Relational Schemas

---

## 1. Introduction to ER-to-Relational Mapping

Conceptual ER diagrams must be systematically translated into relational database tables (DDL schemas) before implementation. This translation follows formal, deterministic mapping rules to preserve entities, attributes, cardinalities, and structural integrity constraints.

---

## 2. Complete Mapping Rules Taxonomy

```
+-------------------------------------------------------------------------------+
| RULE 1: Strong Entity Sets          --> Independent Relational Table          |
| RULE 2: Weak Entity Sets            --> Table with Owner FK + Partial Key     |
| RULE 3: 1:1 Binary Relationship     --> FK placement (Total side preferred)   |
| RULE 4: 1:N Binary Relationship     --> Foreign Key on 'N' Side Table         |
| RULE 5: M:N Binary Relationship     --> Separate Junction Table (Composite PK)|
| RULE 6: Multivalued Attributes       --> Separate Table with Parent FK         |
| RULE 7: Composite Attributes         --> Flattened into Individual Columns     |
| RULE 8: EER Generalization (1:1/Single)--> 3 Translation Strategies           |
+-------------------------------------------------------------------------------+
```

---

## 3. Step-by-Step Translation Guidelines

### Rule 1: Mapping Strong Entity Sets
* Create a standalone table for every strong entity set.
* Include all simple single-valued attributes.
* Set the ER primary key attribute as the table's **Primary Key**.

```text
ER:  [ STUDENT ] ( Roll_No [PK], Name, Email )
SQL: STUDENT ( Roll_No INT PRIMARY KEY, Name VARCHAR(50), Email VARCHAR(100) )
```

---

### Rule 2: Mapping Weak Entity Sets
* Create a table for the weak entity set.
* Include all simple local attributes of the weak entity.
* Add the **Primary Key of the Identifying Owner Entity** as a Foreign Key column.
* Composite Primary Key of Weak Table = $(\text{Owner FK} + \text{Partial Key / Discriminator})$.

```text
ER:  [[ DEPENDENT ]] ( Name [Partial Key], Age ) dependent on [ EMPLOYEE ] ( SSN [PK] )
SQL: DEPENDENT (
        Emp_SSN INT,
        Dep_Name VARCHAR(50),
        Age INT,
        PRIMARY KEY (Emp_SSN, Dep_Name),
        FOREIGN KEY (Emp_SSN) REFERENCES EMPLOYEE(SSN) ON DELETE CASCADE
     )
```

---

### Rule 3: Mapping Binary 1:1 Relationships
Choose one of three strategies based on participation constraints:

1. **Total Participation on One Side (Recommended)**:
   * Place the Foreign Key in the table representing the entity set with **Total Participation**.
   * Add a `UNIQUE` constraint on the FK to enforce 1:1.
   * *Example*: `DEPARTMENT` (Total) has `MANAGER` (`EMPLOYEE`, Partial).
     * Place `Manager_SSN` inside `DEPARTMENT` table with `NOT NULL UNIQUE`.

2. **Partial Participation on Both Sides**:
   * Combine foreign keys into either table, or create a third lookup table.

3. **Total Participation on Both Sides**:
   * Merge both entity sets into a single unified relational table.

---

### Rule 4: Mapping Binary 1:N Relationships
* Identify the entity set on the **$N$-side** (many side).
* Include the Primary Key of the $1$-side entity set as a **Foreign Key** inside the $N$-side table.
* Include any relationship attributes inside the $N$-side table.

```text
ER:  [ DEPARTMENT ] ( 1 ) <--- Employs ---> ( N ) [ EMPLOYEE ]
SQL: EMPLOYEE (
        Emp_ID INT PRIMARY KEY,
        Emp_Name VARCHAR(50),
        Dept_ID INT NOT NULL, -- Foreign Key from 1-side (DEPARTMENT)
        FOREIGN KEY (Dept_ID) REFERENCES DEPARTMENT(Dept_ID)
     )
```

---

### Rule 5: Mapping Binary M:N Relationships
* Create a **new associative / junction table** (Relationship Table).
* Include Primary Keys of **both** participating entity sets as Foreign Key attributes in the new table.
* Primary Key of Junction Table = **Composite Key** $(\text{PK}_A, \text{PK}_B)$.
* Include any relationship attributes (e.g., `Grade`, `Date`) as columns in this junction table.

```text
ER:  [ STUDENT ] ( M ) <--- Enrolls ---> ( N ) [ COURSE ] (Attribute: Grade)
SQL: ENROLLMENT (
        Student_ID INT,
        Course_ID INT,
        Grade VARCHAR(2),
        PRIMARY KEY (Student_ID, Course_ID),
        FOREIGN KEY (Student_ID) REFERENCES STUDENT(Student_ID),
        FOREIGN KEY (Course_ID) REFERENCES COURSE(Course_ID)
     )
```

---

### Rule 6: Mapping Multivalued Attributes
* Create a **separate new table** for the multivalued attribute.
* Attributes in new table: $(\text{Parent Entity PK}, \text{Multivalued Attribute Value})$.
* Primary Key = Composite of all attributes in the new table.
* Foreign Key = References parent entity table with `ON DELETE CASCADE`.

```text
ER:  [ EMPLOYEE ] ( Emp_ID [PK] ) with Multivalued Attribute (( Phone_Number ))
SQL: EMP_PHONES (
        Emp_ID INT,
        Phone_Number VARCHAR(15),
        PRIMARY KEY (Emp_ID, Phone_Number),
        FOREIGN KEY (Emp_ID) REFERENCES EMPLOYEE(Emp_ID) ON DELETE CASCADE
     )
```

---

### Rule 7: Mapping Composite Attributes
* Flatten the hierarchy. Do **not** create a separate table.
* Direct simple sub-components become individual columns in the entity table.

```text
ER:  Attribute Address -> ( Street, City, ZipCode )
SQL: EMPLOYEE ( Emp_ID INT PK, Street VARCHAR(100), City VARCHAR(50), ZipCode VARCHAR(10) )
```

---

## 4. Comprehensive Translation Matrix

| ER Element | Relational Database Schema Equivalent | Primary Key Formation |
| :--- | :--- | :--- |
| **Strong Entity** | Independent Table | Entity's Key Attribute |
| **Weak Entity** | Dependent Table | Composite: Owner PK + Discriminator |
| **1:1 Relationship** | FK in one table (prefer Total side) | Table's natural PK |
| **1:N Relationship** | FK placed inside **$N$-side** table | $N$-side Table's natural PK |
| **M:N Relationship** | Junction Table | Composite: $(\text{PK}_A + \text{PK}_B)$ |
| **Multivalued Attribute**| Dedicated Secondary Table | Composite: $(\text{Parent PK} + \text{Attribute Value})$ |
| **Composite Attribute** | Direct flattened scalar columns | N/A (Part of entity table) |

---

## 5. Conceptual Practice Exercises

1. Map an ER relationship `DOCTOR` (1) $\leftrightarrow$ (N) `PATIENT` where `PATIENT` has a multivalued attribute `(( Allergies ))`. Write the full relational DDL schemas.
2. What minimum number of tables are required to map $3$ Strong Entities connected via a single Ternary M:N:P relationship?
3. Convert an EER Aggregation pattern `(EMPLOYEE WORKS_ON PROJECT) REQUIRES EQUIPMENT` into relational tables.

---

## 6. Hard Placement & Interview Questions (FAANG Level)

### Q1: Given an ER model with $E$ Strong Entities, $W$ Weak Entities, $B_{1:1}$ binary 1:1 relationships, $B_{1:N}$ binary 1:N relationships, $B_{M:N}$ binary M:N relationships, and $M$ multivalued attributes. Derive the mathematically MINIMUM number of relational tables required to represent this entire schema without data loss.

**Answer:**
Let us analyze table requirements per component:
1. **Strong Entities ($E$)**: Require $E$ tables.
2. **Weak Entities ($W$)**: Require $W$ tables.
3. **Binary 1:1 Relationships ($B_{1:1}$)**: 0 extra tables (FK folded into participating entity tables).
4. **Binary 1:N Relationships ($B_{1:N}$)**: 0 extra tables (FK folded into $N$-side entity tables).
5. **Binary M:N Relationships ($B_{M:N}$)**: Requires $B_{M:N}$ junction tables.
6. **Multivalued Attributes ($M$)**: Requires $M$ secondary tables.

$$\text{Minimum Total Tables} = E + W + B_{M:N} + M$$

---

### Q2: Why is it an anti-pattern to fold a binary 1:N relationship into the 1-side table as an array column (e.g., PostgreSQL `INTEGER[]`), even though it avoids creating a foreign key on the N-side?

**Answer:**
Storing foreign keys as an array in the 1-side table (`Department(Dept_ID, Employee_IDs[])`) violates relational properties:
1. **1NF Violation**: Attributes must contain scalar atomic values. An array of IDs is non-atomic.
2. **Indexing Deficiencies**: Standard B+ Tree indexes cannot efficiently index individual elements inside array columns compared to foreign key B+ Tree index scans on scalar columns.
3. **Referential Integrity Loss**: Standard RDBMS engines cannot enforce `FOREIGN KEY ON DELETE CASCADE` integrity constraints on individual array elements.
4. **Query Overhead**: Joins require array unnesting functions (`UNNEST()`), causing full table scans instead of fast index joins.

---

### Q3: Demonstrate the relational schema transformation of a Recursive 1:N Relationship (e.g., "An Employee manages multiple Employees") and explain how self-referential integrity constraints are maintained during `DELETE` operations.

**Answer:**

```sql
CREATE TABLE Employees (
    Emp_ID INT PRIMARY KEY,
    Name VARCHAR(100) NOT NULL,
    Manager_ID INT NULL, -- Self-referencing Foreign Key
    FOREIGN KEY (Manager_ID) REFERENCES Employees(Emp_ID)
        ON DELETE SET NULL -- Or RESTRICT / CASCADE
);
```

* **Execution Behavior on Deleting a Manager**:
  * If `ON DELETE SET NULL`: Deleting Manager 101 automatically sets `Manager_ID = NULL` for all direct report employees.
  * If `ON DELETE CASCADE`: Deleting Manager 101 recursively deletes all direct reports, which then deletes *their* direct reports (entire reporting subtree deleted).
  * If `ON DELETE RESTRICT`: DBMS blocks deletion of Manager 101 as long as at least one report points to 101.
