# Lecture 3: Entity-Relationship (ER) Model & Notations

---

## 1. Introduction to Entity-Relationship (ER) Modeling

The **Entity-Relationship (ER) Model** is a high-level conceptual data model developed by Peter Chen (1976). It allows database designers to represent real-world enterprise structures, entities, and complex associations in a visual, technology-agnostic blueprint before implementation in a specific relational database management system.

---

## 2. Core Building Blocks of the ER Model

```
       +-------------------+       ( Has )       +-------------------+
       |     ENTITY        |=====<========>======|     ENTITY        |
       |    (Student)      |    Relationship     |    (Course)       |
       +-------------------+                     +-------------------+
           |           |                             |           |
       (Roll_No)    (Name)                       (Course_ID)  (Title)
      [Primary Key]                               [Primary Key]
```

---

### 2.1 Entities & Entity Sets

* **Entity**: A real-world object, concept, or thing that has a distinct existence and can be uniquely identified.
  * *Tangible Entities*: Student, Car, Building, Employee.
  * *Intangible/Conceptual Entities*: Account, Project, Job Position, Enrollment.
* **Entity Set**: A collection of homogeneous entities sharing identical attributes. (e.g., all `Student` entities in a university form the `STUDENT` entity set).

#### Strong vs. Weak Entities

