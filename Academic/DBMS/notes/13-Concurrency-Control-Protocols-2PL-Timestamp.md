# Lecture 13: Concurrency Control Protocols (Lock-Based, 2PL, Timestamp Ordering)

---

## 1. Lock-Based Concurrency Control Protocols

Concurrency control protocols are algorithms enforced by the DBMS engine to guarantee that all execution schedules are serializable and recoverable without manual user intervention.

### 1.1 Lock Types & Lock Compatibility Matrix

A **Lock** is a synchronization variable associated with a data item controlling concurrent access.

1. **Shared Lock ($S$) / Read Lock**:
   * Acquired by a transaction prior to executing a `READ` operation.
   * Multiple transactions can concurrently hold Shared locks on the same data item.
2. **Exclusive Lock ($X$) / Write Lock**:
   * Acquired by a transaction prior to executing a `WRITE` operation.
   * Only one transaction can hold an Exclusive lock on a data item. No other transaction can hold any lock ($S$ or $X$) simultaneously.

#### Lock Compatibility Matrix:

| Requested Lock \ Currently Held Lock | Shared ($S$) | Exclusive ($X$) |
| :--- | :--- | :--- |
| **Shared ($S$)** | **GRANTED** | **DENIED (Blocked)** |
| **Exclusive ($X$)** | **DENIED (Blocked)** | **DENIED (Blocked)** |

---

## 2. Two-Phase Locking (2PL) Protocol

The **Two-Phase Locking (2PL)** protocol guarantees **Conflict Serializability** by dividing lock management within a transaction into two distinct, non-overlapping phases.

```
Number of
Locks Held
  ^
  |        Growing Phase            Shrinking Phase
  |      (Locks acquired only)     (Locks released only)
  |            /=======\
  |           /         \
  |          /           \
  |         /  Lock Point \
  +--------+---------------+-----------------------------> Time
```

### Rules of Basic 2PL:
1. **Growing Phase**: A transaction may **acquire** locks, but **cannot release** any locks.
2. **Shrinking Phase**: A transaction may **release** locks, but **cannot acquire** any new locks.
3. **Lock Point**: The instant in time when a transaction acquires its final lock (the end of the Growing Phase).

---

### 2.1 Variations of Two-Phase Locking

#### 1. Basic 2PL
* **Rules**: Standard growing and shrinking phases.
* **Guarantee**: Guarantees Conflict Serializability.
* **Drawback**: Prone to **Cascading Rollbacks** and **Deadlocks**.

#### 2. Strict 2PL
* **Rules**: Transaction acquires locks normally during growing phase, but **holds ALL Exclusive ($X$) locks until the transaction commits or aborts**.
* **Guarantee**: Guarantees Conflict Serializability AND **Cascadeless Schedules** (eliminates Dirty Reads).

#### 3. Rigorous 2PL
* **Rules**: Transaction **holds ALL locks ($S$ and $X$) until the transaction commits or aborts**.
* **Guarantee**: Serial order of transactions is identical to the order in which transactions commit.

#### 4. Conservative (Static) 2PL
* **Rules**: Transaction must declare and acquire **ALL required locks BEFORE execution begins**. If any lock cannot be acquired, none are acquired, and transaction waits.
* **Guarantee**: Completely **Deadlock-Free**!
* **Drawback**: Hard to predict all data items needed in advance.

---

## 3. Timestamp Ordering Protocols

Timestamp Ordering protocols do not use locks to enforce serializability. Instead, each transaction $T_i$ is assigned a globally unique monotonically increasing **Timestamp** $TS(T_i)$ when it enters the system.

### 3.1 Basic Timestamp Ordering (TO) Protocol

For every data item $X$, the DBMS maintains two timestamp fields:
* $R\text{-}TS(X)$: The largest timestamp of any transaction that successfully read $X$.
* $W\text{-}TS(X)$: The largest timestamp of any transaction that successfully wrote $X$.

#### Read Rule for $T_i$ executing $Read(X)$:
1. If $TS(T_i) < W\text{-}TS(X)$:
   * Transaction $T_i$ is attempting to read an item that was already overwritten by a younger transaction.
   * **Action**: **REJECT and ABORT $T_i$**. Roll back $T_i$ and restart with a new timestamp.
2. If $TS(T_i) \ge W\text{-}TS(X)$:
   * **Action**: **EXECUTE $Read(X)$**. Set $R\text{-}TS(X) = \max(R\text{-}TS(X), TS(T_i))$.

#### Write Rule for $T_i$ executing $Write(X)$:
1. If $TS(T_i) < R\text{-}TS(X)$:
   * Transaction $T_i$ is attempting to write a value that was already read by a younger transaction.
   * **Action**: **REJECT and ABORT $T_i$**.
2. If $TS(T_i) < W\text{-}TS(X)$:
   * Transaction $T_i$ is attempting to write an obsolete value.
   * **Action**: **REJECT and ABORT $T_i$**.
