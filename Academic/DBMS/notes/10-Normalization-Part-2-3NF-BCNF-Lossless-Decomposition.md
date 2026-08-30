# Lecture 10: Normalization Part 2 (3NF, BCNF, Lossless Join & Dependency Preservation)

---

## 1. Third Normal Form (3NF)

### Motivation: Eliminating Transitive Dependencies
While 2NF eliminates partial dependencies, it still permits **Transitive Dependencies**, which cause update anomalies.

```
       TRANSITIVE DEPENDENCY (3NF Violation):
       Emp_ID -------------------------> Dept_ID      (Emp_ID is Candidate Key)
       Dept_ID ------------------------> Dept_Name    (Dept_ID is NOT a Candidate Key)
       ------------------------------------------------
       Emp_ID ------------[Transitive]---> Dept_Name
```

---

### Definition of 3NF:
A relation schema $R$ is in **Third Normal Form (3NF)** if, for every non-trivial functional dependency $X \rightarrow A$ in $F^+$:
1. $X$ is a **Super Key** of $R$, **OR**
2. $A$ is a **Prime Attribute** (a member of some candidate key).

```
                      3NF CONDITION: X -> A
                      /                   \
                     /                     \
       (X is a Super Key)        OR        (A is a Prime Attribute)
```

---

## 2. Boyce-Codd Normal Form (BCNF)

### Motivation: Stricter Normalization than 3NF
BCNF was proposed by Raymond F. Boyce and Edgar F. Codd to handle anomalies arising in relations with **multiple overlapping candidate keys**.

### Definition of BCNF:
A relation schema $R$ is in **Boyce-Codd Normal Form (BCNF)** if, for every non-trivial functional dependency $X \rightarrow A$ in $F^+$:
* $X$ is a **Super Key** of $R$.

```
                      BCNF CONDITION: X -> A
                                |
                                v
                   (X MUST BE A SUPER KEY ONLY)
                   [No exceptions for Prime Attributes!]
```

---

### Comparison: 3NF vs. BCNF

| Feature | Third Normal Form (3NF) | Boyce-Codd Normal Form (BCNF) |
| :--- | :--- | :--- |
| **Strictness** | Slightly weaker than BCNF. | Strictly stronger than 3NF ($\text{BCNF} \subset \text{3NF}$). |
| **Allowed FDs** | Determinant is Super Key **OR** Dependent is Prime Attribute. | Determinant **MUST** be a Super Key. |
| **Dependency Preservation** | **Always guaranteed** during 3NF decomposition. | **Not always guaranteed** without losing BCNF property. |
| **Lossless Join** | **Always achievable**. | **Always achievable**. |
| **Anomalies** | Minor redundancy allowed when prime attributes depend on non-keys. | Zero functional dependency redundancy anomalies. |

```
       +-------------------------------------------------------+
       |                         1NF                           |
       |  +-------------------------------------------------+  |
       |  |                      2NF                        |  |
       |  |  +-------------------------------------------+  |  |
       |  |  |                   3NF                     |  |  |
       |  |  |  +-------------------------------------+  |  |  |
       |  |  |  |                BCNF                 |  |  |  |
       |  |  |  +-------------------------------------+  |  |  |
       |  |  +-------------------------------------------+  |  |
       |  +-------------------------------------------------+  |
       +-------------------------------------------------------+
```

---

## 3. Lossless-Join Decomposition

When a relation $R$ is decomposed into $R_1, R_2, \dots, R_k$, the decomposition must be **Lossless-Join** (Non-additive join).

### Definition:
A decomposition of $R$ into $R_1$ and $R_2$ is **Lossless** if:
$$R_1 \bowtie R_2 = R$$

If $R_1 \bowtie R_2 \supset R$, the decomposition is **Lossy** because it introduces **Spurious (Fake) Tuples** upon joining.

```
                  LOSSLESS JOIN TEST FOR DECOMPOSITION (R1, R2):
                  Must satisfy AT LEAST ONE condition:
                  1) (R1 ∩ R2) -> R1   (Common attribute is Super Key of R1)
                  2) (R1 ∩ R2) -> R2   (Common attribute is Super Key of R2)
```

---

## 4. Dependency Preservation Property

### Definition:
A decomposition of $R$ into $R_1, R_2, \dots, R_k$ is **Dependency Preserving** if the closure of the union of dependencies across all sub-relations equals the closure of the original dependency set $F$:
$$(F_1 \cup F_2 \cup \dots \cup F_k)^+ = F^+$$

* **Why it matters**: If a dependency $X \rightarrow Y$ is NOT preserved in a single table decomposition, enforcing $X \rightarrow Y$ during SQL `INSERT`/`UPDATE` calls requires expensive cross-table `JOIN` checks across distributed nodes.

---

## 5. Summary of Decomposition Possibilities

| Normal Form Target | Lossless Join Guaranteed? | Dependency Preservation Guaranteed? |
| :--- | :--- | :--- |
| **2NF Decomposition** | Yes | Yes |
| **3NF Decomposition** | **Yes** | **Yes** |
| **BCNF Decomposition** | **Yes** | **No (Not always achievable)** |

---

## 6. Conceptual Practice Exercises

