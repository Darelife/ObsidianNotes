# Lecture 9: Normalization in DBMS — Part 1 (Functional Dependencies, 1NF, 2NF)

---

## 1. Motivation: Database Anomalies

Un-normalized database designs lead to data redundancy, wasting disk space and causing severe transactional anomalies.

### Example Un-normalized Relation `Emp_Dept`:
$$\text{Emp\_Dept}(\underline{\text{Emp\_ID}}, \text{Emp\_Name}, \text{Dept\_ID}, \text{Dept\_Name}, \text{Dept\_Head})$$

```
+--------+----------+---------+------------+-----------+
| Emp_ID | Emp_Name | Dept_ID | Dept_Name  | Dept_Head |
+--------+----------+---------+------------+-----------+
| 101    | Alice    | D01     | Engineering| Dr. Smith |
| 102    | Bob      | D01     | Engineering| Dr. Smith |
| 103    | Charlie  | D02     | HR         | Ms. Davis |
+--------+----------+---------+------------+-----------+
```

### The Three Classic Database Anomalies:

1. **Insertion Anomaly**:
   * *Problem*: We cannot insert a newly created Department (e.g., `D03 - Finance`) into the table unless we assign a valid `Emp_ID`. Since `Emp_ID` is the Primary Key, it cannot be `NULL`.
2. **Deletion Anomaly**:
   * *Problem*: If Employee 103 (`Charlie`) leaves the company and we delete their record row, the entire `D02 - HR` department data is permanently wiped from the database.
3. **Update Anomaly**:
   * *Problem*: If `Dr. Smith` resigns and is replaced by `Dr. Adams` as Head of Engineering, we must update multiple rows across the database. If we update Bob's row but miss Alice's row, the database enters an inconsistent state.

---

## 2. Functional Dependencies (FDs)

A **Functional Dependency (FD)** is a formal constraint between two sets of attributes in a relation.

### Definition:
Given a relation $R$, a functional dependency $X \rightarrow Y$ (read as "$X$ functionally determines $Y$", or "$Y$ is functionally dependent on $X$") holds if, for any two tuples $t_1, t_2 \in R$:
$$\text{If } t_1[X] = t_2[X], \quad \text{then } t_1[Y] = t_2[Y]$$

```
   [ X ] ---------------------> [ Y ]
(Left Hand Side:               (Right Hand Side:
 Determinant)                   Dependent)
```

### Types of Functional Dependencies:

1. **Trivial FD**: $X \rightarrow Y$ is trivial if $Y \subseteq X$.
   * *Example*: $\{\text{Emp\_ID}, \text{Name}\} \rightarrow \{\text{Name}\}$. Always holds true.
2. **Non-Trivial FD**: $X \rightarrow Y$ is non-trivial if $Y \not\subseteq X$.
   * *Example*: $\text{Emp\_ID} \rightarrow \text{Emp\_Name}$.
3. **Completely Non-Trivial FD**: $X \rightarrow Y$ where $X \cap Y = \emptyset$.

---

## 3. Armstrong's Axioms for Functional Dependencies

Armstrong's Axioms are a set of sound and complete inference rules used to compute the closure of functional dependencies.

```
                  ARMSTRONG'S AXIOMS
                  /        |        \
                 /         |         \
         Reflexivity   Augmentation  Transitivity
         If Y <= X,    If X -> Y,    If X -> Y & Y -> Z,
         then X -> Y   then XZ -> YZ then X -> Z
```

### Primary Axioms (Sound & Complete):
1. **Reflexivity Rule**: If $Y \subseteq X$, then $X \rightarrow Y$.
2. **Augmentation Rule**: If $X \rightarrow Y$, then $XZ \rightarrow YZ$ for any set of attributes $Z$.
3. **Transitivity Rule**: If $X \rightarrow Y$ and $Y \rightarrow Z$, then $X \rightarrow Z$.

### Secondary Derived Rules (Convenience Rules):
4. **Union Rule**: If $X \rightarrow Y$ and $X \rightarrow Z$, then $X \rightarrow YZ$.
5. **Decomposition Rule**: If $X \rightarrow YZ$, then $X \rightarrow Y$ and $X \rightarrow Z$.
6. **Pseudo-Transitivity Rule**: If $X \rightarrow Y$ and $WY \rightarrow Z$, then $WX \rightarrow Z$.

