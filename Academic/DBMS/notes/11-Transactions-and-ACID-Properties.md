# Lecture 11: Transactions and ACID Properties

---

## 1. Introduction to Database Transactions

A **Transaction** is a logical unit of database processing that includes one or more database operations (such as `READ`, `WRITE`, `INSERT`, `DELETE`). It transitions a database from one consistent state to another consistent state.

```
[ Consistent State S1 ] ---- Transaction T ----> [ Consistent State S2 ]
```

---

## 2. Transaction State Diagram

During execution, a transaction passes through a sequence of execution states managed by the DBMS Transaction Manager.

```
                  +-------------------+
                  |      ACTIVE       |
                  +-------------------+
                    /               \
                   /                 \
                  v                   v
      +-------------------+   +-------------------+
      | PARTIALLY COMMITTED|  |      FAILED       |
      +-------------------+   +-------------------+
                |                       |
                v                       v
      +-------------------+   +-------------------+
      |     COMMITTED     |   |     ABORTED       |
      +-------------------+   +-------------------+
```

### Transaction States Explained:

1. **Active**: Initial state. Transaction remains active while executing `READ` and `WRITE` operations.
2. **Partially Committed**: Entered right after the final operation of the transaction has been executed in memory, but log buffers have not yet been flushed to disk.
3. **Committed**: Entered after successful completion. All log records flushed to disk (`fsync`). Database modifications are permanent.
4. **Failed**: Entered when normal execution can no longer proceed due to hardware error, concurrency deadlock, or logical constraint violation (`CHECK` failure).
5. **Aborted**: Entered after the transaction has been rolled back and the database has been restored to its pre-transaction state.
   * *Post-Abort Options*: Restart transaction (if system error) or Kill transaction (if bad user input logic).

---

## 3. The ACID Properties

To guarantee integrity, reliability, and correctness under concurrent access and system crashes, database systems enforce the four **ACID** properties.

```
       +-------------------------------------------------------+
       |                  THE ACID GUARANTEES                  |
       +-------------------------------------------------------+
       | ATOMICITY   | All operations complete or none do.     |
       | CONSISTENCY | Database transitions between valid states|
       | ISOLATION   | Concurrent transactions don't interfere |
       | DURABILITY  | Committed updates survive system crashes|
       +-------------------------------------------------------+
```

---

### 3.1 Atomicity ("All-or-Nothing")

* **Definition**: A transaction is an indivisible, atomic unit of work. Either **ALL** of its operations are executed successfully and permanently applied, or **NONE** of them are.
* **Failure Scenario**: Bank Transfer of $100 from Account A to Account B:
  1. $A := A - 100$
  2. *Crash occurs!*
  3. $B := B + 100$ (Never executes).
* **DBMS Enforcement**: 
  * Implemented using **Undo Logging** (Log-Based Recovery) or **Shadow Paging**.
  * If a crash occurs midway, the DBMS reads the undo log records and rolls back all partial writes performed by uncommitted transaction $T$.

---

### 3.2 Consistency ("Preserving Application Invariants")

* **Definition**: A transaction must transform the database from one valid consistent state (satisfying all domain rules, foreign keys, unique checks, and business logic invariants) to another valid consistent state.
* **Formula**: $\sum \text{Balances}_{\text{Before}} = \sum \text{Balances}_{\text{After}}$
* **Responsibility**: Joint responsibility between **Database Engine** (enforcing schema DDL constraints) and **Application Programmer** (writing correct transaction logic).

---

### 3.3 Isolation ("Concurrent Execution Abstraction")

* **Definition**: The execution of a transaction concurrently with other transactions must be completely isolated, as if it were executing alone in a single-threaded sequential environment.
* **Why needed**: Prevents concurrency anomalies (Dirty Reads, Unrepeatable Reads, Lost Updates).
* **DBMS Enforcement**: Implemented using **Concurrency Control Protocols** (2-Phase Locking, Timestamp Ordering, Multi-Version Concurrency Control - MVCC).

---

### 3.4 Durability ("Permanence of Committed Data")

