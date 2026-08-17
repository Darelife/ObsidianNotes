# Lecture 21: Database Indexing (Deep Dive into B-Trees and B+ Trees)

Lecture 15 introduced multi-level indexing conceptually. This lecture covers the actual, self-balancing tree data structures — B-Trees and B+ Trees — that implement multi-level indexes in essentially every production DBMS.

## 1. Why Not a Simple Binary Search Tree (BST)?

A standard BST has height `O(log₂ n)`, meaning `n` millions of records would need roughly 20+ comparisons/disk accesses to find a record. The problem: **each node access in a BST is a separate disk I/O**, and disk I/O is orders of magnitude slower than in-memory comparisons. We want a tree structure that **minimizes the number of disk accesses (tree height)**, even if it means more comparisons happen *within* each node (which is cheap, since a node once loaded is in memory).

This motivates trees with a much **higher branching factor** (many children per node) — this is exactly what B-Trees provide.

## 2. B-Tree — Structure and Properties

A **B-Tree of order `m`** is a self-balancing search tree where:

1. Every node has **at most `m` children** and **at most `m-1` keys**.
2. Every node (except root) has **at least `⌈m/2⌉` children** and **at least `⌈m/2⌉-1` keys** — ensures nodes stay reasonably full, avoiding wasted space.
3. The **root** has at least 2 children (unless it's a leaf, i.e., the whole tree has just one node).
4. All **leaf nodes appear at the same depth** — this is what keeps the tree perfectly balanced, guaranteeing `O(log n)` height regardless of insertion order.
5. Within each node, keys are stored in **sorted order**, and each key acts as a separator: a key `Ki` sits between two child pointers, such that all keys in the left child subtree are `< Ki` and all keys in the right child subtree are `> Ki`.
6. **Crucially: in a B-Tree, both keys AND their associated data/record pointers are stored at EVERY level of the tree — not just at the leaves.**

## 3. B+ Tree — The Variant Actually Used in Most DBMSs

A **B+ Tree** is a refinement of the B-Tree with one critical structural difference:

- **Internal (non-leaf) nodes store ONLY keys**, used purely to guide the search — they do **not** store actual data/record pointers.
- **All actual data/record pointers are stored ONLY in the leaf nodes.**
- **Leaf nodes are additionally linked together** in a sorted, singly (or doubly) linked list — enabling fast **sequential/range scans** without needing to traverse back up the tree.

```
                [ 30 | 60 ]                <- Internal node (keys only, no data)
               /     |      \
        [10|20]   [40|50]   [70|80]        <- Internal/leaf nodes
       /   |  \    /  |  \   /  |  \
    ...   ...  ... (leaf nodes contain actual record pointers)
      
Leaf level: [10]->[20]->[30]->[40]->...->[80]   <- linked list for range scans
```

## 4. Why B+ Trees Are Preferred Over B-Trees for Disk-Based Storage

1. **Higher fan-out (more keys per internal node)**: Since internal nodes in a B+ Tree don't need to store data pointers (only keys + child pointers), more keys fit into a single disk block/page, increasing the branching factor — this means a **shorter tree**, fewer levels, fewer disk I/Os per search.

2. **Efficient range queries**: Because all data lives at the leaf level, and leaves are linked in sorted order, a range query (`WHERE age BETWEEN 20 AND 30`) can locate the starting leaf via a single top-down search, then simply **walk the linked list** to collect all matching results — no need to repeatedly traverse up and down the tree as a B-Tree range query would require.

3. **Uniform access time**: Since ALL actual records are found at the leaf level (never at an internal node), every search takes exactly the same number of steps (the tree height) — predictable, uniform performance, unlike a B-Tree where a key might be found early at an internal node (faster) or only at a leaf (slower), leading to inconsistent lookup times.

4. **Simpler internal-node management**: Internal nodes only ever deal with keys, simplifying insertion/deletion logic compared to a B-Tree juggling both keys and data pointers at every level.

## 5. Node Splitting (Handling Overflow on Insertion)

When inserting a key into a node that is already at its maximum capacity (`m-1` keys), the node **overflows** and must **split**:

1. The node's keys are divided into two new nodes.
2. The **middle key** is pushed **up** into the parent node as a new separator.
   - In a B-Tree: the middle key moves up and is **removed** from the two split leaf/child nodes.
   - In a B+ Tree: the middle key is **copied** up to the parent (it still also remains in the right-hand leaf node, since leaves must retain all actual data keys).
3. If the parent node also overflows as a result, the split **propagates upward**, potentially all the way to the root — if the root itself splits, a **new root** is created, and the tree grows one level taller (this is the only way a B/B+ Tree increases in height, keeping it always balanced).

## 6. Deletion and Underflow

When deleting a key causes a node to fall below the minimum required keys (`⌈m/2⌉-1`), it **underflows**, and the tree rebalances via:
1. **Borrowing (redistribution)**: Borrow a key from an adjacent sibling node (through the parent), if the sibling has more than the minimum number of keys.
2. **Merging**: If no sibling has a spare key to lend, merge the underflowing node with a sibling, pulling down the corresponding separator key from the parent — which might, in turn, cause the parent to underflow, propagating the fix upward (potentially shrinking the tree's height if the root ends up with only one child).

## 7. Time Complexity Summary

| Operation | Time Complexity |
|---|---|
| Search | O(log_m n) |
| Insert | O(log_m n) |
| Delete | O(log_m n) |
| Range Query | O(log_m n + k), where k = number of matching records |

(where `m` is the order/branching factor — much larger than 2, unlike a binary tree, which is exactly why the tree stays extremely shallow even for millions of records.)

## Summary
B-Trees and B+ Trees are self-balancing, high-branching-factor tree structures purpose-built to minimize disk I/O for database indexes, since disk access — not in-memory comparison — is the real performance bottleneck. The B+ Tree refines the B-Tree by storing data pointers exclusively at leaf nodes (keeping internal nodes compact for a higher fan-out and shorter tree) and by linking leaf nodes together for fast, efficient range scans — which is why virtually every production relational database (MySQL's InnoDB, PostgreSQL, Oracle, SQL Server) uses B+ Trees, not plain B-Trees, as its default index structure.

## 8. Worked Example — B+ Tree Insertion (Order 3)

Consider a B+ Tree of order 3 (max 2 keys per node, max 3 children). Insert keys: 10, 20, 5, 6, 12, 30, 7, 17.

1. Insert 10, 20 → leaf `[10, 20]` (root, still within capacity).
2. Insert 5 → leaf `[5, 10, 20]` overflows (order 3 = max 2 keys) → split: `[5, 10]` and `[20]`, middle key `10` copied up → new root `[10]` with children `[5,10]` and `[20]`.
3. Continue inserting 6, 12, 30, 7, 17, splitting/promoting keys similarly whenever a leaf exceeds 2 keys — each split may propagate a new separator key up to the parent, and if the root itself overflows, a brand-new root is created, increasing tree height by exactly one level.

This walkthrough is a common **whiteboard exercise** in interviews — practice drawing this out by hand for a given order and key sequence.

## 9. B+ Tree vs. Hash Index — Quick Comparison

| Aspect | B+ Tree | Hash Index |
|---|---|---|
| Equality search | O(log n) | O(1) average |
| Range search | Efficient (sorted leaves) | Not supported efficiently |
| Sorted output (ORDER BY) | Natural (leaves already sorted) | Not supported |
| Typical use | Default index type in most RDBMS | Specialized equality-only lookups |

---

## Placement Interview Questions — Lecture 21

**Q1. Why are B+ Trees preferred over plain Binary Search Trees for database indexing?**
Because disk I/O is the real bottleneck, not comparisons; a BST's low branching factor (2) leads to a tall tree requiring many disk accesses, while a B+ Tree's high branching factor (many keys per node) keeps the tree extremely shallow, minimizing disk I/Os per search.

**Q2. What is the key structural difference between a B-Tree and a B+ Tree?**
In a B-Tree, both keys and data pointers are stored at every level (internal and leaf nodes). In a B+ Tree, internal nodes store ONLY keys (for navigation); ALL actual data pointers live exclusively at the leaf level, and leaves are linked together in a sorted list.

**Q3. Why are B+ Trees generally preferred over B-Trees in real-world database systems?**
Higher fan-out (internal nodes hold only keys, fitting more per disk block, shortening the tree), efficient range queries (via the linked leaf list), and uniform/predictable access time (every search always reaches the leaf level, unlike a B-Tree where data could be found earlier at an internal node).

**Q4. Explain what happens during a node split in a B+ Tree insertion.**
When a node exceeds its maximum key capacity, it splits into two nodes; the middle key is copied up into the parent as a new separator (and also remains in the right leaf, since B+ Tree leaves must retain all data keys). If the parent also overflows, the split propagates upward, potentially creating a new root and increasing tree height by one.

**Q5. How does deletion cause underflow in a B+/B-Tree, and how is it resolved?**
If deleting a key causes a node to fall below the minimum required keys, it underflows and is resolved by either borrowing a key from an adjacent sibling (via the parent) or merging with a sibling (pulling down the separator key from the parent) — which may propagate upward and potentially shrink the tree's height.

**Q6. What is the time complexity of search, insert, and delete operations in a B+ Tree?**
All are O(log_m n), where m is the tree's order (branching factor) and n is the number of keys — much shallower than a binary tree due to the high fan-out.

**Q7. Why are B+ Trees especially efficient for range queries?**
Because all data resides at the leaf level, and leaves are linked together in sorted order via a linked list — a range query locates the starting leaf with a single top-down search, then simply walks the linked list to collect all matching results without repeatedly traversing back up the tree.

**Q8. Why do internal nodes in a B+ Tree not store data pointers?**
To maximize the number of keys that fit into a single disk block/page for internal nodes, increasing the branching factor and keeping the tree as shallow (short) as possible — since internal nodes exist purely to guide navigation, not to store actual record data.

**Q9. What's the difference in access time consistency between B-Trees and B+ Trees?**
In a B-Tree, a key might be found early at an internal node (fast) or only at a leaf (slower) — inconsistent access time. In a B+ Tree, every search always proceeds all the way to the leaf level, since that's the only place data actually lives — giving uniform, predictable access time for every query.

**Q10. Which real-world databases use B+ Trees as their default index structure?**
MySQL's InnoDB storage engine, PostgreSQL, Oracle Database, and Microsoft SQL Server all use B+ Trees as their default/primary index structure.
