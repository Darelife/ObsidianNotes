# Lecture 14: Deadlock Handling & Recovery Systems in DBMS

## PART A: Deadlock Handling

### 1. What is a Deadlock?

A deadlock occurs when two or more transactions are each waiting for a lock held by another transaction in the same set, forming a **circular wait**, so that none of them can ever proceed.

*Example*: T1 holds a lock on A and requests a lock on B. T2 holds a lock on B and requests a lock on A. Neither can proceed — a deadlock.

### 2. Deadlock Handling Strategies

There are three broad strategies:

#### a. Deadlock Prevention
Ensure the system **never enters** a state that could lead to deadlock, typically by imposing an ordering on transactions using timestamps, deciding in advance whether a transaction should wait or be rolled back when it requests a lock held by another.

**Wait-Die Scheme** (non-preemptive — older transaction waits):
- If requesting transaction `Ti` is **older** than the lock-holding transaction `Tj` (i.e., `TS(Ti) < TS(Tj)`) → `Ti` is allowed to **wait**.
- If `Ti` is **younger** than `Tj` → `Ti` is **rolled back (dies)**, and may restart later with the same original timestamp.

**Wound-Wait Scheme** (preemptive — older transaction "wounds"/preempts younger):
- If `Ti` is **older** than `Tj` → `Ti` **wounds** `Tj`, forcing `Tj` to roll back and release its locks; `Ti` proceeds.
- If `Ti` is **younger** than `Tj` → `Ti` is allowed to **wait**.

Both schemes guarantee no deadlock because they always allow only one direction of waiting (younger never waits for older to wound, avoiding a cycle), and both restart aborted transactions with their **original timestamp** to prevent starvation (so an old transaction eventually becomes the oldest and always wins).