---

## 4. Attribute Closure ($X^+$) Algorithm

The **Attribute Closure** $X^+$ of a set of attributes $X$ under a set of functional dependencies $F$ is the set of all attributes functionally determined by $X$.

### Algorithm:
```text
Input: Set of attributes X, Set of FDs F
Output: Attribute Closure X+

1. X+ := X
2. repeat
3.     for each FD (Y -> Z) in F do
4.         if Y <= X+ then
5.             X+ := X+ U Z
6.         end if
7.     end for
8. until (X+ does not change in an iteration)
9. return X+
```

### Usage of Attribute Closure:
* **Testing Super Keys**: $X$ is a Super Key of relation $R$ if $X^+$ contains **all** attributes of $R$.
* **Testing Candidate Keys**: $X$ is a Candidate Key if $X^+$ contains all attributes of $R$, and no proper subset $Y \subset X$ has $Y^+$ containing all attributes.

---

## 5. First Normal Form (1NF)

### Definition:
A relation $R$ is in **First Normal Form (1NF)** if and only if the domain of every attribute contains only **atomic (indivisible) scalar values**, and there are no repeating groups or composite/multivalued attributes.

```
NON-1NF TABLE:
+--------+----------+--------------------------+
| Emp_ID | Name     | Phone_Numbers (Multi)   |
+--------+----------+--------------------------+
| 101    | Alice    | 9876543210, 9123456789   |  <-- Violates 1NF!
+--------+----------+--------------------------+

CONVERTED TO 1NF TABLE:
+--------+----------+--------------+
| Emp_ID | Name     | Phone_Number |
+--------+----------+--------------+
| 101    | Alice    | 9876543210   |
| 101    | Alice    | 9123456789   |
+--------+----------+--------------+
```

---

## 6. Second Normal Form (2NF)

### Definition:
A relation $R$ is in **Second Normal Form (2NF)** if and only if:
1. $R$ is in **1NF**.
2. No non-prime attribute is **partially dependent** on any candidate key of $R$.

*(Translation: Every non-prime attribute must depend on the WHOLE candidate key, not just a proper subset of it).*

```
           PARTIAL DEPENDENCY (2NF Violation):
     Composite Candidate Key: ( Student_ID, Course_ID )
     
     ( Student_ID, Course_ID ) ------------> Grade        (Full Dependency - OK)
       Student_ID -------------------------> Student_Name (Partial Dependency - VIOLATION!)
```

### Prime vs. Non-Prime Attribute:
* **Prime Attribute**: An attribute that is a member of **at least one** Candidate Key of $R$.
* **Non-Prime Attribute**: An attribute that is **not** a member of any Candidate Key of $R$.

### Partial Dependency Definition:
A dependency $X \rightarrow A$ is a **Partial Dependency** if:
1. $X$ is a proper subset of a Candidate Key ($X \subset CK$).
2. $A$ is a non-prime attribute.

### How to Convert a Relation to 2NF:
If a partial dependency $X \rightarrow A$ exists:
1. Decompose $R$ into two relations:
   * $R_1(X, A)$ (Contains the determinant subset and the partially dependent attribute).
   * $R_2(R - \{A\})$ (Original relation with non-prime attribute $A$ removed).

---

## 7. Conceptual Practice Exercises

1. Given $R(A, B, C, D, E, F)$ and $F = \{ A \rightarrow B, BC \rightarrow DE, E \rightarrow F \}$. Compute $\{A, C\}^+$. Is $\{A, C\}$ a Candidate Key?
2. Test if $R(A, B, C, D)$ with Primary Key $(A, B)$ and FDs $\{ AB \rightarrow C, B \rightarrow D \}$ is in 2NF. If not, decompose it into 2NF relations.
3. Prove that any relation schema with a single scalar attribute Candidate Key is automatically in 2NF (provided it is in 1NF).

---

## 8. Hard Placement & Interview Questions (FAANG Level)

