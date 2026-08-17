# Lecture 15: Indexing in DBMS

## 1. File Organization — Storing Records on Disk

Before understanding indexing, it helps to know how records are stored in files on disk:

1. **Heap (Unordered) File Organization**: Records are placed wherever there's free space, in no particular order. Insertion is fast (O(1)), but searching requires a full scan (O(n)).
2. **Sequential (Ordered) File Organization**: Records are stored in sorted order based on a search key. Enables faster searching (binary search-like access), but insertion/deletion is expensive since order must be maintained (may require shifting records).
3. **Hash File Organization**: A hash function maps a search key value directly to the disk block/bucket where the record is stored. Very fast for exact-match lookups but poor for range queries.
4. **Clustered File Organization**: Records with similar/related search key values are physically stored close together (in the same or nearby disk blocks).

## 2. What is an Index?

An **index** is an auxiliary data structure associated with a file of records that allows faster lookup of records based on some **search key**, without needing to scan the entire file. It's conceptually similar to an index at the back of a textbook — instead of reading the whole book to find "Normalization," you look it up in the index and jump directly to the page.

An index consists of **index entries** (also called index records), each typically containing:
- The search key value.
- A **pointer** to the disk block (or exact record) containing the actual data.

## 3. Types of Indexes based on Search Key