| Feature | Strong Entity Set | Weak Entity Set |
| :--- | :--- | :--- |
| **Key Attribute** | Possesses a Primary Key. | Lacks a primary key; relies on Partial Key (Discriminator). |
| **Dependency** | Independent existence. | Existence-dependent on an Identifying Owner Entity. |
| **Notation** | Single Rectangle `[ Entity ]` | Double Rectangle `[[ Weak Entity ]]` |
| **Relationship** | Standard Relationship `< Rel >` | Identifying Relationship `<| Identifying Rel |>` (Double Diamond) |
| **Primary Key Formation** | $\text{Primary Key}$ | $\text{Owner's Primary Key} + \text{Partial Key}$ |

```
  +------------------+         < Submits >         +--------------------+
  |     EMPLOYEE     |--------<=========== >=======|     DEPENDENT      |
  |  (Strong Entity) |         (Identifying Rel)   |   (Weak Entity)    |
  +------------------+                             +--------------------+
  { SSN (PK) }                                     { Dependent_Name (Partial Key) }
```

---

### 2.2 Attributes & Attribute Classification

An **Attribute** represents a property or characteristic describing an entity set.

```
                  +--------------------------------+
                  |         ATTRIBUTE TYPES        |
                  +--------------------------------+
                    |        |          |        |
         +----------+        |          |        +----------+
         v                   v          v                   v
    [ Composite ]       [ Multi-Valued ] [ Derived ]    [ Key Attribute ]
  (First_Name, Last_Name)   (Phones)    (Age from DOB)     (Student_ID)
```

1. **Simple / Atomic Attribute**: Cannot be divided into smaller sub-components.
   * *Example*: `SSN`, `Gender`, `Temperature`.
2. **Composite Attribute**: Can be subdivided into smaller meaningful sub-parts.
   * *Example*: `Address` $\rightarrow$ (`Street`, `City`, `State`, `ZipCode`).
3. **Single-Valued Attribute**: Holds exactly one single value for a given entity instance.
   * *Example*: `Date_Of_Birth`, `SSN`.
4. **Multi-Valued Attribute**: Holds a set of multiple values for a single entity instance.
   * *Notation*: Double Ellipse `(( Multi-Valued ))`.
   * *Example*: `Phone_Numbers`, `Degrees`, `Skills`.
5. **Derived Attribute**: Attribute value is dynamically computed or derived from another attribute.
   * *Notation*: Dashed Ellipse `( Derived )`.
   * *Example*: `Age` (derived from `Date_Of_Birth` and current date), `Total_Order_Amount` (derived from sum of line items).
6. **Key Attribute**: Uniquely identifies each entity instance within an entity set.
   * *Notation*: Ellipse with Underlined Text `(_Key_Attribute_)`.

---

### 2.3 Relationships & Degree of Relationship

A **Relationship** is an association among two or more entity sets.

#### Degree of a Relationship
The **Degree** refers to the number of participating entity sets involved in the relationship.

1. **Unary (Recursive) Relationship** (Degree 1): An entity set is related to itself.
   * *Example*: An `EMPLOYEE` manages another `EMPLOYEE` (`Manages`).
2. **Binary Relationship** (Degree 2): Association between two distinct entity sets.
   * *Example*: `STUDENT` enrolls in `COURSE`.
3. **Ternary Relationship** (Degree 3): Association involving three entity sets simultaneously.
   * *Example*: `SUPPLIER` supplies a `PART` to a specific `PROJECT`.
4. **N-ary Relationship** (Degree N): Association involving $N$ entity sets.

```
Unary (Degree 1):        Binary (Degree 2):             Ternary (Degree 3):
 +----------+             +----------+    +--------+     +----------+  +---------+
 | EMPLOYEE |             | STUDENT  |----| COURSE |     | SUPPLIER |  | PROJECT |
 +----------+             +----------+    +--------+     +----------+  +---------+
   |      |                                                    \            /
   +-[Manages]-+                                                [ Supplies ]
                                                                     |
                                                                +---------+
                                                                |  PART   |
                                                                +---------+
```

---

## 3. Structural Constraints on Relationships

Structural constraints limit the number of possible combinations of entities that may participate in a relationship set. They consist of **Cardinality Ratio** and **Participation Constraints**.

### 3.1 Mapping Cardinalities (Cardinality Ratios)

Expresses the number of entities to which another entity can be associated via a binary relationship set.

1. **One-to-One (1:1)**:
   * An entity in set A is associated with at most one entity in set B, and vice versa.
   * *Example*: `DEPARTMENT` has one `MANAGER` (`EMPLOYEE`).
2. **One-to-Many (1:N)**:
   * An entity in set A can be associated with any number ($0 \dots N$) of entities in set B. An entity in set B is associated with at most one entity in set A.
   * *Example*: `DEPARTMENT` employs many `EMPLOYEES`.
3. **Many-to-One (N:1)**:
   * Reverse of 1:N. Multiple entities in set A map to a single entity in set B.
4. **Many-to-Many (M:N)**:
   * An entity in set A is associated with any number of entities in set B, and vice versa.
   * *Example*: `STUDENT` enrolls in `COURSE`.

---

### 3.2 Participation Constraints

Defines whether the existence of an entity depends on its being related to another entity via the relationship set.

1. **Total Participation (Existence Dependency)**:
   * Every entity in the entity set MUST participate in at least one relationship instance in the relationship set.
   * *Notation*: Double line `=======` connecting Entity to Relationship.
   * *Example*: Every `LOAN` must belong to a `CUSTOMER`.
2. **Partial Participation**:
   * Some entities in the entity set MAY NOT participate in the relationship.
   * *Notation*: Single line `-------` connecting Entity to Relationship.
   * *Example*: Not every `EMPLOYEE` manages a `DEPARTMENT`.

---

### 3.3 Structural Notation: Min-Max Constraint Notation `(min, max)`

The $(min, max)$ notation replaces cardinality ratios and participation constraints with a explicit pair of integers $(m, M)$ attached to each entity line in a relationship.

* $min \ge 0$: Specifies minimum participation. If $min = 0$, participation is **Partial**. If $min \ge 1$, participation is **Total**.
* $max \ge 1$: Specifies maximum participation (or $N$ for unrestricted).

```
   +----------+  (1, 1)                  (0, N)  +----------+
   | EMPLOYEE |==========[ Works_For ]-----------|DEPARTMENT|
   +----------+                                  +----------+
```
*Interpretation*:
- Every `EMPLOYEE` must work for exactly 1 department $(1,1)$.
- A `DEPARTMENT` can employ anywhere from 0 to $N$ employees $(0,N)$.

---

## 4. Standard Chen Notation Reference Table

| Concept | Chen ER Notation Symbol |
| :--- | :--- |
| **Strong Entity Set** | Rectangle `[ Entity ]` |
| **Weak Entity Set** | Double Rectangle `[[ Weak Entity ]]` |
| **Relationship Set** | Diamond `< Relationship >` |
| **Identifying Relationship**| Double Diamond `<| Identifying Rel |>` |
| **Attribute** | Ellipse `( Attribute )` |
| **Key Attribute** | Ellipse with underlined text `( _Primary_Key_ )` |
| **Partial Key (Discriminator)**| Ellipse with dashed underlined text `( -Partial_Key- )` |
| **Multivalued Attribute** | Double Ellipse `(( Multivalued ))` |
| **Derived Attribute** | Dashed Ellipse `( Dashed )` |
| **Composite Attribute** | Tree of Ellipses branching from parent ellipse |
| **Total Participation** | Double Line `======` |
| **Partial Participation**| Single Line `------` |

---

## 5. Conceptual Practice Exercises

1. Give a real-world enterprise example where a relationship between two entities has its own attributes (e.g., `Grade` in `Student-Enrolls-Course`).
2. Express a system where a `Doctor` treats a `Patient` in a specific `Hospital` using $(min, max)$ notation, ensuring a patient can be treated by multiple doctors across different hospitals.
3. Compare Weak Entity sets vs Strong Entity sets with composite multi-valued key attributes.

---

## 6. Hard Placement & Interview Questions (FAANG Level)

### Q1: Can a binary M:N relationship always be represented as a Weak Entity set? What are the precise logical and structural differences?
**Answer:**
No. While both involve dependent structures, they serve fundamental structural differences:
* **M:N Binary Relationship**: Connects two independent Strong Entity sets. The associative table formed contains the composite primary keys of both strong entities ($\text{PK}_A + \text{PK}_B$). Neither entity depends on the other for existence.
* **Weak Entity Set**: Has a 1:N existence-dependency on an Identifying Owner Entity. A weak entity cannot exist without its owner. Its partial key (discriminator) is only unique *within* the context of a single owner instance.
* **Key Difference**: In an M:N relationship, both participating entities can exist independently without any relationships present. In a Weak Entity set, a weak record cannot exist in the database without being tied to a valid parent owner tuple.

---

### Q2: Consider a Ternary Relationship $R(A, B, C)$. Under what exact conditions can $R$ be decomposed into two Binary Relationships $R_1(A, B)$ and $R_2(B, C)$ without losing semantic constraints or introducing invalid tuples (lossy decomposition)?
**Answer:**
A ternary relationship $R(A, B, C)$ can be losslessly decomposed into two binary relationships $R_1(A, B)$ and $R_2(B, C)$ **if and only if**:
1. There exists a functional dependency $B \rightarrow A$ or $B \rightarrow C$ within $R$, meaning $B$ uniquely determines $A$ or $C$.
2. If no functional dependency exists (i.e., the cardinality is strictly M:N:P without conditional constraints), decomposing $R(A,B,C)$ into $(A,B)$ and $(B,C)$ results in **spurious tuples** upon joining back $R_1 \bowtie R_2$.
* **Example**: If `Supplier S` supplies `Part P` to `Project J`, but `Supplier S` supplies `Part P` *regardless* of which project it is, then the ternary relationship is redundant and can be factored into $R_1(S, P)$ and $R_2(P, J)$.

---

### Q3: Convert the following constraint into $(min, max)$ ER notation: "A Bank Branch employs at least 3 employees and at most 50. Every employee must be assigned to exactly 1 branch. An employee may manage at most 1 branch, but a branch must have exactly 1 manager."
**Answer:**
We define two separate relationships between `BRANCH` and `EMPLOYEE`:

1. **`Employs` Relationship**:
   - `EMPLOYEE` participation: $(1, 1)$  (Assigned to exactly 1 branch)
   - `BRANCH` participation: $(3, 50)$ (Employs min 3, max 50)

2. **`Manages` Relationship**:
   - `EMPLOYEE` participation: $(0, 1)$  (Manages at most 1 branch)
   - `BRANCH` participation: $(1, 1)$  (Has exactly 1 manager)
