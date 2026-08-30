# Lecture 15: Indexing in DBMS

---

## 1. Introduction: Why Indexing?

Without an index, searching for a specific record in a database table containing $N$ disk blocks requires a **Sequential File Scan (Full Table Scan)** with $O(N)$ block I/O reads. 

An **Index** is an auxiliary data structure (stored on disk) designed to minimize the number of disk block transfers required to locate matching tuples by providing $O(\log N)$ or $O(1)$ search lookups.

```
       +-------------------------------------------------------+
       |               INDEX ACCESS MECHANISM                  |
       +-------------------------------------------------------+
       | [ Search Key ] ---> [ Index Structure ] ---> [ Record Pointer ]
       |                            |
       |                   (B+ Tree / Hash File)
       +-------------------------------------------------------+
```

---

## 2. Basic Index Classification Architecture

```
                             DATABASE INDEXES
                             /              \
                            /                \
                    Primary Index          Secondary Index
                   (On Ordered Data File) (On Unordered / Secondary Attribute)
                     /           \
                    /             \
             Clustered Index    Non-Clustered Index
            (Data file ordered   (Data file unordered on search key)
             on search key)
```

---

### 2.1 Dense vs. Sparse Indexing

#### 1. Dense Index
* **Definition**: An index record appears for **EVERY SINGLE search key value** (and tuple) present in the data file.
* **Pros**: Extremely fast lookups; can answer index-only queries without reading data pages.
* **Cons**: Consumes significant disk space.

```
  INDEX PAGES                             DATA PAGES
+--------------+                       +-------------------+
| Key | Pointer|                       | Key | Data Fields |
+--------------+                       +-------------------+
| 10  | -------|---------------------->| 10  | Alice       |
| 20  | -------|---------------------->| 20  | Bob         |
| 30  | -------|---------------------->| 30  | Charlie     |
+--------------+                       +-------------------+
```

#### 2. Sparse Index
* **Definition**: An index record appears for **ONLY SOME search key values** (typically one index entry per data disk block).
* **Requirement**: The underlying data file **MUST BE PHYSICALLY SORTED** on the search key.
* **Pros**: Requires significantly less memory and disk storage space.

```
  SPARSE INDEX                            SORTED DATA BLOCKS
+--------------+                       +-------------------+
| Key | Pointer|                       | Block 1 (Keys 10, 15, 18)
+--------------+                       +-------------------+
| 10  | -------|---------------------->| 10  | Alice       |
| 40  | -------|------------+          | 15  | Bob         |
+--------------+            |          +-------------------+
                            |          | Block 2 (Keys 40, 42, 50)
                            |          +-------------------+
                            +--------->| 40  | David       |
                                       | 42  | Eve         |
                                       +-------------------+
```

---

### 2.2 Primary vs. Secondary vs. Clustered Indexes

#### 1. Primary Index
* Built on an **Ordered Data File** using the relation's **Primary Key**.
* Always Sparse (typically 1 index entry per data block).

#### 2. Clustered Index (Clustering Index)
* Built on an **Ordered Data File** using a **Non-Key (duplicate-allowed)** attribute (e.g., `Department_ID`).
* Data file is physically sorted so that all records sharing the same search key value are contiguous on disk.

#### 3. Secondary Index (Non-Clustered Index)
* Built on an attribute where the physical data file is **NOT SORTED** on that search key.
* MUST be a **Dense Index** (every search key or row must have a corresponding index pointer).
* Points to physical tuple record IDs (RID: `Block_ID + Offset`) or Primary Key values.

---

### 2.3 Comparison Summary Matrix

| Feature | Primary Index | Clustered Index | Secondary Index |
| :--- | :--- | :--- | :--- |
| **Data File Physical Order** | Sorted on Primary Key | Sorted on Non-Key Attribute | Unsorted |
| **Search Key Key-ness** | Unique Key | Non-Unique Key | Unique or Non-Unique |
| **Dense or Sparse?** | Sparse | Sparse | **Dense** |
| **Max Quantity Per Table** | **1** | **1** | **Multiple (N)** |

---

## 3. Multi-Level Indexing

When a primary index file grows too large to fit in RAM memory, searching the index itself becomes an I/O bottleneck. A **Multi-Level Index** treats the outer index as a primary file and builds a secondary sparse index over it.

