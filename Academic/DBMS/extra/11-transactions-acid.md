# Lecture 11: Transactions and ACID Properties

## 1. What is a Transaction?

A **transaction** is a single logical unit of work performed on a database, consisting of one or more operations (read/write), which must execute completely, as an indivisible whole, or not at all.

*Classic example*: Transferring ₹500 from Account A to Account B requires two operations:
1. `Debit A by 500`
2. `Credit B by 500`

Either **both** operations succeed, or **neither** should take effect — leaving the account balances inconsistent halfway through is unacceptable.

## 2. States of a Transaction

A transaction moves through a well-defined state diagram during its lifetime:

1. **Active** — the initial state; the transaction is executing its operations.
2. **Partially Committed** — after the final operation has executed, but before the changes are permanently saved to disk.
3. **Committed** — the transaction has completed successfully and all changes are permanently saved.
4. **Failed** — the transaction cannot proceed normally (due to hardware failure, logical error, etc.) and cannot continue.
5. **Aborted** — the transaction has been rolled back, and the database is restored to its state before the transaction began. From here, the transaction may either be **restarted** or **killed**.

```
Active → Partially Committed → Committed
   ↓             ↓
 Failed  -------→  Aborted → (Restart / Kill)
```

## 3. ACID Properties

ACID is the set of four guarantees that a DBMS must provide for reliable transaction processing.

### A — Atomicity
"All or nothing." Every operation within a transaction either completes fully, or none of it takes effect. If any part fails, the entire transaction is rolled back, undoing all partial changes.

*Mechanism*: Implemented via logging (undo logs) and the recovery manager — if a failure occurs mid-transaction, the log is used to revert any partial writes.

### C — Consistency
A transaction must take the database from one **valid state** to another **valid state**, preserving all defined integrity constraints (primary keys, foreign keys, check constraints, application-level invariants like "total balance across all accounts must remain the same after a transfer").

*Note*: Consistency is a joint responsibility — the DBMS enforces schema-level constraints, but application logic must also be correctly written to maintain business-level invariants.

### I — Isolation
Concurrently executing transactions should not interfere with each other in ways that produce incorrect results — as if each transaction were executed **serially** (one after another), even though they may actually be interleaved for performance.

*Mechanism*: Enforced via concurrency control protocols (locking, timestamp ordering — covered in Lectures 12–13) and various **isolation levels** (Read Uncommitted, Read Committed, Repeatable Read, Serializable) that trade off strictness against performance.

### D — Durability
Once a transaction is committed, its changes must **persist permanently**, even in the event of a system crash, power failure, or other catastrophic event.

*Mechanism*: Implemented via write-ahead logging (WAL) — changes are logged to non-volatile storage before being considered "committed" — combined with periodic checkpoints, so recovery can replay committed transactions after a crash.

## 4. Why ACID Matters — Example Walkthrough

Consider the fund transfer transaction again:
```sql
BEGIN TRANSACTION;
  UPDATE Account SET balance = balance - 500 WHERE acc_id = 'A';
  UPDATE Account SET balance = balance + 500 WHERE acc_id = 'B';
COMMIT;
```

- **Atomicity** ensures that if the system crashes after the first UPDATE but before the second, the debit from A is rolled back too — money isn't lost into the void.
- **Consistency** ensures the total sum of money across all accounts is unchanged after the transaction (assuming that was a defined invariant).
- **Isolation** ensures that if another transaction reads Account A's balance mid-transfer, it either sees the balance *before* the debit or *after* the full transfer — never a partially-debited, inconsistent value.
- **Durability** ensures that once the transaction commits (and the bank shows "transfer successful"), the change survives even if the server crashes one second later.

## 5. Transaction Control Commands (SQL)

- `BEGIN` / `START TRANSACTION` — marks the start of a transaction.
- `COMMIT` — saves all changes made during the transaction permanently.
- `ROLLBACK` — undoes all changes made during the transaction, restoring the previous state.
- `SAVEPOINT` — sets a named point within a transaction to which you can later roll back partially.