3. Otherwise:
   * **Action**: **EXECUTE $Write(X)$**. Set $W\text{-}TS(X) = TS(T_i)$.

---

### 3.2 Thomas' Write Rule (Optimization)

Thomas' Write Rule modifies the Basic Timestamp Ordering protocol to allow higher concurrency by **ignoring obsolete writes** instead of aborting the transaction.

#### Modified Write Rule Condition:
* If $TS(T_i) < W\text{-}TS(X)$:
  * Instead of aborting $T_i$, simply **IGNORE / SKIP the $Write(X)$ operation** and continue execution!
  * *Reasoning*: A younger transaction has already overwritten $X$ with a newer committed value. Skipping $T_i$'s stale write maintains correctness without unnecessarily aborting $T_i$.

---

## 4. Comparison of Protocols

| Protocol | Strategy | Deadlock Possible? | Cascading Rollback Possible? |
| :--- | :--- | :--- | :--- |
| **Basic 2PL** | Pessimistic (Locks) | **Yes** | **Yes** |
| **Strict 2PL** | Pessimistic (Locks) | **Yes** | **No** |
| **Conservative 2PL**| Pessimistic (Pre-allocation)| **No** | **No** |
| **Basic Timestamp** | Optimistic (Timestamps) | **No** | **Yes** |
| **Thomas' Write Rule**| Optimistic (Skip Writes) | **No** | **Yes** |

---

## 5. Conceptual Practice Exercises

1. Trace a transaction executing under Strict 2PL. At what exact step are shared vs exclusive locks released?
2. Apply Basic Timestamp Ordering to Schedule $S$: $T_1(TS=10), T_2(TS=20)$. $R_1(X), W_2(X), W_1(X)$. Will $T_1$ abort?
3. Explain why Thomas' Write Rule enables schedules that are View Serializable but NOT Conflict Serializable.

---

## 6. Hard Placement & Interview Questions (FAANG Level)

### Q1: Prove mathematically that any schedule conforming to the 2-Phase Locking (2PL) protocol is guaranteed to be Conflict Serializable.

**Answer:**
* **Proof by Contradiction**:
  1. Assume a schedule $S$ generated under 2PL is **NOT** Conflict Serializable.
  2. Then, the precedence graph of $S$ must contain a cycle: $T_1 \rightarrow T_2 \rightarrow T_3 \rightarrow \dots \rightarrow T_k \rightarrow T_1$.
  3. Edge $T_1 \rightarrow T_2$ implies $T_1$ accessed item $X$ (and released its lock on $X$) **before** $T_2$ acquired its lock on $X$.
     $$\text{LockPoint}(T_1) < \text{Unlock}(T_1, X) < \text{Lock}(T_2, X)$$
  4. Extending this inequality around the entire cycle yields:
     $$\text{LockPoint}(T_1) < \text{LockPoint}(T_2) < \dots < \text{LockPoint}(T_k) < \text{LockPoint}(T_1)$$
  5. This reduces to $\text{LockPoint}(T_1) < \text{LockPoint}(T_1)$, which is a logical impossibility!
  6. Contradiction proven. Therefore, all 2PL schedules are strictly Conflict Serializable.

---

### Q2: What is the "Phantom Lock" problem in Multiple Granularity Locking (MGL), and how do Intent Locks (IS, IX, SIX) resolve hierarchy lock checking overhead?
**Answer:**
* **Problem**: In a database lock hierarchy (Database $\rightarrow$ Table $\rightarrow$ Page $\rightarrow$ Tuple), if $T_1$ locks a single Tuple inside Page 5 with an Exclusive lock, $T_2$ attempting to acquire an Exclusive lock on the entire Table must scan millions of tuples to verify no individual tuple is locked.
* **Intent Locks Solution**:
  * Before a transaction locks a leaf node (Tuple), it must acquire **Intent Locks** on all parent nodes (Page, Table, Database).
  * **Intent Shared (IS)**: Indicates an explicit $S$ lock will be acquired at a lower level.
  * **Intent Exclusive (IX)**: Indicates an explicit $X$ lock will be acquired at a lower level.
  * **Shared Intent Exclusive (SIX)**: Explicit $S$ lock on current node + IX lock on lower level.
  * $T_2$ attempting to lock the Table simply checks if an $IX$ or $IS$ lock exists on the Table node. Checks take $O(1)$ constant time.

---

### Q3: Under what exact operational conditions will the Timestamp Ordering protocol exhibit starvation (livelock), and how can it be mitigated?
**Answer:**
* **Starvation Scenario**: If a long-running transaction $T_{long}$ is repeatedly aborted and restarted because younger transactions constantly perform writes with higher timestamps, $T_{long}$ may be repeatedly aborted forever.
* **Mitigation**: When a transaction is aborted in timestamp ordering, it should NOT be assigned the current wall-clock timestamp upon restart. Instead, it maintains its original initial timestamp $TS(T_{long})$, prioritizing older transactions over newly arriving transactions.