1. Given $R(A, B, C)$ with $F = \{ AB \rightarrow C, C \rightarrow A \}$. Find Candidate Keys and test if $R$ is in 3NF and BCNF.
2. Prove that if $R_1 \cap R_2 = \emptyset$, joining $R_1$ and $R_2$ will produce a Cartesian product containing spurious tuples.
3. Show an example of a relation in BCNF where dependency preservation is sacrificed.

---

## 7. Hard Placement & Interview Questions (FAANG Level)

### Q1: Given $R(A, B, C, D)$ with FDs $F = \{ A \rightarrow B, B \rightarrow C, C \rightarrow D, D \rightarrow A \}$. Determine the highest normal form of $R$.

**Answer:**
1. **Find Candidate Keys**:
   * Compute closures:
     * $A^+ = \{A, B, C, D\} = R \implies \{A\}$ is a Candidate Key.
     * $B^+ = \{B, C, D, A\} = R \implies \{B\}$ is a Candidate Key.
     * $C^+ = \{C, D, A, B\} = R \implies \{C\}$ is a Candidate Key.
     * $D^+ = \{D, A, B, C\} = R \implies \{D\}$ is a Candidate Key.
   * Every single attribute $\{A\}$, $\{B\}$, $\{C\}$, $\{D\}$ is an independent Candidate Key!
   * Super Keys: Every non-empty subset of $R$ is a Super Key.

2. **Check BCNF Condition**:
   * For every non-trivial FD $X \rightarrow Y$ in $F$:
     * $A \rightarrow B$: Determinant $A$ is a Super Key (Check!).
     * $B \rightarrow C$: Determinant $B$ is a Super Key (Check!).
     * $C \rightarrow D$: Determinant $C$ is a Super Key (Check!).
     * $D \rightarrow A$: Determinant $D$ is a Super Key (Check!).

3. **Conclusion**:
   Every determinant is a Super Key. Therefore, relation $R$ is in **BCNF** (highest normal form).

---

### Q2: Given $R(A, B, C, D)$ with $F = \{ AB \rightarrow C, C \rightarrow D, D \rightarrow A \}$. Decompose $R$ into BCNF step-by-step. Is the resulting decomposition dependency-preserving?

**Answer:**
1. **Candidate Keys of $R$**: $\{A, B\}$, $\{D, B\}$, $\{C, B\}$.
2. **Check 3NF / BCNF Violations**:
   * $AB \rightarrow C$: $AB$ is a Super Key (OK).
   * $C \rightarrow D$: $C$ is NOT a Super Key ($C^+ = \{C, D, A\}$). $D$ is prime (3NF OK, **BCNF Violation**).
   * $D \rightarrow A$: $D$ is NOT a Super Key ($D^+ = \{D, A, B\}$ is false; $D^+ = \{D, A, B\}$ needs $B$). **BCNF Violation**.

3. **BCNF Decomposition Step**:
   * Pick violating FD $C \rightarrow D$.
   * Decompose $R$ into:
     * $R_1(C, D)$ with $F_1 = \{ C \rightarrow D \}$. Candidate Key: $\{C\}$. (In BCNF!).
     * $R_2(A, B, C)$ with attributes $(R - \{D\})$.

4. **Check $R_2(A, B, C)$**:
   * Dependencies holding on $R_2$: $\{ AB \rightarrow C, C \rightarrow A \}$.
   * Candidate Keys of $R_2$: $\{A, B\}$ and $\{C, B\}$.
   * $C \rightarrow A$: $C$ is NOT a Super Key of $R_2$ ($C^+ = \{C, A\}$). **BCNF Violation!**
   * Decompose $R_2$ using $C \rightarrow A$:
     * $R_{2a}(C, A)$ with $F_{2a} = \{ C \rightarrow A \}$. (In BCNF).
     * $R_{2b}(B, C)$ with attributes $(R_2 - \{A\})$. (In BCNF).

5. **Final BCNF Relations**: $R_1(C, D)$, $R_{2a}(C, A)$, $R_{2b}(B, C)$.
6. **Dependency Preservation Check**:
   * Original FD $AB \rightarrow C$ is now split across $R_{2a}(C, A)$ and $R_{2b}(B, C)$.
   * $AB$ cannot be evaluated in any single relation ($A$ and $B$ do not exist together in any sub-table!).
   * Hence, **Dependency $AB \rightarrow C$ is LOST**. This proves BCNF does not always preserve functional dependencies.

---

### Q3: Explain why 3NF Synthesis (Bernstein's Algorithm) guarantees both Lossless Join and Dependency Preservation, whereas standard top-down BCNF decomposition may drop functional dependencies.
**Answer:**
* **Bernstein's 3NF Synthesis** works bottom-up:
  1. Computes Canonical Cover $F_c$.
  2. For every FD $X \rightarrow Y \in F_c$, creates a explicit target schema $R_i(X, Y)$. (Directly guarantees **Dependency Preservation**).
  3. If no created relation contains a Candidate Key of original $R$, adds $R_{key}(CK)$ to guarantee **Lossless Join**.
* **BCNF Decomposition** works top-down by repeatedly splitting violating tables $X \rightarrow Y$ into $(X, Y)$ and $(R - Y)$. This top-down binary chopping can isolate LHS attributes from RHS attributes of other compound FDs, destroying global dependency preservation.