## Summary
A transaction is the fundamental unit of reliable work in a DBMS, moving through active, partially-committed, committed, or failed/aborted states. The ACID properties — Atomicity (all-or-nothing), Consistency (valid-state-to-valid-state), Isolation (safe concurrent execution), and Durability (permanence after commit) — are the contract a DBMS makes with applications to guarantee correctness even in the presence of failures and concurrent access. The next two lectures dive into exactly how Isolation is technically achieved through concurrency control.

## 6. Isolation Levels (SQL Standard) — Detail

Since strict serializable isolation is expensive, SQL defines four isolation levels, each allowing progressively fewer anomalies at the cost of more locking/overhead:

| Isolation Level | Dirty Read | Unrepeatable Read | Phantom Read |
|---|---|---|---|
| Read Uncommitted | Possible | Possible | Possible |
| Read Committed | Prevented | Possible | Possible |
| Repeatable Read | Prevented | Prevented | Possible |
| Serializable | Prevented | Prevented | Prevented |

- **Read Uncommitted**: Lowest isolation — transactions can read uncommitted (dirty) data from other transactions.
- **Read Committed**: A transaction only ever sees data that has been committed at the moment of each read (default in many DBMSs, e.g., PostgreSQL, Oracle).
- **Repeatable Read**: Guarantees that if a transaction reads a row twice, it will see the same data both times (default in MySQL/InnoDB).
- **Serializable**: The strictest level — transactions behave as if executed one at a time, completely isolated from each other.

## 7. Schedules and Transaction Interleaving (Bridge to Lecture 12)

The Isolation property is what motivates the entire study of schedules, serializability, and concurrency control protocols — covered in depth in the next two lectures.

---

## Placement Interview Questions — Lecture 11

**Q1. What is a transaction? Give a real-world example.**
A single logical unit of work comprising one or more read/write operations that must execute completely or not at all. Example: transferring money between two bank accounts (debit + credit).

**Q2. List and explain the states of a transaction.**
Active (executing), Partially Committed (final operation done, not yet saved), Committed (permanently saved), Failed (cannot proceed), Aborted (rolled back to prior state).

**Q3. Explain the ACID properties with an example.**
Atomicity: all-or-nothing execution. Consistency: valid-state-to-valid-state, preserving constraints. Isolation: concurrent transactions don't interfere incorrectly. Durability: committed changes survive crashes. Example: a fund transfer must debit and credit atomically, keep total balance consistent, not expose partial state to other readers, and persist after commit.

**Q4. What is the difference between Atomicity and Durability?**
Atomicity concerns whether a transaction's operations complete fully or not at all (all-or-nothing), while Durability concerns whether a *committed* transaction's results persist permanently even after a crash — they apply at different stages (during vs. after execution).

**Q5. What are isolation levels? Name them in increasing order of strictness.**
Rules governing how much one transaction can be affected by concurrent transactions' uncommitted/committed changes: Read Uncommitted → Read Committed → Repeatable Read → Serializable, each preventing more anomalies (dirty read, unrepeatable read, phantom read) than the last.

**Q6. What SQL commands are used to control transactions?**
`BEGIN`/`START TRANSACTION`, `COMMIT`, `ROLLBACK`, `SAVEPOINT`.

**Q7. Can a transaction be partially committed but never fully committed? What happens then?**
If a system crash occurs between "partially committed" and "committed," recovery mechanisms (write-ahead log) determine whether to complete (REDO) or undo (UNDO) the transaction, depending on what was logged before the crash.

**Q8. What is the difference between a failed and an aborted transaction?**
Failed: the transaction cannot continue due to an error and stops executing. Aborted: the transaction has been actively rolled back, restoring the database to its state before the transaction began — failed transactions typically transition into the aborted state.

**Q9. Why is Consistency considered a joint responsibility between the DBMS and the application?**
Because the DBMS enforces schema-level constraints (PK, FK, CHECK), but higher-level business invariants (e.g., "total balance across accounts must remain constant after a transfer") depend on the application logic being written correctly — the DBMS can't infer or enforce business rules it isn't told about.

**Q10. What is a SAVEPOINT, and why would you use one?**
A named point within a transaction to which you can partially roll back, without undoing the entire transaction — useful for handling partial errors in a long, multi-step transaction without discarding all prior work.
