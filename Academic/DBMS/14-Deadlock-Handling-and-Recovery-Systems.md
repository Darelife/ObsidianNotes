# Lecture 14: Deadlock Handling & Recovery Systems in DBMS

---

## 1. Deadlock Handling Mechanisms

A **Deadlock** occurs in a concurrent database system when two or more transactions are in a circular wait state, where each transaction holds a lock on a data item that another transaction requires to proceed.

```
       [ Transaction T1 ] --- Holds Lock on X ---> ( Data Item X )
               ^                                        |
               |                                 Needs Lock on X
        Needs Lock on Y                                 |
               |                                        v
       ( Data Item Y ) <--- Holds Lock on Y --- [ Transaction T2 ]
```

---

### 1.1 Deadlock Prevention Protocols

Deadlock prevention protocols use transaction timestamps to decide whether a transaction should **wait** or **die/wound** when a lock collision occurs, preventing circular wait conditions.

Assume $T_i$ requests a lock held by $T_j$.

```
                        DEADLOCK PREVENTION SCHEMES
                        /                         \
                       /                           \
            1. WAIT-DIE SCHEME             2. WOUND-WAIT SCHEME
            (Non-Preemptive)               (Preemptive)
            If Ti is OLDER than Tj:        If Ti is OLDER than Tj:
              Ti WAITS.                      Ti WOUNDS (Aborts) Tj.
            If Ti is YOUNGER than Tj:      If Ti is YOUNGER than Tj:
              Ti DIES (Aborts).              Ti WAITS.
```

#### Detailed Comparison:

| Protocol | Scenario: $T_i$ (Older) requests lock held by $T_j$ (Younger) | Scenario: $T_i$ (Younger) requests lock held by $T_j$ (Older) | Behavior Style |
| :--- | :--- | :--- | :--- |
| **Wait-Die** | $T_i$ **Waits** | $T_i$ **Dies (Aborts & Restarts)** | Non-preemptive (Older wait, younger die) |
| **Wound-Wait**| $T_i$ **Wounds $T_j$ (Aborts $T_j$)** | $T_i$ **Waits** | Preemptive (Older wounds younger) |

---

### 1.2 Deadlock Detection & Wait-For Graph (WFG)

If deadlock prevention is not used, the system allows locks to be requested freely and periodically runs a **Deadlock Detection** algorithm using a **Wait-For Graph (WFG)**.

#### Wait-For Graph (WFG) Definition:
* **Nodes ($V$)**: Active transactions $\{T_1, T_2, \dots, T_n\}$.
* **Directed Edge ($T_i \rightarrow T_j$)**: Created when $T_i$ is waiting for $T_j$ to release a lock on a data item.

$$\text{A Deadlock exists } \iff \text{The Wait-For Graph contains a CYCLE.}$$

```
                WAIT-FOR GRAPH (WFG) DEADLOCK CYCLE:
                [ T1 ] ------------ Waiting for Lock ------------> [ T2 ]
                  ^                                                   |
                  |                                                   |
                  +----------------- Waiting for Lock ----------------+
```

---

### 1.3 Deadlock Recovery (Victim Selection)

When a cycle is detected in the WFG, the DBMS breaks the deadlock by selecting a **Victim Transaction** to abort and roll back.

#### Victim Selection Criteria:
1. **Transaction Cost / Progress**: Select transactions that have executed for the shortest time or modified the fewest data items.
2. **Number of Held Locks**: Select transactions holding the fewest locks.
3. **Rollback Cost**: Prefer partial rollback to savepoints over total transaction abort.
4. **Starvation Prevention**: Track how many times a transaction has been selected as a victim. Increase its priority over time so it is not starved continuously.

---

## 2. Failure Classification & Database Recovery

DBMS failure recovery systems restore the database to a consistent state following system crashes or physical storage failures.

### 2.1 Failure Classification

1. **Transaction Failure**:
   * *Logical Errors*: Internal transaction code error (divide by zero, constraint violation).
   * *System Errors*: Deadlock detection abort or explicit rollback request.
2. **System Crash (Soft Failure)**:
   * Power failure, OS kernel crash, or RAM failure causing volatile buffer pool contents to be lost. Non-volatile disk storage remains intact.
3. **Disk Failure (Hard Failure)**:
   * Physical disk sector corruption, head crash, or storage media failure destroying secondary storage data files.

---

## 3. Log-Based Recovery Mechanisms

The **Log** is an append-only sequential file stored on stable storage tracking all database modification operations.

### 3.1 Log Record Structure
Every log entry records an operational state change:
* `<T_i, Start>`: Transaction $T_i$ started.
* `<T_i, X, Old_V, New_V>`: $T_i$ updated item $X$ from `Old_V` (for Undo) to `New_V` (for Redo).
* `<T_i, Commit>`: $T_i$ committed successfully.
* `<T_i, Abort>`: $T_i$ aborted.

---

### 3.2 Deferred vs. Immediate Database Modification

```
+-------------------------------------------------------------------------------+
| RECOVERY METHOD           | WHEN DISK UPDATES OCCUR | LOG REQUIRES            |
+-------------------------------------------------------------------------------+
| Deferred Modification     | ONLY AFTER COMMIT       | Redo Values Only        |
| Immediate Modification    | AT ANY TIME (STEAL)    | Undo AND Redo Values    |
+-------------------------------------------------------------------------------+
```

#### 1. Deferred Modification (No-Steal / Redo-Only)
* Data writes are deferred in RAM buffer pool until transaction reaches commit point.
* If transaction fails *before* commit, zero disk writes occurred $\implies$ **NO UNDO needed**.
* Recovery scans log to **REDO** committed transactions.

