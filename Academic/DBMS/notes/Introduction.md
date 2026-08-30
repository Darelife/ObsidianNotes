# DBMS Placement Preparation Series — Master Syllabus & Table of Contents

Welcome to the comprehensive, placement-focused Database Management Systems (DBMS) lecture notes series. Each document is detailed with core theoretical foundations, mathematical proofs, architectural diagrams, step-by-step algorithms, real-world case studies, practice exercises, and hard placement interview questions with answers.

---

## 📚 Complete Lecture Index

1. [[Academic/DBMS/notes/01-Introduction-to-DBMS]]
   * Data vs. Raw Bytes vs. Information, File System Limitations, Core Advantages of DBMS.
2. [[Academic/DBMS/notes/02-Architecture-of-DBMS]]
   * ANSI-SPARC 3-Schema Architecture, Logical & Physical Data Independence, DBA Role.
3. [[03-ER-Model-and-Notations]]
   * Entities, Attributes (Composite, Multivalued, Derived), Relationships, Structural & Participation Constraints.
4. [[Academic/DBMS/notes/04-Extended-ER-Features]]
   * Top-Down & Bottom-Up Abstractions, Disjointness & Completeness Constraints, Attribute Inheritance, Aggregation.
5. [[05-How-to-Think-and-Formulate-ER-Diagrams]]
   * 5-Step Problem-Solving Pipeline, Noun/Verb Heuristic, Fan/Chasm Trap Avoidance, Hospital System Case Study.
6. [[06-Designing-ER-Model-of-Facebook]]
   * Practical Case Study: User Graph, Threaded Comments, Polymorphic Reactions, Messages, Groups & Pages.
7. [[07-Relational-Model-in-DBMS]]
   * Tuples, Attributes, Domains, Key Hierarchy (Super, Candidate, Primary, Foreign), Domain/Entity/Referential Integrity.
8. [[08-Transform-ER-Model-to-Relational-Model]]
   * Translation Rules 1-8 for Strong/Weak Entities, Relationships, Multivalued Attributes, EER Inheritance.
9. [[09-Normalization-Part-1-FDs-1NF-2NF]]
   * Anomalies, Functional Dependencies (FDs), Armstrong's Axioms, Attribute Closure ($X^+$), 1NF, 2NF.
10. [[10-Normalization-Part-2-3NF-BCNF-Lossless-Decomposition]]
    * 3NF, BCNF, Lossless-Join Condition, Dependency Preservation, 3NF Synthesis vs BCNF Top-Down Decomposition.
11. [[11-Transactions-and-ACID-Properties]]
    * Transaction State Diagram, Deep-Dive on ACID guarantees, WAL (Write-Ahead Logging), Steal/No-Force Policies.
12. [[12-Concurrency-Control-in-DBMS]]
    * Concurrency Anomalies (Dirty Read, Fuzzy Read, Phantom Read, Lost Update), Schedules, Precedence Graph & Conflict Serializability.
13. [[13-Concurrency-Control-Protocols-2PL-Timestamp]]
    * Shared/Exclusive Locks, 2PL Proof, Strict/Rigorous/Conservative 2PL, Basic Timestamp Ordering, Thomas' Write Rule.
14. [[14-Deadlock-Handling-and-Recovery-Systems]]
    * Wait-Die vs Wound-Wait, Wait-For Graph (WFG), Log-Based Recovery (Deferred vs Immediate), ARIES 3-Phase Recovery.
15. [[15-Indexing-in-DBMS]]
    * File Organization, Dense vs Sparse Indexing, Primary vs Secondary vs Clustered Indexes, Block I/O Cost Calculations.
16. [[Academic/DBMS/notes/16-Types-of-Databases]]
    * Taxonomy of RDBMS, NoSQL Categories (Document, Key-Value, Wide-Column, Graph), Legacy Models (Hierarchical, Network).
17. [[17-NoSQL-vs-SQL-Databases]]
    * Schema-on-Write vs Schema-on-Read, ACID vs BASE, Decision Tree, Tunable Consistency ($R+W>N$), LSM Trees vs B+ Trees.
18. [[18-Clustering-and-Replication-in-DBMS]]
    * Shared-Disk vs Shared-Nothing, Synchronous/Asynchronous/Semi-Sync Replication, RTO & RPO, Split-Brain & Quorum Consensus.
19. [[Academic/DBMS/notes/19-Database-Scaling-Patterns]]
    * Vertical vs Horizontal Scaling, Vertical/Horizontal Partitioning, Range/Hash Sharding, Consistent Hashing & 2PC.
20. [[20-CAP-Theorem-in-Distributed-Databases]]
    * Consistency (Linearizability), Availability, Partition Tolerance, 2-Node Mathematical Proof, PACELC Theorem Extension.
21. [[21-Deep-Dive-B-Trees-and-B-Plus-Trees]]
    * B-Tree vs B+ Tree Mechanics, Node Splitting & Merging, Order & Block Capacity Calculations, Lehman-Yao B-link Trees, Slotted Page Layout.
22. [[22-Master-Slave-Architecture-and-Failover]]
    * Write-Master Read-Slave Routing, Leader Election, VIP Rotation, Cascading Replication, Placement Preparation Map & Case Study.
