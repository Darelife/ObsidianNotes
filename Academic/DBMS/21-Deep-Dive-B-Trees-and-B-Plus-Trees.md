# Lecture 21: Deep Dive into B-Trees and B+ Trees

---

## 1. Introduction: Tree Structures for Disk Storage

In disk-based storage engines, accessing a page from physical disk storage is roughly $100,000\times$ slower than accessing RAM. Traditional Binary Search Trees (BST, AVL, Red-Black Trees) have a low **Fan-Out** (degree = 2), producing tall trees of height $H \approx \log_2 N$, requiring many sequential disk block reads per search lookup.

**B-Trees** and **B+ Trees** are self-balancing multi-way search trees optimized for disk systems. They maximize node **Fan-Out** (degree $m \approx 100 \dots 1000$), keeping tree height exceptionally short ($H \le 3$ or $4$), allowing searches across billions of records in just 3-4 disk block transfers.

---

## 2. B-Tree Internal Architecture

A **B-Tree** of order $m$ is an $m$-way search tree satisfying:
1. Every internal node (except root) has at least $\lceil m / 2 \rceil$ children.
2. The root node has at least 2 children (unless it is a leaf).
3. All leaf nodes appear at the **exact same depth** (perfectly balanced).
4. **Data Pointers in Internal Nodes**: Internal nodes store BOTH search keys AND actual data record pointers (or tuple payloads).

```
                            B-TREE NODE STRUCTURE
       +---------------------------------------------------------------+
       | P1 | Key1 | DataPtr1 | P2 | Key2 | DataPtr2 | ... | Pk | Keyk | DataPtrk | Pk+1 |
       +---------------------------------------------------------------+
```

---

## 3. B+ Tree Internal Architecture

A **B+ Tree** is a specialized variant of a B-Tree designed specifically for database file indexing.

```
                              B+ TREE STRUCTURE
                              
                          [  Root Node: 50  ]   (Internal Nodes: Keys + Child Pointers ONLY)
                         /                  \
                        v                    v
              [ Internal: 20, 35 ]       [ Internal: 65, 80 ]
             /         |         \       /         |         \
            v          v          v     v          v          v
         +------+   +------+   +------+ +------+ +------+ +------+
         |10|15 |==>|20|30 |==>|35|45 |=>|50|60 |=>|65|75 |=>|80|90 | (Leaf Nodes: Data Pointers
         +------+   +------+   +------+ +------+ +------+ +------+  & Doubly-Linked Pointers)
```

---

### Structural Differences: B-Tree vs. B+ Tree

| Feature / Property | B-Tree | B+ Tree |
| :--- | :--- | :--- |
| **Data Pointer Location** | Stored in **both** Internal and Leaf nodes. | Stored **EXCLUSIVELY in Leaf nodes**. |
| **Internal Node Structure** | Stores `(Child_Ptr, Key, Data_Ptr)`. | Stores `(Child_Ptr, Key)` **ONLY**. |
| **Leaf Node Structure** | Independent, unlinked leaf nodes. | Linked sequentially via **Doubly-Linked Lists**. |
| **Fan-Out (Degree $m$)** | Lower (due to data pointer space overhead). | **Significantly Higher** (stores more keys per block). |
| **Point Query Speed** | Can be faster if key is found in internal node. | Uniform search time ($O(\log_m N)$ always reaches leaf). |
| **Range Query Speed** | **Slow**: Requires expensive in-order tree traversal. | **Extremely Fast**: Traverse to start leaf, then follow linked list pointers sequentially! |

---

## 4. Key B+ Tree Operations

### 4.1 Insertion & Node Splitting
1. Traverse tree to locate target leaf node.
2. Insert key in sorted order into leaf node.
3. **If Node Overflows** (contains $> m-1$ keys):
   * Split node into two sibling nodes at median key $\lceil m/2 \rceil$.
   * **In Leaf Node Split**: Copy median key up to parent node, while **retaining** a copy in the right leaf.
   * **In Internal Node Split**: **Push** median key up to parent node (do NOT retain in child).

---

### 4.2 Deletion & Node Merging / Redistribution
1. Locate target leaf node and delete key.
2. **If Node Underflows** (contains $< \lceil m/2 \rceil - 1$ keys):
   * **Borrow (Redistribution)**: Borrow a key from an adjacent left/right sibling if sibling has extra keys.
   * **Merge**: If sibling cannot lend a key, merge node with sibling and pull down parent key.

---

## 5. Mathematical Node Capacity & Order Calculations

### Formula for Order of B+ Tree ($m$):
Let:
* $B$ = Disk Block Size (in bytes).
* $K$ = Key Size (in bytes).
* $P$ = Block Pointer Size (in bytes).
* $D$ = Record / Data Pointer Size (in bytes).