* **Definition**: Once a transaction successfully commits, its updates to the database are **permanent** and will survive any subsequent system failure, power outage, or OS crash.
* **DBMS Enforcement**:
  * Implemented using **Write-Ahead Logging (WAL)** and **Redo Logs**.
  * Rule: Before a transaction is marked as committed, all associated Redo log records MUST be flushed to non-volatile physical storage (disk/SSD).

---

## 4. Summary Matrix: ACID Responsibilities

| ACID Property | Primary Risk | DBMS Component Responsible |
| :--- | :--- | :--- |
| **Atomicity** | Partial crashes, power failure | Recovery Manager / Transaction Manager (Undo Logs) |
| **Consistency** | Business logic & constraint violation | Developer Logic + DBMS Constraint Enforcer |
| **Isolation** | Concurrent race conditions | Concurrency Control Manager (Locks / MVCC) |
| **Durability** | OS crash, disk failures after commit | Recovery Manager (Redo Logs / WAL / Disk Flushes) |

---

## 5. Conceptual Practice Exercises

1. Trace the transaction state transitions for a transaction that encounters a division-by-zero SQL error during execution.
2. Differentiate between `Partially Committed` and `Committed` states. Why can a transaction still fail in the `Partially Committed` state?
3. Explain why storing log records on the same physical disk partition as data files increases durability risk.

---

## 6. Hard Placement & Interview Questions (FAANG Level)

### Q1: Explain the WAL (Write-Ahead Logging) protocol protocol and the Steal/No-Force buffer management policy. How do they guarantee Atomicity and Durability simultaneously?

**Answer:**
Buffer Pool Eviction Policies are categorized into two independent dimensions:

1. **STEAL vs. NO-STEAL Policy (Pertains to Atomicity)**:
   * **STEAL**: The DBMS can flush uncommitted dirty pages to disk to make space in the memory buffer pool for other queries. (Requires **Undo** logging to roll back changes if transaction aborts).
   * **NO-STEAL**: Uncommitted dirty pages can NEVER be written to disk until transaction commits. (Avoids Undo logging, but memory-expensive).

2. **FORCE vs. NO-FORCE Policy (Pertains to Durability)**:
   * **FORCE**: All modified pages MUST be written to disk before commit completes. (High I/O latency penalty).
   * **NO-FORCE**: Transaction commits as soon as log records are written to disk; dirty data pages can remain in RAM indefinitely. (Requires **Redo** logging to reconstruct data if system crashes).

**WAL Protocol Guarantee**:
* Modern enterprise systems use **STEAL / NO-FORCE** for maximum performance.
* **WAL Rule**: 
  1. Undo logs flushed to disk *before* dirty data page is written to disk (STEAL requirement).
  2. Redo logs flushed to disk *before* transaction returns commit success (NO-FORCE requirement).

---

### Q2: What is a "Cascading Abort" (Cascading Rollback), and why is it a nightmare in high-throughput transaction processing systems?
**Answer:**
* **Definition**: A situation where the failure of one transaction $T_1$ triggers an automatic chain reaction of rollbacks across multiple dependent concurrent transactions $T_2, T_3, \dots, T_n$.
* **Scenario**:
  1. $T_1$ writes data item $X$.
  2. $T_2$ reads $X$ (uncommitted dirty value written by $T_1$).
  3. $T_2$ computes new values based on $X$ and writes $Y$.
  4. $T_3$ reads $Y$.
  5. $T_1$ aborts / fails!
  6. Because $T_1$'s write was invalid, $T_2$'s read was dirty and invalid. $T_2$ MUST be aborted. Because $T_2$ aborted, $T_3$ MUST be aborted!
* **Resolution**: Systems use **Cascadeless Schedules** (or Strict 2PL), ensuring transactions only read COMMITTED data values.

---

### Q3: How does Multi-Version Concurrency Control (MVCC) eliminate read-write blocking conflicts while maintaining snapshot isolation?
**Answer:**
* In MVCC (used by PostgreSQL, InnoDB, Oracle):
  * A `WRITE` operation does not overwrite the existing data row in place. Instead, it creates a new tuple version stamped with the transaction's commit timestamp / Transaction ID (`xmin`, `xmax`).
  * A `READ` operation selects the latest tuple version whose timestamp is less than or equal to the reading transaction's snapshot start timestamp.
  * **Result**: **"Readers never block Writers, and Writers never block Readers."**