1. **Primary Index**: Built on the field that is the primary key of a sequentially-ordered file (same order as the data file's sort order).
2. **Secondary Index**: Built on a non-primary-key field, which may or may not be ordered. Since the underlying data file cannot be sorted by every field simultaneously, secondary indexes typically require an extra level of indirection.

## 4. Primary Index: Dense vs. Sparse

### a. Dense Index
Contains **one index entry for every single search key value** (i.e., for every record) in the data file. Faster lookups (direct pointer to every record), but larger index size.

### b. Sparse Index
Contains index entries for **only some** of the search key values — typically one entry per disk block, pointing to the first record in that block. To find a specific record, the DBMS locates the nearest preceding index entry, then scans sequentially within that block. Smaller index size, but requires additional in-block scanning.

```
Dense Index:                     Sparse Index:
[Key1] -> Record1                [Key1] -> Block1 (scan within)
[Key2] -> Record2                [Key4] -> Block2 (scan within)
[Key3] -> Record3
[Key4] -> Record4
...
```

## 5. Clustered vs. Non-Clustered (Secondary) Indexing

### a. Clustering Index (Clustered Index)
Built on a field that is **not** necessarily the primary key, but where the data file itself IS physically sorted/grouped based on that field. Since only one physical ordering can exist per file, a table can have **at most one** clustered index.

*Example*: An `Orders` table physically stored/clustered by `customer_id`, even though the primary key is `order_id`.

### b. Secondary (Non-Clustering) Index
Built on a field where the data file is **not** physically ordered by that field. The index must therefore contain (essentially) one entry per record (dense), since there's no reliable way to skip within unordered blocks. A table can have **many** secondary indexes.

## 6. Multi-Level Indexing

When a single-level index itself becomes too large to fit in main memory, searching it also becomes slow (requires multiple disk I/Os even on the index itself). **Multi-level indexing** solves this by treating the first-level index as a data file in its own right, and building a second (coarser) index on top of it — and so on, recursively, until the top-most level (called the **root level**) is small enough to fit comfortably in memory.

```
Level 2 (outer index) → small enough to fit in memory
       ↓
Level 1 (inner index) → points into Level 0
       ↓
Level 0 (actual data records)
```

This multi-level structure is the conceptual seed that grows into **B-Trees and B+ Trees** (covered in Lecture 21), which are the actual, self-balancing, dynamically-maintained multi-level index structures used in virtually all production DBMSs.

## 7. Trade-offs of Indexing

| Benefit | Cost |
|---|---|
| Much faster SELECT / WHERE / JOIN / ORDER BY queries | Extra disk space to store the index |
| Faster range queries (for ordered indexes) | Slower INSERT/UPDATE/DELETE (index must also be updated) |
| Can enforce uniqueness (unique indexes) | Maintenance overhead, especially for multi-level structures |

## Summary
An index is a separate, smaller lookup structure that maps search key values to disk locations, dramatically speeding up data retrieval compared to full file scans. Indexes are categorized as **dense** (entry per record) vs. **sparse** (entry per block), and as **primary/clustering** (data physically ordered by the indexed field — only one per table) vs. **secondary/non-clustering** (data not ordered by the field — many per table possible). When a single index level grows too large, **multi-level indexing** recursively indexes the index itself, setting up the foundation for the B-Tree/B+ Tree structures explored in depth in Lecture 21.

## 8. Additional Index Types

- **Composite (Multi-column) Index**: Built on multiple columns together (e.g., `(last_name, first_name)`), useful for queries filtering/sorting on that exact combination or a leading prefix of it.
- **Unique Index**: Enforces that indexed values are distinct — often automatically created for primary keys and any column with a `UNIQUE` constraint.
- **Bitmap Index**: Uses bitmaps (bit arrays) to represent the presence/absence of each possible value; efficient for columns with **low cardinality** (few distinct values, e.g., gender, boolean flags), common in data warehousing.
- **Full-Text Index**: Specialized for efficient searching within large text fields (keyword search), rather than exact-match/range lookups.
- **Hash Index**: Uses a hash function to map keys directly to storage locations — extremely fast for exact-match equality lookups, but doesn't support range queries or sorting.

## 9. When NOT to Use an Index

- On columns with very **low cardinality** (few distinct values) — the index may not narrow down results meaningfully, and a full scan might even be faster.
- On tables with **very high write/update frequency** relative to reads, since every index must also be updated on every write.
- On very **small tables**, where a full scan is already fast enough that index overhead isn't worth it.

---

## Placement Interview Questions — Lecture 15

**Q1. What is an index? Why is it needed?**
An auxiliary data structure that maps search key values to disk locations, allowing much faster data retrieval than a full table scan — needed because scanning every record for every query doesn't scale as data grows.

**Q2. Differentiate between dense and sparse indexes.**
Dense: one index entry per record (faster lookup, larger index size). Sparse: one index entry per disk block (smaller index, but requires scanning within the block after locating the nearest entry).

**Q3. Differentiate between primary, clustering, and secondary indexes.**
Primary index: built on the primary key of a sequentially-ordered file. Clustering index: built on a non-primary-key field where the data file is physically sorted by that field (only one per table). Secondary index: built on a field where the data isn't physically ordered by it (many per table possible, typically dense).

**Q4. Why can a table have only one clustering index but multiple secondary indexes?**
Because a table's data can only be physically stored/sorted in one order at a time, so only one field can have the data physically clustered around it — secondary indexes are separate lookup structures that don't require the underlying data's physical order to match.

**Q5. What is multi-level indexing, and why is it needed?**
When a single-level index itself becomes too large to fit comfortably in memory, searching it also requires multiple disk I/Os; multi-level indexing recursively builds a coarser index on top of the first-level index, continuing until the top level is small enough to fit in memory, drastically reducing the number of disk accesses needed.

**Q6. What are the trade-offs of using an index?**
Faster reads (SELECT, WHERE, JOIN, ORDER BY) at the cost of extra disk space and slower writes (INSERT/UPDATE/DELETE), since every index must also be updated on every write.

**Q7. When would you avoid creating an index on a column?**
On low-cardinality columns (few distinct values), on write-heavy tables where index maintenance overhead outweighs read benefits, and on very small tables where a full scan is already fast.

**Q8. What is a composite index, and when is it useful?**
An index built on multiple columns together, useful for queries that filter or sort on that exact combination (or a leading prefix) of columns — e.g., an index on `(last_name, first_name)` speeds up queries filtering by last_name alone, or by both together, but not by first_name alone.

**Q9. What is a hash index, and what's its major limitation?**
An index using a hash function to map keys directly to storage locations, offering very fast exact-match lookups — but it cannot efficiently support range queries or sorted retrieval, since hashing destroys any ordering relationship between keys.

**Q10. How does file organization (heap, sequential, hash) relate to indexing choices?**
Heap files pair naturally with secondary/dense indexes since there's no inherent order to exploit. Sequential files pair with primary/sparse indexes since the file's physical order matches the index's search key order. Hash files essentially act as their own index via the hash function, but only support equality lookups.