#### 2. Immediate Modification (Steal / Undo-Redo)
* Uncommitted dirty buffer pages can be flushed to disk at any time before commit.
* If transaction fails *before* commit, modified data pages on disk must be reverted $\implies$ **UNDO required**.
* Recovery requires two phases: **UNDO active uncommitted transactions** and **REDO committed transactions**.

---

### 3.3 Checkpointing

Scanning the entire log file from the beginning of time during crash recovery is computationally prohibitive. **Checkpointing** periodically flushes dirty pages and creates a snapshot boundary in the log file.

```
Log File:
[ Start T1 ] ... [ Update X ] ... < CHECKPOINT (Active: T2, T3) > ... [ Commit T2 ] ... [ CRASH! ]
                                         ^
                                         |
                            Recovery scan only goes back
                            to this Checkpoint record!
```

#### ARIES Crash Recovery Algorithm (3 Phases):
1. **Analysis Phase**: Scans log **forward** from the last checkpoint to identify dirty pages and active transactions (Dirty Page Table & Transaction Table) at the time of crash.
2. **Redo Phase ("Repeating History")**: Scans log **forward** from the minimum `RecLSN` to re-apply all logged updates (for both committed and uncommitted transactions) to restore the state at crash time.
3. **Undo Phase**: Scans log **backward** to roll back updates of all active uncommitted transactions (loser transactions), writing Compensation Log Records (CLRs) to prevent infinite loops during repeated crashes.

---

## 4. Conceptual Practice Exercises

1. Trace the Wait-Die scheme for transactions $T_1 (TS=10)$ and $T_2 (TS=20)$ when $T_2$ requests a lock held by $T_1$.
2. Why does the Immediate Database Modification technique require both `Old_Value` and `New_Value` in log entries, while Deferred Modification only requires `New_Value`?
3. What is a Compensation Log Record (CLR) in ARIES recovery, and why does it prevent cascading crash loops during undo recovery?

---

## 5. Hard Placement & Interview Questions (FAANG Level)

### Q1: Given the following log contents at crash time:
```text
1. <T1, Start>
2. <T1, A, 10, 20>
3. <T2, Start>
4. <T2, B, 30, 40>
5. <T1, Commit>
6. <CHECKPOINT (T2)>
7. <T3, Start>
8. <T3, C, 50, 60>
9. <T2, B, 40, 50>
10. <T2, Commit>
11. <T4, Start>
12. <T4, D, 70, 80>
--- SYSTEM CRASH ---
```
### Trace the REDO and UNDO lists during Immediate Modification recovery.

**Answer:**
1. **Analyze Log for Transaction States at Crash Time**:
   * $T_1$: Started before checkpoint, committed at line 5 (Committed).
   * $T_2$: Started at line 3, committed at line 10 (Committed).
   * $T_3$: Started at line 7, **no commit/abort record** $\implies$ **Uncommitted (Loser)**.
   * $T_4$: Started at line 11, **no commit/abort record** $\implies$ **Uncommitted (Loser)**.

2. **Categorize Transactions**:
   * **REDO List**: $\{ T_2 \}$ (Committed after checkpoint). Note: $T_1$ committed before checkpoint and updates were flushed.
   * **UNDO List**: $\{ T_3, T_4 \}$ (Active/Uncommitted at crash time).

3. **Execution Steps**:
   * **REDO Phase**: Re-apply updates for $T_2$: Line 9 set $B = 50$.
   * **UNDO Phase**: Roll back updates in reverse order for $T_4$ and $T_3$:
     * Line 12: Undo $T_4$'s update on $D$, restore $D = 70$.
     * Line 8: Undo $T_3$'s update on $C$, restore $C = 50$.
     * Write `<T4, Abort>` and `<T3, Abort>` log records.

---

### Q2: Compare Wait-Die vs Wound-Wait in terms of transaction abort frequency and starvation risks under workloads where long-running transactions repeatedly request locks held by short transactions.
**Answer:**
* **Wait-Die (Non-preemptive)**:
  * If a younger transaction requests a lock held by an older transaction, the younger transaction dies and restarts.
  * *Drawback*: A younger transaction may die and restart repeatedly multiple times while waiting for the same long-running older transaction to finish. Produces **higher abort count**.
* **Wound-Wait (Preemptive)**:
  * If an older transaction requests a lock held by a younger transaction, the older transaction "wounds" (aborts) the younger transaction.
  * *Advantage*: When an older transaction requests a lock, it aborts the younger holder immediately. Fewer total aborts occur because older transactions preemptively sweep through without blocking.
* **Conclusion**: Wound-Wait minimizes transaction abort frequency compared to Wait-Die under high lock contention.

---

### Q3: Why is Fuzzy Checkpointing preferred over Strict Checkpointing in commercial high-throughput databases?
**Answer:**
* **Strict Checkpointing**: Freezes all buffer pool transactions, flushes every dirty page in RAM to disk, writes checkpoint record, and resumes queries.
  * *Drawback*: Causes severe system I/O latency spikes (stop-the-world pauses) lasting several seconds.
* **Fuzzy Checkpointing**:
  * Does NOT freeze active transactions or force immediate flushing of all dirty pages.
  * Writes a snapshot of the **Dirty Page Table (DPT)** and **Transaction Table** to the log file while background I/O threads continuously write dirty pages to disk asynchronously.
  * *Result*: Zero query latency pauses while preserving full crash recovery invariants.
