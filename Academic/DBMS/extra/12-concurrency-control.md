# Lecture 12: Concurrency Control in DBMS

## 1. Why Concurrency Control?

Real-world DBMSs serve many users/transactions simultaneously for performance reasons (better throughput, reduced waiting time). But letting transactions run concurrently, unmanaged, can violate the **Isolation** property from ACID and produce incorrect results. Concurrency control is the set of techniques used to interleave transactions safely.

## 2. Problems Caused by Uncontrolled Concurrency

### a. Dirty Read Problem
A transaction reads data that has been written by another transaction that **has not yet committed**. If that other transaction later rolls back, the first transaction has now read data that never "really" existed.

*Example*: T1 updates balance to 5000 (uncommitted). T2 reads balance = 5000. T1 rolls back (balance reverts to 4000). T2 now has stale/incorrect data.

### b. Lost Update Problem
Two transactions read the same data, and both write back updated values — the second write overwrites (loses) the first transaction's update entirely.

*Example*: Both T1 and T2 read `balance = 1000`. T1 computes `1000+500=1500` and writes it. T2 (using its own stale read of 1000) computes `1000+300=1300` and writes it, **overwriting** T1's update — the +500 update is lost.

### c. Unrepeatable Read Problem
A transaction reads the same data item **twice** and gets **different values** each time, because another committed transaction modified it in between the two reads.

*Example*: T1 reads `balance = 1000`. T2 updates and commits `balance = 1200`. T1 reads `balance` again within the same transaction and now gets `1200` — inconsistent with its first read.

### d. Phantom Read Problem
A transaction re-executes a query returning a set of rows matching a condition, and finds that the **set of rows has changed** (new rows inserted or existing ones deleted matching the condition) because another transaction committed in between.

*Example*: T1 runs `SELECT * FROM Orders WHERE amount > 1000` → gets 5 rows. T2 inserts a new order with amount 1500 and commits. T1 re-runs the same query → now gets 6 rows — a "phantom" row appeared.

## 3. Schedules

A **schedule** is a sequence of interleaved operations (read/write) from multiple transactions, representing the actual chronological order in which the DBMS executes them.

### a. Serial Schedule
Transactions execute **one completely after another**, with no interleaving at all. Serial schedules are always consistent (correct) by definition, but offer poor performance/throughput.

### b. Non-Serial (Concurrent) Schedule
Operations from different transactions are **interleaved**. May or may not be correct, depending on the specific interleaving.

## 4. Serializability

Since serial schedules are guaranteed correct but slow, and we want the performance benefits of non-serial schedules, we need a way to check whether a given non-serial schedule is "as good as" some serial schedule — this property is called **serializability**.

A schedule is **serializable** if its effect on the database (and the final resulting state) is **equivalent** to some serial schedule of the same set of transactions.

### Two Common Types

1. **Conflict Serializability**
   Two operations are said to **conflict** if:
   - They belong to different transactions, AND
   - They operate on the same data item, AND
   - At least one of them is a **write** operation.
   
   (Read-Read pairs never conflict; only Read-Write, Write-Read, and Write-Write pairs conflict.)

   A schedule is **conflict-serializable** if it can be transformed into a serial schedule by repeatedly **swapping adjacent non-conflicting operations**, without changing the relative order of conflicting operations.

   **Testing method — Precedence Graph (Serialization Graph)**:
   - Draw a node for each transaction.
   - Draw a directed edge `Ti → Tj` if `Ti` has an operation that conflicts with, and occurs before, an operation of `Tj` on the same data item.
   - If the resulting graph has **no cycles**, the schedule is conflict-serializable (and the topological order of the graph gives a valid equivalent serial order). If there IS a cycle, the schedule is **not** conflict-serializable.

2. **View Serializability**
   A more general (but harder to test in practice — NP-complete) notion, where a schedule is view-serializable if it is "view equivalent" to some serial schedule: same initial reads, same final writes, and same read-from relationships for every data item across transactions. Every conflict-serializable schedule is also view-serializable, but not vice versa.

## 5. Recoverability of Schedules

Independent of serializability, schedules are also classified by whether they can be safely **recovered** in the event that a transaction aborts:

1. **Recoverable Schedule**: If `Tj` reads a value written by `Ti`, then `Ti` must **commit before** `Tj` commits.
2. **Cascadeless Schedule**: If `Tj` reads a value written by `Ti`, then `Ti` must have **already committed** before `Tj` even performs that read (stricter than just "before Tj commits") — avoids cascading rollbacks.
3. **Strict Schedule**: A transaction can neither read nor write a data item until the transaction that last wrote it has committed or aborted — the strictest and simplest to reason about, and what most real systems implement via locking.