### Q1: Given a relation schema $R(A, B, C, D, E)$ with functional dependencies $F = \{ AB \rightarrow C, C \rightarrow D, D \rightarrow E, E \rightarrow A \}$. Find ALL Candidate Keys of $R$.

**Answer:**
Let us determine candidate keys using attribute closures:

1. **Identify essential attributes**:
   * Attributes appearing only on LHS: None.
   * Attributes appearing only on RHS: None.
   * Attributes not appearing anywhere: $B$ (appears on LHS of $AB$, but never on RHS).
   * Since $B$ is never determined by any attribute, **$B$ must be present in EVERY candidate key**.

2. **Compute closures containing $B$**:
   * Try $\{A, B\}^+$:
     * $\{A, B\}^0 = \{A, B\}$
     * $AB \rightarrow C \implies \{A, B, C\}$
     * $C \rightarrow D \implies \{A, B, C, D\}$
     * $D \rightarrow E \implies \{A, B, C, D, E\} = R$.
     * Thus, **$\{A, B\}$ is a Candidate Key**.

   * Try $\{C, B\}^+$:
     * $\{C, B\}^0 = \{C, B\}$
     * $C \rightarrow D \implies \{B, C, D\}$
     * $D \rightarrow E \implies \{B, C, D, E\}$
     * $E \rightarrow A \implies \{A, B, C, D, E\} = R$.
     * Thus, **$\{C, B\}$ is a Candidate Key**.

   * Try $\{D, B\}^+$:
     * $\{D, B\}^0 = \{D, B\}$
     * $D \rightarrow E \implies \{B, D, E\}$
     * $E \rightarrow A \implies \{A, B, D, E\}$
     * $AB \rightarrow C \implies \{A, B, C, D, E\} = R$.
     * Thus, **$\{D, B\}$ is a Candidate Key**.

   * Try $\{E, B\}^+$:
     * $\{E, B\}^0 = \{E, B\}$
     * $E \rightarrow A \implies \{A, B, E\}$
     * $AB \rightarrow C \implies \{A, B, C, E\}$
     * $C \rightarrow D \implies \{A, B, C, D, E\} = R$.
     * Thus, **$\{E, B\}$ is a Candidate Key**.

3. **Conclusion**:
   The relation has **4 Candidate Keys**: $\{A, B\}$, $\{C, B\}$, $\{D, B\}$, and $\{E, B\}$.
   * Prime Attributes: $\{A, B, C, D, E\}$ (All attributes are prime!).
   * Non-Prime Attributes: $\emptyset$.
   * Since there are zero non-prime attributes, partial dependencies cannot exist $\implies$ **Relation is automatically in 2NF!**

---

### Q2: What is the Canonical Cover (Minimal Cover) of a set of functional dependencies, and why is it required before performing 3NF synthesis?

**Answer:**
A **Canonical Cover ($F_c$)** of a set of FDs $F$ is a simplified minimal set of dependencies equivalent to $F$ satisfying 3 properties:
1. Every FD in $F_c$ is of the form $X \rightarrow A$, where $A$ is a single attribute.
2. No extraneous attributes exist on LHS or RHS of any FD in $F_c$.
3. No FD in $F_c$ is redundant (i.e., removing $X \rightarrow A$ does not alter $F_c^+$).

**Why required for 3NF Synthesis**:
Without computing canonical cover first, standard 3NF decomposition algorithms produce redundant relations, leading to unnecessary tables and un-optimized database schemas.

---

### Q3: Prove that if a relation $R$ is in 2NF and has NO composite Candidate Keys (all Candidate Keys consist of a single attribute), then $R$ is guaranteed to be in 2NF without any partial dependency checks.
**Answer:**
* A Partial Dependency requires $X \rightarrow A$, where $X \subset CK$ (proper subset of a Candidate Key) and $A$ is non-prime.
* If all Candidate Keys of $R$ are singletons (contain exactly 1 attribute, $|CK| = 1$), the only proper subset of any Candidate Key is the empty set $\emptyset$.
* Since an empty set cannot functionally determine non-trivial non-prime attributes in a valid 1NF relation, no partial dependency can mathematically exist.
* Therefore, single-attribute candidate key relations in 1NF are **automatically in 2NF**.
