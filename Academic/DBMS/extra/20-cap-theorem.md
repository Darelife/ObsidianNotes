# Lecture 20: CAP Theorem in Distributed Databases

## 1. What is the CAP Theorem?

Formulated by Eric Brewer, the **CAP theorem** states that a **distributed data store** (a database spread across multiple networked nodes, as built up through clustering, replication, and sharding in Lectures 18–19) can provide **at most two out of the following three guarantees simultaneously** — never all three at once, in the presence of a network partition:

- **C — Consistency**
- **A — Availability**
- **P — Partition Tolerance**

## 2. The Three Properties, Defined

### a. Consistency (C)
Every read receives the **most recent write** (or an error) — all nodes see the **same data at the same time**. This is a stronger, distributed-systems-flavored notion of consistency than the "C" in ACID (though related in spirit).

### b. Availability (A)
Every request (read or write) receives a **non-error response**, without guarantee that it contains the most recent write — the system is always "up" and responsive, even if some nodes are unreachable.

### c. Partition Tolerance (P)
The system **continues to operate** despite an arbitrary number of messages being dropped or delayed by the network between nodes (a "network partition" — e.g., a network cable is cut, splitting the cluster into two groups that can't talk to each other).

## 3. Why You Can't Have All Three

In any real distributed system, **network partitions are inevitable** — networks fail, packets get lost, links go down. So **Partition Tolerance is not really optional** for a distributed system; you must design for it. This means the real-world choice boils down to:

**When a partition happens, do you sacrifice Consistency or Availability?**

- If a node cannot confirm it has the latest data (because it can't communicate with other nodes during the partition), it can either:
  - **Refuse to respond** (or respond with an error) until it can confirm it has up-to-date data → preserves **Consistency**, sacrifices **Availability**. → **CP system**.
  - **Respond anyway** with potentially stale data → preserves **Availability**, sacrifices **Consistency**. → **AP system**.

(Note: "CA" — consistent and available but not partition-tolerant — is only really achievable in the absence of network partitions, e.g., a single-node, non-distributed database. It's not a meaningful option for a truly distributed system.)

## 4. CP vs. AP — Real-World Examples

### CP Systems (prioritize Consistency over Availability)
During a partition, these systems may become **unavailable** (return errors or time out) for some requests to guarantee that no stale/incorrect data is ever returned.
- **Examples**: MongoDB (in certain configurations), HBase, Google Spanner (achieves very strong consistency via specialized infrastructure like atomic clocks), traditional RDBMS in synchronous replication mode.
- **Best for**: Banking systems, inventory systems where showing outdated stock counts could cause real business problems (e.g., overselling).

### AP Systems (prioritize Availability over Consistency)
During a partition, these systems continue to serve reads/writes on all reachable nodes, accepting that different nodes might temporarily disagree, relying on **eventual consistency** (BASE model from Lecture 17) to reconcile afterward.
- **Examples**: Cassandra, DynamoDB, CouchDB, Riak.
- **Best for**: Social media feeds, shopping carts, systems where being always-responsive matters more than every user seeing perfectly up-to-date data instantly.

## 5. A Practical Note: PACELC (Extension of CAP)

CAP only describes trade-offs **during a partition**. The **PACELC theorem** extends this: even when there's **no partition (Else)**, a system must still choose between **Latency (L)** and **Consistency (C)** — because achieving strong consistency across replicas (waiting for all/majority acknowledgments — recall synchronous replication from Lecture 18) inherently adds latency.

**PACELC**: *if Partition, choose between Availability and Consistency; Else, choose between Latency and Consistency.*

## 6. CAP Theorem and Earlier Concepts — How It All Connects

- **Replication** (Lecture 18): Synchronous replication leans toward Consistency (CP); asynchronous replication leans toward Availability (AP), since replicas may lag.
- **Sharding** (Lecture 19): Cross-shard operations during a partition force the same C-vs-A tradeoff for any transaction spanning multiple shards.
- **NoSQL vs SQL** (Lecture 17): Most traditional SQL databases were designed as single-node systems prioritizing ACID consistency; when distributed, they often lean CP. Many NoSQL databases were explicitly designed AP-first to prioritize availability at internet scale.

## Summary
The CAP theorem states that a distributed database can guarantee at most two of Consistency, Availability, and Partition Tolerance simultaneously — and because network partitions are unavoidable in any real distributed system, the practical choice always comes down to **CP** (sacrifice availability during a partition to stay consistent) versus **AP** (sacrifice strict consistency to stay available, relying on eventual consistency). The choice should be driven by the actual business requirements of the application — financial correctness generally favors CP, while user-facing responsiveness at scale often favors AP — and the PACELC extension reminds us that even without partitions, there's still an inherent latency-vs-consistency trade-off to navigate.

## 7. Common Misconception Corner

- **Misconception**: "CAP means you must always pick exactly 2 out of 3, permanently." **Reality**: In the absence of a partition, a well-designed distributed system CAN be both consistent and available — the trade-off (C vs. A) only forces a hard choice specifically **during** an actual network partition event. Outside of partitions, PACELC (below) is the more relevant framework.
- **Misconception**: "NoSQL = AP, SQL = CP, always." **Reality**: This is a rough generalization, not an absolute rule — many modern systems (e.g., Google Spanner, CockroachDB) blur this line, offering strong consistency at scale using specialized techniques (e.g., synchronized atomic clocks, consensus protocols).

## 8. Consensus Protocols (Brief Mention)

Distributed systems that need strong consistency despite being spread across multiple nodes often rely on **consensus protocols** like **Paxos** or **Raft** to agree on a single, consistent value/state (e.g., who the current master is) even in the presence of node failures — foundational to modern CP-leaning distributed databases and to safe automatic failover (Lecture 22).

---

## Placement Interview Questions — Lecture 20

**Q1. State the CAP theorem.**
A distributed data store can provide at most two of the following three guarantees simultaneously: Consistency (every read gets the latest write), Availability (every request gets a non-error response), and Partition Tolerance (the system keeps working despite network partitions).

**Q2. Why is Partition Tolerance essentially non-negotiable for a real distributed system?**
Because network partitions (dropped/delayed messages between nodes) are an inevitable, unavoidable reality in any real distributed network — a system that isn't partition-tolerant simply cannot function correctly as a distributed system at all.

**Q3. What is the real practical trade-off implied by CAP, given that P is mandatory?**
The real choice, during an actual partition, is between Consistency and Availability — CP systems refuse/delay responses to guarantee correctness, while AP systems respond with potentially stale data to remain available.

**Q4. Give examples of CP and AP systems.**
CP: MongoDB (certain configs), HBase, Google Spanner, traditional RDBMS with synchronous replication. AP: Cassandra, DynamoDB, CouchDB, Riak.

**Q5. What is the PACELC theorem, and how does it extend CAP?**
PACELC states: if there's a Partition, choose between Availability and Consistency (as in CAP); Else (no partition), choose between Latency and Consistency — since achieving strong consistency across replicas even in normal operation requires waiting for acknowledgments, adding latency.

**Q6. Why can't a distributed system be both fully Consistent and fully Available during a partition?**
Because during a partition, some nodes can't communicate with others — if a node responds without confirming it has the latest data (to stay Available), it risks returning stale data (violating Consistency); if it refuses to respond until it can confirm (to stay Consistent), it becomes unavailable for that request.

**Q7. Is CA (Consistent and Available, without Partition Tolerance) achievable? When?**
Only in systems that aren't truly distributed (e.g., a single-node database) or that can somehow guarantee network partitions will never occur — not a realistic option for genuine distributed systems.

**Q8. How does CAP relate to the choice between synchronous and asynchronous replication?**
Synchronous replication leans toward Consistency (CP) since it waits for replica confirmation before committing, at the cost of availability/latency during network issues. Asynchronous replication leans toward Availability (AP), since it doesn't wait, but replicas may lag and serve stale data.

**Q9. Would you choose a CP or an AP system for a banking application? Justify.**
CP — because showing incorrect/stale account balances or allowing conflicting concurrent transactions could cause serious real-world financial errors (e.g., overdraft, double-spending); it's safer to briefly become unavailable than to return incorrect financial data.

**Q10. Would you choose a CP or an AP system for a social media "like" counter? Justify.**
AP — because it's far more important for the system to remain responsive and available to users at all times; a temporarily slightly-stale like count is a negligible, acceptable trade-off compared to the feed becoming unavailable.