#### 1. Internal Node Constraint:
An internal node of order $m$ contains $m$ child block pointers and $m-1$ search keys:
$$m \cdot P + (m - 1) \cdot K \le B$$
$$m(P + K) - K \le B \implies m \le \frac{B + K}{P + K}$$

#### 2. Leaf Node Constraint (Order $m_{leaf}$):
A leaf node contains $m_{leaf}$ search keys, $m_{leaf}$ data record pointers, and 1 next-leaf block pointer ($P_{next}$):
$$m_{leaf} (K + D) + P_{next} \le B \implies m_{leaf} \le \frac{B - P_{next}}{K + D}$$

---

## 6. Conceptual Practice Exercises

1. Calculate the order $m$ of an internal B+ Tree node given Block Size $B = 512 \text{ bytes}$, Key Size $K = 8 \text{ bytes}$, and Pointer Size $P = 6 \text{ bytes}$.
2. Trace step-by-step insertions of keys $\{10, 20, 30, 40, 50\}$ into a B+ Tree of order $m = 3$.
3. Why are range queries (`WHERE Age BETWEEN 20 AND 30`) significantly faster in B+ Trees compared to B-Trees?

---

## 7. Hard Placement & Interview Questions (FAANG Level)

### Q1: Calculate the MAXIMUM capacity (number of record pointers) of a 3-Level B+ Tree given:
* Block Size $B = 8192 \text{ bytes}$ (8 KB).
* Key Size $K = 16 \text{ bytes}$.
* Block Pointer Size $P = 8 \text{ bytes}$.
* Record Pointer Size $D = 12 \text{ bytes}$.
* Assume node fill factor is 100%.

**Answer:**

1. **Calculate Internal Node Order ($m$)**:
   $$m(P + K) - K \le B$$
   $$m(8 + 16) - 16 \le 8192$$
   $$24m \le 8208 \implies m = \lfloor 8208 / 24 \rfloor = \mathbf{342 \text{ pointers per internal node}}.$$
   * Maximum Keys per internal node = $m - 1 = 341$.

2. **Calculate Leaf Node Capacity ($m_{leaf}$)**:
   $$m_{leaf}(K + D) + P_{next} \le B$$
   $$m_{leaf}(16 + 12) + 8 \le 8192$$
   $$28 m_{leaf} \le 8184 \implies m_{leaf} = \lfloor 8184 / 28 \rfloor = \mathbf{292 \text{ records per leaf node}}.$$

3. **Calculate Total Records in a 3-Level B+ Tree**:
   * Height $H = 3$ (Root at Level 1, Internal Nodes at Level 2, Leaf Nodes at Level 3).
   * Level 1 (Root): 1 Node $\implies m = 342$ Level 2 pointers.
   * Level 2 (Internal): 342 Nodes $\implies 342 \times 342 = 116,964$ Level 3 Leaf pointers.
   * Level 3 (Leaf): 116,964 Leaf Nodes $\implies 116,964 \times 292 = \mathbf{34,153,488 \text{ records}!}$

* **Takeaway**: A 3-level B+ Tree can index over **34 Million Records**, requiring at most 3 block reads per point lookup!

---

### Q2: Explain why PostgreSQL uses B+ Tree variants (nbtree) with Prefix Compression and High-Key boundary pointers for concurrency control.
**Answer:**
* **Prefix Compression**: Index keys often share long common text prefixes (e.g., `https://www.google.com/search?q=1...`). Storing shared prefixes once per node shrinks key size $K$, dramatically increasing node order $m$ and fan-out.
* **Lehman & Yao (L&Y) B-link Tree Algorithm**:
  * Standard B+ Trees require locking parent and child nodes during splits, causing lock contention bottlenecks.
  * PostgreSQL uses **B-link Trees**, which add a high-key right-pointer between internal nodes.
  * Allows readers to traverse down the tree WITHOUT acquiring locks on parent nodes, even while write operations are actively splitting sibling nodes concurrently!

---

### Q3: What is the "Slotted-Page" layout inside a B+ Tree leaf page, and how does it handle variable-length columns (`VARCHAR`, `TEXT`) without memory fragmentation?
**Answer:**
* **Slotted-Page Layout Architecture**:
  ```
  +-----------------------------------------------------------------+
  | PAGE HEADER | SLOT ARRAY (Offsets) -------->   <-- VAR-LENGTH DATA|
  | (Metadata)  | [Offset1][Offset2]...               [Record 2][Rec 1]|
  +-----------------------------------------------------------------+
  ```
* **Mechanism**:
  * **Slot Array**: Located at the top of the page, growing downwards. Contains small 2-byte integer pointers (offsets) pointing to tuple data.
  * **Data Storage**: Located at the bottom of the page, growing upwards. Contains actual variable-length record payload bytes.
* **De-fragmentation Benefit**: When a variable-length row is deleted or updated, the DBMS defragments tuple space inside the page locally by sliding byte boundaries. Slot array pointers are updated in-place without invalidating external B+ Tree block pointers.
