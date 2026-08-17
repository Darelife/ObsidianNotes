# Lecture 12: Concurrency Control in DBMS

---

## 1. Motivation: Concurrent Transaction Execution

In a multi-user enterprise DBMS, executing transactions serially (one after another) leads to unacceptable CPU idle time and poor throughput. However, interleaved execution of concurrent operations on shared data items can cause data corruption if not strictly managed.

```
       +-------------------------------------------------------+
       |               CONCURRENCY READ/WRITE ANOMALIES        |
       +-------------------------------------------------------+
       | 1. Dirty Read Anomaly (Read Uncommitted Data)         |
       | 2. Non-Repeatable Read Anomaly (Fuzzy Read)           |
       | 3. Phantom Read Anomaly (Tuples inserted concurrently)|
       | 4. Lost Update Anomaly (Overwritten Write)            |
       +-------------------------------------------------------+
```

---

## 2. Read/Write Operations & Race Anomalies

### 2.1 The Four Classic Concurrency Anomalies

#### 1. Dirty Read Anomaly (Read Uncommitted Data)
* **Definition**: Occurs when Transaction $T_2$ reads a data item modified by Transaction $T_1$ while $T_1$ is still uncommitted. If $T_1$ subsequently aborts, $T_2$ has read data that "never existed".

```
     T1                      T2
     --------------------------
     Write(X) [v1 -> v2]
                             Read(X) [Reads v2]
     ABORT! [X restored to v1]
                             (T2 relies on dirty v2!)
```

#### 2. Non-Repeatable Read Anomaly (Fuzzy Read)
* **Definition**: Occurs when Transaction $T_1$ reads a data item twice, but gets **different scalar values** because Transaction $T_2$ updated and committed that data item between the two reads.

```
     T1                      T2
     --------------------------
     Read(X) [Value: 100]
                             Write(X) [Value: 200]
                             COMMIT
     Read(X) [Value: 200]    <-- Value changed within same T1 transaction!
```

#### 3. Phantom Read Anomaly
* **Definition**: Occurs when Transaction $T_1$ executes a range query (e.g., `SELECT COUNT(*) WHERE Age > 30`) twice, but receives a **different set of matching rows** because Transaction $T_2$ inserted or deleted a row matching that range predicate between the two queries.

```
     T1                                T2
     ------------------------------------
     SELECT COUNT(*) WHERE Age > 30 (5 rows)
                                       INSERT INTO Student (Age: 35)
                                       COMMIT
     SELECT COUNT(*) WHERE Age > 30 (6 rows!) <-- Phantom tuple appeared!
```

#### 4. Lost Update Anomaly
* **Definition**: Occurs when two transactions concurrently read the same data item and both attempt to write an update. The second write completely overwrites the update made by the first transaction without incorporating it.

```
     T1                      T2
     --------------------------
     Read(X) [100]
                             Read(X) [100]
     X := X + 50
     Write(X) [150]
                             X := X + 20
                             Write(X) [120] <-- T1's addition of +50 is completely lost!
```

---

## 3. SQL Isolation Levels & Anomalies Matrix

ANSI SQL standard defines four Isolation Levels to manage concurrency trade-offs:

| Isolation Level | Dirty Read | Non-Repeatable Read | Phantom Read | Lost Update |
| :--- | :--- | :--- | :--- | :--- |
| **READ UNCOMMITTED** | **Possible** | **Possible** | **Possible** | **Possible** |
| **READ COMMITTED** | **Prevented** | **Possible** | **Possible** | **Prevented** |
| **REPEATABLE READ** | **Prevented** | **Prevented** | **Possible** | **Prevented** |
| **SERIALIZABLE** | **Prevented** | **Prevented** | **Prevented** | **Prevented** |

---

## 4. Schedules and Equivalence

A **Schedule** $S$ represents the chronological execution sequence of operations across multiple concurrent transactions.

* **Serial Schedule**: Operations of each transaction are executed consecutively to completion without interleaving. (Guarantees correctness, but low throughput).
* **Non-Serial Schedule**: Operations of concurrent transactions are interleaved.

```
Serial Schedule:                  Non-Serial Schedule (Interleaved):
   T1           T2                   T1           T2
-----------  -----------          -----------  -----------
Read(A)                           Read(A)
Write(A)                          Write(A)
Read(B)                                        Read(A)
Write(B)                          Read(B)
             Read(A)                           Write(A)
             Write(A)             Write(B)
```

---

## 5. Conflict Serializability

A schedule $S$ is **Conflict Serializable** if it is conflict-equivalent to some serial schedule $S'$.

### 5.1 Conflicting Operations
Two operations $O_i$ and $O_j$ belonging to different transactions $T_i$ and $T_j$ are in **Conflict** if and only if:
1. They belong to **different transactions** ($T_i \neq T_j$).
2. They access the **same data item** ($X$).
3. At least one of the operations is a **`WRITE` operation** ($W(X)$).

#### Conflict Matrix:

| Operation Pair | Same Item? | Different Transactions? | Conflicting? |
| :--- | :--- | :--- | :--- |
| $R_i(X), R_j(X)$ | Yes | Yes | **No Conflict** |
| $R_i(X), W_j(X)$ | Yes | Yes | **CONFLICT!** (Read-Write Conflict) |
| $W_i(X), R_j(X)$ | Yes | Yes | **CONFLICT!** (Write-Read Conflict) |
| $W_i(X), W_j(X)$ | Yes | Yes | **CONFLICT!** (Write-Write Conflict) |

---

### 5.2 Precedence Graph (Serialization Graph) Algorithm

To test if a non-serial schedule $S$ is Conflict Serializable:

#### Graph Construction Rules:
1. Create a directed Graph $G = (V, E)$ where vertices $V$ represent transactions $\{T_1, T_2, \dots, T_n\}$.
2. Add a directed edge $T_i \rightarrow T_j$ if there exists an operation $O_i$ in $T_i$ that **conflicts** with operation $O_j$ in $T_j$, and $O_i$ executed **before** $O_j$ in schedule $S$.

```
               PRECEDENCE GRAPH CONFLICT EDGE DERIVATION:
               1. Read(X) in Ti  ---> Write(X) in Tj  =>  Edge Ti -> Tj
               2. Write(X) in Ti ---> Read(X) in Tj   =>  Edge Ti -> Tj
               3. Write(X) in Ti ---> Write(X) in Tj  =>  Edge Ti -> Tj
```

### Conflict Serializability Theorem:
$$\text{A Schedule } S \text{ is Conflict Serializable } \iff \text{Its Precedence Graph } G \text{ contains NO CYCLES.}$$

* If **Acyclic**: Topological sorting of $G$ gives the equivalent **Serial Execution Order**.
* If **Cyclic**: The schedule is **NOT Conflict Serializable**.

---

## 6. Conceptual Practice Exercises

1. Given Schedule $S: R_1(X), W_2(X), W_1(X), W_2(Y), C_1, C_2$. Draw the precedence graph and test for conflict serializability.
2. What is the difference between Conflict Serializability and View Serializability?
3. Which SQL isolation level prevents Non-Repeatable Reads but allows Phantom Reads?

---

## 7. Hard Placement & Interview Questions (FAANG Level)

### Q1: Prove mathematically why testing for View Serializability is an NP-Complete problem, whereas testing for Conflict Serializability takes $O(V + E)$ polynomial time.

**Answer:**
* **Conflict Serializability ($O(V+E)$)**:
  * Involves inspecting pairwise adjacent operations, identifying conflicting $R/W$ pairs, and building a directed precedence graph $G(V, E)$.
  * Checking for cycles in a graph takes linear time $O(V + E)$ using Depth-First Search (DFS) or Kahn's Topological Sort algorithm.

* **View Serializability (NP-Complete)**:
  * View serializability relaxes conflict equivalence rules by allowing blind writes ($W(X)$ without preceding $R(X)$).
  * Checking if schedule $S$ is view-equivalent to *some* serial schedule requires testing whether there exists *any* permutation of $n$ transactions ($n!$ possible serial schedules) satisfying initial read, updated read, and final write conditions.
  * Yannakakis (1984) formally proved that checking view serializability reduces to the 3-SAT Boolean Satisfiability problem, rendering it **NP-Complete**.
  * DBMS concurrency engines enforce conflict serializability or 2PL instead of view serializability.

---

### Q2: Consider Schedule $S: R_1(A), W_2(A), W_1(A), W_2(B), C_2, C_1$. Trace the precedence graph edges, identify all cycles, and determine if an equivalent serial order exists.

**Answer:**
1. **Identify Conflicting Operations**:
   * $R_1(A)$ occurs before $W_2(A)$ on item $A \implies$ **Edge $T_1 \rightarrow T_2$**.
   * $W_2(A)$ occurs before $W_1(A)$ on item $A \implies$ **Edge $T_2 \rightarrow T_1$**.
2. **Precedence Graph Construction**:
   * Vertices: $\{T_1, T_2\}$.
   * Edges: $T_1 \rightarrow T_2$ and $T_2 \rightarrow T_1$.
3. **Graph Evaluation**:
   * The graph contains a **Cycle**: $T_1 \rightarrow T_2 \rightarrow T_1$.
4. **Conclusion**:
   * Because a cycle exists, Schedule $S$ is **NOT Conflict Serializable**. No equivalent serial schedule exists.

---

### Q3: How do database storage engines eliminate Phantom Reads under the `SERIALIZABLE` isolation level? Compare Predicate Locking vs. Index-Range Locks (Next-Key Locks).
**Answer:**
1. **Predicate Locking**:
   * Locks the search condition itself (e.g., `WHERE Age > 30`).
   * *Drawback*: Evaluating predicate overlaps against incoming concurrent `INSERT` queries requires heavy mathematical expression evaluation, rendering it too slow for production use.

2. **Next-Key Locking (InnoDB / Standard approach)**:
   * Combines an Index-Record lock on existing records with a **Gap Lock** on the key space gaps before and between records.
   * *Example*: If index contains values 10 and 20, executing `WHERE Age > 15` acquires a Gap lock on interval $(10, 20)$ and Next-Key lock on $(20, +\infty)$.
   * Any concurrent transaction attempting `INSERT INTO Student (Age = 18)` is blocked when attempting to acquire an insert intention lock in the locked gap.