#### b. Deadlock Avoidance
Similar in spirit to prevention but decides more dynamically — evaluate resource requests and grant them only if the resulting state is provably "safe" (won't lead to deadlock), akin to the Banker's Algorithm from operating systems. Less commonly implemented in DBMSs compared to detection.

#### c. Deadlock Detection and Recovery
Allow deadlocks to occur, but periodically check for them and resolve any found.

**Wait-For Graph**:
- A directed graph where each node is a transaction.
- A directed edge `Ti → Tj` exists if `Ti` is waiting for a lock currently held by `Tj`.
- The DBMS periodically runs a **cycle-detection algorithm** on this graph.
- **A cycle in the wait-for graph indicates a deadlock.**

**Recovery from Detected Deadlock**:
Once a cycle is found, one or more transactions must be chosen as the **victim(s)** and rolled back to break the cycle. Victim selection considers:
- How much work the transaction has already done (prefer rolling back transactions with less progress).
- How many data items the transaction has used/how many more it needs.
- How many transactions will need to be rolled back (minimize cascading effects).
- **Starvation avoidance**: ensure the same transaction isn't repeatedly picked as the victim — often solved by factoring in the number of times a transaction has already been rolled back.

**Rollback options**:
- **Total rollback**: abort the transaction entirely and restart it.
- **Partial rollback**: roll back only as far as necessary to release the specific locks causing the deadlock, then retry from that point — more efficient, but harder to implement correctly.

## PART B: Failure Classification and Recovery

### 3. Types of Failures

1. **Transaction Failure**: Caused by logical errors (e.g., bad input data, integrity constraint violation) or system errors (e.g., deadlock victim). Affects a single transaction.
2. **System Crash**: Hardware malfunction, software bug (OS or DBMS), causing loss of the contents of volatile (main) memory, but the content of non-volatile storage (disk) remains intact and correct — assumed under the **fail-stop** assumption.
3. **Disk Failure**: A disk block loses its content, either due to a head crash or failure during a data transfer operation — requires restoring from backups plus replaying logs.

### 4. Log-Based Recovery

The **transaction log** is a sequential record of all update operations performed by transactions, stored on stable (non-volatile) storage. Each log record typically contains: transaction ID, data item, old value, new value.

Two key approaches for how updates are applied relative to logging:

#### a. Deferred Database Modification
- All writes made by a transaction are recorded **only in the log** first; actual updates to the database itself are **deferred** until the transaction reaches its **commit point**, at which time the log is used to actually perform the writes.
- Since the database is never modified before commit, there is **no need for UNDO** during recovery — only **REDO** of committed transactions is needed (in case the system crashed after commit but before all writes were flushed to disk).

#### b. Immediate Database Modification
- Updates are applied to the database **immediately**, even before the transaction commits — but the log entry (with old and new values) is always written **before** the actual database update (this ordering itself is called **Write-Ahead Logging, WAL**).
- Because uncommitted changes may already be on disk, recovery needs **both**:
  - **UNDO**: revert changes made by transactions that had **not committed** at the time of the crash (using the "old value" from the log).
  - **REDO**: reapply changes made by transactions that **had committed** but whose changes might not have been fully flushed to disk (using the "new value" from the log).

### 5. Checkpoints

Scanning the entire log from the beginning during recovery is expensive as the log grows over time. A **checkpoint** is a periodic snapshot marker: at the checkpoint, the DBMS ensures all committed transactions' changes are flushed to disk and records which transactions were active at that moment. During recovery, the log only needs to be scanned back to the **most recent checkpoint**, not the entire history — significantly speeding up recovery.

## Summary
Deadlocks — circular waits among transactions for locks — can be handled by **prevention** (Wait-Die / Wound-Wait schemes based on transaction timestamps), or by **detection and recovery** using a wait-for graph and cycle detection, followed by careful victim selection to roll back and break the cycle. Separately, failure recovery relies on **write-ahead logging**, using either **deferred modification** (REDO only) or **immediate modification** (both UNDO and REDO), with **checkpoints** used to bound how much of the log must be replayed after a crash.

## PART C: Additional Depth

### 6. Necessary Conditions for Deadlock (Coffman Conditions)

A deadlock can only occur if all four of these conditions hold simultaneously:
1. **Mutual Exclusion** — at least one resource (lock) is held in a non-shareable mode.
2. **Hold and Wait** — a transaction holds at least one lock while waiting to acquire another.
3. **No Preemption** — locks cannot be forcibly taken away from a transaction; they must be released voluntarily.
4. **Circular Wait** — a closed chain of transactions exists, each waiting for a lock held by the next.

Breaking any ONE of these conditions prevents deadlock — this is the theoretical basis for prevention schemes.

### 7. ARIES Recovery Algorithm (Brief Mention)

Most modern DBMSs implement a sophisticated recovery algorithm called **ARIES** (Algorithm for Recovery and Isolation Exploiting Semantics), which uses a three-pass recovery process after a crash:
1. **Analysis Pass** — determines which transactions were active and which pages were dirty at the time of the crash.
2. **Redo Pass** — reapplies all logged changes (both committed and uncommitted) to restore the exact state at the time of the crash.
3. **Undo Pass** — rolls back the changes of transactions that were still active (not committed) at the time of the crash.

---

## Placement Interview Questions — Lecture 14

**Q1. What is a deadlock? Give an example.**
A circular wait where two or more transactions each hold a lock the other needs. E.g., T1 holds lock on A, wants lock on B; T2 holds lock on B, wants lock on A — neither can proceed.

**Q2. What are the four necessary conditions for a deadlock to occur (Coffman conditions)?**
Mutual exclusion, hold and wait, no preemption, and circular wait — all four must hold simultaneously for deadlock to be possible.

**Q3. Explain the Wait-Die and Wound-Wait deadlock prevention schemes.**
Both use transaction timestamps to decide whether to wait or abort. Wait-Die (non-preemptive): older transaction can wait for younger; younger requesting a lock held by older is aborted (dies). Wound-Wait (preemptive): older transaction preempts (wounds) younger, forcing it to abort; younger can wait for older.

**Q4. Why do Wait-Die and Wound-Wait guarantee no deadlock?**
Because they enforce a strict, consistent ordering (based on timestamps) for which transactions are allowed to wait — waiting can only happen in one direction, so a circular wait chain can never form.

**Q5. How is a deadlock detected? Explain the wait-for graph.**
Using a wait-for graph, where nodes represent transactions and a directed edge Ti → Tj means Ti is waiting for a lock held by Tj. The DBMS periodically checks this graph for cycles — a cycle indicates a deadlock.

**Q6. Once a deadlock is detected, how is it resolved?**
By selecting a "victim" transaction (based on factors like work done, resources held/needed, and cascading rollback impact) and rolling it back (partially or totally) to break the cycle, releasing its locks so other transactions can proceed.

**Q7. What is starvation in the context of deadlock handling, and how is it avoided?**
Starvation occurs when the same transaction keeps getting picked as the victim repeatedly and never completes. Avoided by factoring the number of past rollbacks into victim selection, ensuring an old, frequently-aborted transaction eventually gets priority.

**Q8. Differentiate between deferred and immediate database modification for recovery.**
Deferred: writes are recorded only in the log until commit; the database itself is updated only after commit — needs only REDO during recovery. Immediate: writes are applied to the database right away (before commit), following write-ahead logging — needs both UNDO (for uncommitted transactions) and REDO (for committed ones not yet flushed).

**Q9. What is Write-Ahead Logging (WAL)?**
The principle that a log entry describing a change must be written to stable storage BEFORE the actual change is applied to the database — ensures that recovery always has enough information to undo or redo any operation, regardless of when a crash occurs.

**Q10. What is a checkpoint, and why is it used in recovery?**
A periodic marker where the DBMS ensures all committed changes are flushed to disk and records active transactions at that moment — during recovery, the log only needs to be scanned back to the most recent checkpoint rather than from the very beginning, significantly speeding up recovery.

**Q11. Differentiate between total rollback and partial rollback.**
Total rollback aborts and restarts the entire transaction. Partial rollback undoes only as much as necessary to release the specific locks causing a deadlock, then retries from that point — more efficient but harder to implement correctly.
