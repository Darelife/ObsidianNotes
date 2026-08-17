# Lecture 13: Concurrency Control Protocols (Lock-Based, 2PL, Timestamp)

Lecture 12 defined what makes a schedule "correct" (serializable, recoverable). This lecture covers the actual **protocols** DBMSs use to enforce that correctness at runtime, without needing to check the entire schedule in hindsight.

## 1. Lock-Based Protocols

The core idea: before a transaction reads or writes a data item, it must acquire an appropriate **lock** on it, and release the lock at some defined point.

### Types of Locks

1. **Shared Lock (S-lock / Read lock)**: Allows a transaction to **read** a data item. Multiple transactions can hold shared locks on the same item simultaneously (reads don't conflict with reads).
2. **Exclusive Lock (X-lock / Write lock)**: Allows a transaction to **read and write** a data item. Only one transaction can hold an exclusive lock on an item at a time, and no other transaction can hold any lock (shared or exclusive) on it simultaneously.

### Lock Compatibility Matrix

| | S | X |
|---|---|---|
| **S** | ✅ Compatible | ❌ Conflict |
| **X** | ❌ Conflict | ❌ Conflict |

### Problems with Simple Locking (without protocol discipline)
Just acquiring/releasing locks on demand doesn't guarantee serializability by itself — a transaction could release a lock too early and re-acquire it later, allowing incorrect interleavings. This motivates the **Two-Phase Locking Protocol**.

## 2. Two-Phase Locking (2PL) Protocol

2PL enforces a strict discipline on when locks can be acquired and released, guaranteeing **conflict serializability**.

### The Two Phases

1. **Growing Phase**: The transaction may **acquire** locks, but cannot release any.
2. **Shrinking Phase**: The transaction may **release** locks, but cannot acquire any new ones.

Once a transaction releases its first lock, it has entered the shrinking phase and can never acquire another lock — this "lock point" (the moment the last lock is acquired) is what makes the schedule provably serializable.

```
Number of locks held
   ^
   |        ___________
   |       /            \
   |      /              \
   |     /                \
   |____/__________________\____> Time
        Growing    Lock    Shrinking
        Phase      Point   Phase
```

### Variants of 2PL

1. **Basic 2PL**: As described above — can still suffer from **cascading rollbacks** (if a transaction releases a lock and then aborts, other transactions that read the uncommitted data must also roll back).

2. **Strict 2PL**: All **exclusive (write) locks** are held until the transaction **commits or aborts** (only released at the very end). This avoids cascading rollbacks because no other transaction can read uncommitted written data. This is the most commonly implemented variant in real systems.

3. **Rigorous 2PL**: **Both** shared and exclusive locks are held until commit/abort (strictest variant). Guarantees that schedules are not just serializable but also that the commit order equals the serialization order — makes recovery simpler.

### Drawback of 2PL: Deadlocks
2PL guarantees serializability but does **not** prevent **deadlocks** — a set of transactions can end up in a circular wait for locks held by each other. This is handled separately (Lecture 14).

## 3. Timestamp Ordering Protocol

An alternative to lock-based protocols. Every transaction `Ti` is assigned a unique **timestamp** `TS(Ti)` at the moment it starts (older transactions get smaller timestamps). The protocol ensures conflicting operations execute in **timestamp order**, guaranteeing serializability equivalent to the order in which transactions started — **without using locks at all**.

### Tracking Metadata
For each data item `Q`, the system maintains:
- `W-timestamp(Q)`: the largest timestamp of any transaction that successfully wrote `Q`.
- `R-timestamp(Q)`: the largest timestamp of any transaction that successfully read `Q`.

### Rules

**For a Read(Q) by transaction Ti:**
- If `TS(Ti) < W-timestamp(Q)` → Ti is trying to read a value that was **already overwritten** by a "future" transaction (relative to Ti). This read is **rejected**, and Ti is **rolled back** (restarted with a new timestamp).
- Otherwise, the read proceeds, and `R-timestamp(Q)` is updated to `max(R-timestamp(Q), TS(Ti))`.

**For a Write(Q) by transaction Ti:**
- If `TS(Ti) < R-timestamp(Q)` → some "future" transaction has already read the current value of Q, and Ti's write would retroactively invalidate that read. This write is **rejected**, and Ti is **rolled back**.
- If `TS(Ti) < W-timestamp(Q)` → Ti's write is "obsolete" (an even newer transaction has already overwritten Q). Under the **Thomas Write Rule** optimization, this write can simply be **ignored** (rather than rejecting/aborting Ti) since its effect would be overwritten anyway.
- Otherwise, the write proceeds, and `W-timestamp(Q)` is updated to `TS(Ti)`.

### Properties of Timestamp Ordering
- Guarantees **conflict serializability** by construction — no precedence graph cycles are possible since all conflicting operations respect timestamp order.
- **Deadlock-free** by design (no locks are held, so no circular waits can occur) — a major advantage over 2PL.
- **Downside**: Can cause more transaction **restarts/rollbacks** compared to lock-based approaches, especially under high contention, since conflicting transactions are aborted rather than made to wait.

## 4. Lock-Based vs. Timestamp-Based — Comparison

| Aspect | Lock-Based (2PL) | Timestamp Ordering |
|---|---|---|
| Mechanism | Transactions wait for locks | Transactions execute in timestamp order |
| Deadlocks | Possible — needs separate handling | Not possible |
| Rollbacks | Fewer, but can cascade (Basic 2PL) | More frequent (aborts on conflict) |
| Overhead | Lock table management | Timestamp bookkeeping per data item |

## Summary
Lock-based protocols use shared/exclusive locks with the **Two-Phase Locking (2PL)** discipline — locks only acquired during a growing phase, only released during a shrinking phase — to guarantee conflict serializability; **Strict 2PL** additionally holds exclusive locks until commit to avoid cascading rollbacks, making it the industry-standard choice. **Timestamp Ordering** offers a lock-free alternative that assigns transactions unique timestamps and rejects/rolls back operations that would violate timestamp order, trading a higher rollback rate for the elimination of deadlocks entirely. The next lecture addresses how deadlocks (a 2PL side-effect) are detected, prevented, and recovered from.

## 5. Multiple Granularity Locking (Additional Concept)

Instead of locking an entire table or a single row, DBMSs often support **multiple granularity locking** — a hierarchy (database → table → page → row) where a transaction can lock at the appropriate level of granularity for its needs, using special **intention locks** (Intention-Shared `IS`, Intention-Exclusive `IX`) on coarser levels to signal that finer-grained locks are held below, avoiding the need to check every single row when a coarse-grained request comes in.

## 6. Optimistic Concurrency Control (Additional Protocol)

An alternative to both locking and timestamp ordering: transactions execute **without any locking** during a "read phase," then before committing, go through a **validation phase** checking for conflicts with concurrently committed transactions; if no conflict, a **write phase** commits the changes, otherwise the transaction is rolled back and retried. Works well when conflicts are rare (low contention).

---

## Placement Interview Questions — Lecture 13

**Q1. What is the difference between a shared lock and an exclusive lock?**
Shared (S) lock allows only reading; multiple transactions can hold it simultaneously on the same item. Exclusive (X) lock allows reading and writing; only one transaction can hold it, and no other transaction can hold any lock on that item concurrently.

**Q2. Explain the Two-Phase Locking (2PL) protocol.**
2PL splits a transaction's lifetime into a growing phase (locks can only be acquired, not released) and a shrinking phase (locks can only be released, not acquired). This discipline guarantees conflict serializability.

**Q3. What is the difference between Basic 2PL, Strict 2PL, and Rigorous 2PL?**
Basic 2PL: locks released any time in the shrinking phase, can cause cascading rollbacks. Strict 2PL: exclusive locks held until commit/abort, avoiding cascading rollbacks. Rigorous 2PL: both shared and exclusive locks held until commit/abort — strictest, simplifies recovery since commit order equals serialization order.

**Q4. Does 2PL guarantee freedom from deadlocks?**
No. 2PL guarantees conflict serializability but does not prevent deadlocks — transactions can still end up in a circular wait for locks. Deadlock handling requires separate mechanisms (Lecture 14).

**Q5. Explain the Timestamp Ordering Protocol.**
Every transaction gets a unique timestamp at start. For each data item, the system tracks the largest read-timestamp (R-timestamp) and write-timestamp (W-timestamp) of transactions that accessed it. Reads/writes that would violate timestamp order (i.e., attempt to read/write data "out of order" relative to already-processed transactions) are rejected and the transaction is rolled back and restarted with a new timestamp.

**Q6. What is the Thomas Write Rule, and how does it optimize the timestamp ordering protocol?**
When a write is "obsolete" (TS(Ti) < W-timestamp(Q), meaning a newer transaction has already overwritten Q), instead of rejecting/aborting Ti, the write is simply ignored, since it would be overwritten anyway — reduces unnecessary rollbacks.

**Q7. Compare lock-based protocols and timestamp ordering in terms of deadlocks and rollbacks.**
Lock-based (2PL) protocols can suffer from deadlocks but generally have fewer overall rollbacks; timestamp ordering is deadlock-free by design (no locks held) but tends to cause more frequent transaction rollbacks/restarts under contention.

**Q8. What is multiple granularity locking, and why is it useful?**
A hierarchical locking scheme (database → table → page → row) allowing transactions to lock at the appropriate level of granularity, using intention locks on coarser levels to avoid expensive checks of every fine-grained lock below — balances lock overhead against concurrency.

**Q9. What is optimistic concurrency control, and when is it preferred?**
A lock-free approach where transactions execute without locking, then validate for conflicts before committing; if a conflict is found, the transaction rolls back and retries. Preferred in low-contention environments where conflicts are rare, avoiding the overhead of locking.

**Q10. Why is Strict 2PL the most commonly implemented protocol in real DBMSs?**
Because it guarantees conflict serializability (like basic 2PL) while also avoiding cascading rollbacks (by holding exclusive locks until commit/abort) — a good balance between correctness, simplicity, and recovery ease, without the complexity of fully rigorous locking.