## Summary
Uncontrolled concurrent execution of transactions can cause dirty reads, lost updates, unrepeatable reads, and phantom reads. Serial schedules avoid these problems by construction but sacrifice performance. **Conflict serializability**, tested via a precedence graph for cycles, gives us a practical way to allow interleaved (non-serial) execution while still guaranteeing correctness equivalent to some serial order. Separately, schedules must also be recoverable (ideally cascadeless or strict) to handle transaction aborts safely. The next lecture covers the actual **protocols** (locking, timestamping) that DBMSs use to enforce these properties in practice.

## 6. Practical Note: How Precedence Graphs Are Used

To test conflict serializability of a schedule `S`:
1. List all pairs of conflicting operations across different transactions.
2. Draw an edge `Ti → Tj` for each case where `Ti`'s operation precedes and conflicts with `Tj`'s operation.
3. Check the graph for cycles using standard graph algorithms (DFS-based cycle detection).
4. No cycle → conflict-serializable; a valid equivalent serial order is any topological sort of the graph.

## 7. Blind Writes and View Serializability Nuance

A **blind write** is a write operation on a data item that was never read by the same transaction beforehand. View serializability specifically differs from conflict serializability in how it treats schedules containing blind writes — a schedule with blind writes might be view-serializable without being conflict-serializable, since view serializability only cares about the final "who reads from whom" and "final writer" relationships, not the exact conflict ordering.

---

## Placement Interview Questions — Lecture 12

**Q1. What is a schedule? Differentiate between serial and non-serial schedules.**
A schedule is a sequence of interleaved operations from multiple transactions. A serial schedule executes transactions one completely after another (no interleaving); a non-serial schedule interleaves operations from different transactions.

**Q2. Explain the dirty read problem with an example.**
A transaction reads uncommitted data written by another transaction. If that other transaction later rolls back, the first transaction has read data that never truly existed. Example: T1 updates balance to 5000 (uncommitted), T2 reads 5000, T1 rolls back to 4000 — T2 now has incorrect data.

**Q3. Explain the lost update problem with an example.**
Two transactions read the same value and both write updated versions back; the second write overwrites (loses) the first transaction's update entirely, since it was based on stale data.

**Q4. Differentiate between unrepeatable read and phantom read.**
Unrepeatable read: a transaction re-reads the *same row* and gets a different value because another transaction modified and committed a change to it in between. Phantom read: a transaction re-runs the *same query* and gets a different *set of rows* because another transaction inserted/deleted rows matching the query's condition.

**Q5. What makes two operations "conflict" with each other?**
They belong to different transactions, operate on the same data item, and at least one of them is a write operation (Read-Read pairs never conflict).

**Q6. What is conflict serializability? How is it tested?**
A schedule is conflict-serializable if it can be transformed into a serial schedule by swapping adjacent non-conflicting operations. Tested using a precedence graph: draw an edge for each conflicting operation pair in execution order; if the graph has no cycles, the schedule is conflict-serializable.

**Q7. What is the difference between conflict serializability and view serializability?**
Conflict serializability requires that the relative order of all conflicting operations matches some serial schedule. View serializability is more general — it only requires the same initial reads, final writes, and "read-from" relationships as some serial schedule, allowing some schedules with blind writes to qualify even if they aren't conflict-serializable. Every conflict-serializable schedule is view-serializable, but not vice versa.

**Q8. What is a recoverable schedule?**
A schedule where, if transaction Tj reads a value written by Ti, then Ti must commit before Tj commits — ensuring that if Ti later aborts, Tj (which read its uncommitted data) hasn't already committed based on that invalid data.

**Q9. Differentiate between cascadeless and strict schedules.**
Cascadeless: Tj can only read a value written by Ti after Ti has already committed (prevents cascading rollbacks). Strict: even stronger — Tj cannot read OR write a data item until the transaction that last wrote it has committed or aborted.

**Q10. Why do we prefer non-serial (interleaved) schedules over serial schedules despite the added complexity of ensuring correctness?**
Because serial execution (one transaction fully completing before the next starts) severely limits throughput and increases wait times; interleaving transactions allows much better resource utilization and performance, as long as we can guarantee (via serializability) that the interleaving doesn't produce incorrect results.
