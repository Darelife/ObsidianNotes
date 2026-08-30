# Lecture 7: The Relational Model in DBMS

---

## 1. Introduction to the Relational Model

Proposed by Edgar F. Codd (E.F. Codd) of IBM in 1970, the **Relational Model** represents database data as a collection of two-dimensional tables called **Relations**. It forms the theoretical and mathematical foundation of modern RDBMS engines (Oracle, PostgreSQL, MySQL, SQL Server).

---

## 2. Core Mathematical Terminologies & Concepts

```
                       RELATION: "STUDENT"
+-------------------------------------------------------------------+
| Roll_No (PK) |  Name       |  Email               |  Age  | Dept  | <--- Relation Schema Header
+--------------+-------------+----------------------+-------+-------+
| 101          | Alice       | alice@univ.edu       | 20    | CS    | \
| 102          | Bob         | bob@univ.edu         | 21    | EE    |  |--- Tuples (Rows / Records)
| 103          | Charlie     | charlie@univ.edu     | 19    | CS    | /
+-------------------------------------------------------------------+
       ^
       |
  Attribute (Column)
  Domain: Integers > 0
```

### 2.1 Terminology Mapping

| Formal Relational Mathematical Term | Informal SQL / Commercial Database Term | Physical File Term |
| :--- | :--- | :--- |
| **Relation** | Table | File |
| **Tuple** | Row / Record | Data Record / Line |
| **Attribute** | Column / Field | Field Header |
| **Domain** | Data Type & Allowed Value Set | Data Field Type |
| **Degree** | Column Count (Number of Attributes) | Record Width |
| **Cardinality** | Row Count (Number of Tuples) | File Size / Line Count |

---

### 2.2 Formal Definitions

1. **Domain ($D$)**: A set of atomic (indivisible) values. (e.g., $\text{Domain of Age} = \{ x \in \mathbb{Z} \mid 16 \le x \le 100 \}$).
2. **Relation Schema ($R$)**: Written as $R(A_1, A_2, \dots, A_n)$, where $R$ is the relation name and $A_1, A_2, \dots, A_n$ are attributes defined over domains $D_1, D_2, \dots, D_n$.
3. **Relation Instance ($r(R)$)**: A mathematical subset of the Cartesian product of the domains of its attributes:
   $$r(R) \subseteq D_1 \times D_2 \times \dots \times D_n$$
4. **Degree of a Relation**: The total number of attributes ($n$) in the schema.
5. **Cardinality of a Relation**: The total number of tuples ($m$) currently contained in the relation instance $r(R)$.

---

## 3. Properties of Relations

A valid mathematical relation enforces six fundamental properties:

1. **Tuples are Unordered**: Ordering of rows does not affect the semantic meaning of the relation. $(\text{Row 1, Row 2}) \equiv (\text{Row 2, Row 1})$.
2. **Attributes are Unordered**: Ordering of columns in the schema header is semantically irrelevant.
3. **Values are Atomic (First Normal Form Assumption)**: Every attribute value in a tuple must be a single scalar atomic value from the domain (no lists, sets, or nested records).
4. **All Tuples are Distinct**: A relation cannot contain identical duplicate rows (mathematical set property).
5. **Null Values**: Special marker `NULL` signifies unknown, missing, or inapplicable attribute values.
6. **Schema Stability**: Attributes have defined fixed domain definitions.

---

## 4. Key Hierarchy in the Relational Model

Keys are attribute combinations that uniquely identify tuples within a relation and establish inter-table relationships.

```
       +-------------------------------------------------------+
       |                     SUPER KEYS                        |
       |  +-------------------------------------------------+  |
       |  |                 CANDIDATE KEYS                  |  |
       |  |  +--------------------+  +-------------------+  |  |
       |  |  |    PRIMARY KEY     |  |   ALTERNATE KEY   |  |  |
       |  |  +--------------------+  +-------------------+  |  |
       |  +-------------------------------------------------+  |
       +-------------------------------------------------------+
```

### 4.1 Super Key (SK)
* **Definition**: Any set of one or more attributes that uniquely identifies a tuple within a relation.
* **Formal Condition**: For any two distinct tuples $t_1, t_2 \in r(R)$, $t_1[SK] \neq t_2[SK]$.
* **Example**: For `Student(Roll_No, Email, Name, Phone)`:
  * `{Roll_No}`, `{Email}`, `{Roll_No, Name}`, `{Roll_No, Email, Phone}` are all Super Keys.

### 4.2 Candidate Key (CK)
* **Definition**: A **minimal Super Key**—a super key from which no proper subset can be removed without losing the property of unique identification.
* **Formal Condition**: 
  1. $CK$ is a Super Key.
  2. $\forall A \in CK$, $(CK - \{A\})$ is **not** a Super Key.
* **Example**: From the super keys above, `{Roll_No}` and `{Email}` are Candidate Keys. `{Roll_No, Name}` is NOT a candidate key because `{Roll_No}` alone suffices.

### 4.3 Primary Key (PK)
* **Definition**: The specific Candidate Key chosen by the database designer as the principal mechanism for identifying tuples within the relation.
* **Rules**: Must be `UNIQUE` and **CANNOT contain `NULL` values**.

### 4.4 Alternate Key (AK)
* **Definition**: Candidate keys that were **not** chosen as the Primary Key.
* **Formula**: $\text{Alternate Keys} = \text{Candidate Keys} - \{\text{Primary Key}\}$.