```
  OUTER INDEX (Level 2)        INNER INDEX (Level 1)          DATA BLOCKS
+-----------------------+    +-----------------------+    +------------------+
| Key | Inner Ptr       |    | Key | Data Ptr        |    | Data Records     |
+-----------------------+    +-----------------------+    +------------------+
| 100 | ----------------|--->| 100 | ----------------|--->| Records 100-199  |
| 500 | ----------------|--->| 500 | ----------------|--->| Records 500-599  |
+-----------------------+    +-----------------------+    +------------------+
```

---

## 4. Conceptual Practice Exercises

1. Why can a relational table have at most ONE Clustered Index, but MULTIPLE Secondary Indexes?
2. Given a sorted data file with 1,000,000 records stored across 10,000 disk blocks. Calculate the number of block accesses needed to find a record using:
   a) Binary Search on raw data blocks.
   b) A Single-level Sparse Primary Index (where 100 index entries fit per block).
3. Differentiate between an Index-Only Scan and a Full Table Scan.

---

## 5. Hard Placement & Interview Questions (FAANG Level)

### Q1: Calculate the exact I/O cost reduction for a table containing $r = 1,000,000$ records with block size $B = 4096 \text{ bytes}$. Each record is $200 \text{ bytes}$. The search key field is $16 \text{ bytes}$ and block pointer is $8 \text{ bytes}$. Compare Binary Search vs. Two-Level Index Search.

**Answer:**
1. **Data File Block Calculations**:
   * Blocking Factor $BF_{data} = \lfloor 4096 / 200 \rfloor = 20 \text{ records per block}$.
   * Number of Data Blocks $b_{data} = \lceil 1,000,000 / 20 \rceil = 50,000 \text{ blocks}$.
   * **Binary Search I/O Cost on Data Blocks**: 
     $$\lceil \log_2(50000) \rceil = \mathbf{16 \text{ block I/O accesses}}.$$

2. **Level 1 Sparse Index Calculations**:
   * Index Entry Size $R_{ind} = 16 + 8 = 24 \text{ bytes}$.
   * Index Blocking Factor $BF_{ind} = \lfloor 4096 / 24 \rfloor = 170 \text{ index entries per block}$.
   * Level 1 Index Entries = 50,000 (one per data block).
   * Level 1 Index Blocks $b_{ind1} = \lceil 50,000 / 170 \rceil = 295 \text{ blocks}$.

3. **Level 2 Sparse Index Calculations**:
   * Level 2 Index Entries = 295 (one per Level 1 index block).
   * Level 2 Index Blocks $b_{ind2} = \lceil 295 / 170 \rceil = 2 \text{ blocks}$.

4. **Total Two-Level Index Search Cost**:
   * Read Level 2 block (1 I/O) + Read Level 1 block (1 I/O) + Read Data Block (1 I/O) = **3 Block I/O Accesses**!
   * *Performance Gain*: Reduced disk I/O from 16 to 3 accesses (81% performance improvement).

---

### Q2: What is the "Write Amplification" penalty of secondary indexes during bulk `INSERT` and `UPDATE` operations, and how do database buffer pools use Change Buffering (Insert Buffer) to mitigate it?
**Answer:**
* **Write Amplification Penalty**: When inserting a single row into a table with $k$ secondary indexes, the DBMS must update the primary clustered table block PLUS perform $k$ separate random disk writes to insert entries into $k$ secondary B+ Tree index structures.
* **InnoDB Change Buffer Optimization**:
  * If a secondary index page is not currently in the memory Buffer Pool during an `INSERT`/`UPDATE`, the DBMS defers the random disk write.
  * The change is cached in memory in the **Change Buffer**.
  * When the secondary index page is later read into memory by another query, the buffered updates are merged asynchronously in a single operation, converting random I/O into fast sequential memory operations.

---

### Q3: Why do secondary indexes in MySQL InnoDB store Primary Key values as record pointers instead of direct physical memory block offset pointers (RIDs)? Explain the trade-off.
**Answer:**
* **Design Choice**: InnoDB secondary indexes store `(Secondary_Key_Value, Primary_Key_Value)` instead of physical disk row addresses `(Page_ID, Slot_Offset)`.
* **Trade-off Analysis**:
  * **Disadvantage**: Secondary index lookups require a two-step traversal: first traverse secondary B+ Tree to find Primary Key, then traverse Primary Clustered B+ Tree to fetch full data row ("Bookmark Lookup").
  * **Advantage (Page Splits & Defragmentation)**: When primary table rows are updated or page splits occur in the clustered B+ Tree, physical memory block addresses of thousands of rows change. If secondary indexes used physical RIDs, *every single secondary index* across the table would require immediate updating. By using Logical Primary Key pointers, physical page movements require **zero updates** to secondary index trees.