### 4.5 Foreign Key (FK)
* **Definition**: An attribute or combination of attributes in a referencing relation $R_1$ that matches the Candidate/Primary Key of a referenced relation $R_2$.
* **Purpose**: Establishes referential integrity across relations.

---

## 5. Relational Integrity Constraints

Integrity Constraints are rules enforced by the RDBMS engine to ensure data correctness, consistency, and validity during transactions.

```
                      INTEGRITY CONSTRAINTS
                      /         |         \
                     /          |          \
            Domain Integrity  Entity Integrity  Referential Integrity
            (Valid types,     (PK != NULL,      (FK points to valid
             CHECK rules)      PK is Unique)     PK or is NULL)
```

### 5.1 Domain Integrity Constraint
* Specifies that every value stored in an attribute must belong to the domain defined for that attribute (e.g., data type, range bounds, `CHECK (Age >= 18)`).

### 5.2 Entity Integrity Constraint
* **Rule**: No attribute participating in the **Primary Key** of a relation can contain a `NULL` value.
* **Reasoning**: Primary keys uniquely identify individual records in physical storage. A `NULL` primary key would mean an unidentified entity, breaking relational set identity.

### 5.3 Referential Integrity Constraint
* **Rule**: A Foreign Key $FK$ in referencing relation $R_1$ that references Primary Key $PK$ in referenced relation $R_2$ must satisfy:
  $$\forall t_1 \in R_1, \text{ either } t_1[FK] = \text{NULL} \quad \text{or} \quad \exists t_2 \in R_2 \text{ such that } t_1[FK] = t_2[PK]$$

#### Cascading Actions on Referential Violation (`ON DELETE` / `ON UPDATE`):
1. `RESTRICT` / `NO ACTION` (Default): Rejects the operation on parent table if matching child records exist.
2. `CASCADE`: Deletes or updates corresponding matching child records automatically.
3. `SET NULL`: Sets matching child foreign key values to `NULL`.
4. `SET DEFAULT`: Sets matching child foreign key values to a pre-defined default value.

---

## 6. Conceptual Practice Exercises

1. Given a relation schema $R(A, B, C, D)$ with candidate keys $\{A, B\}$ and $\{C\}$, list **all** possible Super Keys of $R$.
2. Calculate the maximum and minimum degree and cardinality when performing a Cartesian Product ($R_1 \times R_2$) of relation $R_1$ (Degree 4, Cardinality 100) and $R_2$ (Degree 3, Cardinality 50).
3. Why can a Foreign Key contain `NULL` values, while a Primary Key strictly cannot?

---

## 7. Hard Placement & Interview Questions (FAANG Level)

### Q1: Given a relation schema $R(A_1, A_2, \dots, A_n)$ where EVERY attribute $A_i$ is an independent candidate key by itself. What is the exact total number of Super Keys for relation $R$?

**Answer:**
* **Given**: Each individual single attribute $\{A_i\}$ is a Candidate Key.
* **Super Key Definition**: Any subset of attributes containing at least one candidate key.
* **Analysis**: Since *every* single attribute is a candidate key, any non-empty subset of attributes in $R$ contains at least one candidate key.
* **Calculation**:
  * Total number of subsets of an $n$-attribute set is $2^n$.
  * The only subset that contains NO candidate key is the empty set $\emptyset$.
* **Total Super Keys**: $2^n - 1$.

---

### Q2: Prove mathematically why the intersection of two Super Keys is NOT guaranteed to be a Super Key, but the union of two Super Keys is ALWAYS a Super Key.

**Answer:**
Let $S_1$ and $S_2$ be two super keys of relation $R(A, B, C, D)$.

1. **Union ($S_1 \cup S_2$)**:
   * Since $S_1$ is a super key, $S_1$ uniquely identifies every tuple in $R$.
   * Adding attributes from $S_2$ to $S_1$ yields $S_1 \cup S_2$.
   * Adding attributes to a super key maintains uniqueness (superscaling property of keys).
   * Therefore, $S_1 \cup S_2$ is **always** a Super Key.

2. **Intersection ($S_1 \cap S_2$)**:
   * *Counterexample*: Consider $R(A, B, C)$ where Candidate Keys are $\{A, B\}$ and $\{A, C\}$.
   * Let $S_1 = \{A, B, C\}$ (Super key containing candidate key $\{A, B\}$).
   * Let $S_2 = \{A, B, D\}$ (Super key containing candidate key $\{A, B\}$).
   * Now consider candidate keys $K_1 = \{A, B\}$ and $K_2 = \{B, C\}$.
   * Let $S_1 = \{A, B\}$ (Super key). Let $S_2 = \{B, C\}$ (Super key).
   * $S_1 \cap S_2 = \{B\}$.
   * $\{B\}$ alone is NOT a candidate key or super key, as $B$ does not uniquely identify tuples.
   * Hence, the intersection of two super keys is **not** guaranteed to be a super key.

---

### Q3: What is the "Phantom Row" anomaly caused by referential integrity violations during concurrent transactions, and how do database engines resolve it?
**Answer:**
* **Scenario**: Transaction $T_1$ inserts a child tuple referencing Parent ID 500 after verifying Parent ID 500 exists in `ParentTable`. Concurrently, Transaction $T_2$ deletes Parent ID 500 from `ParentTable` and commits before $T_1$ commits.
* **Anomaly**: Child table now contains an orphaned foreign key record pointing to a non-existent parent row.
* **Resolution**: RDBMS engines acquire Shared ($S$) locks or Intent locks on the referenced parent row during child insertion, or use Serializable snapshot isolation (SSI) to detect write-skew conflicts before commit.
