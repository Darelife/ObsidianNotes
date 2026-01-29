- A bunch of nand cells (helps with concurrency)

# Nand cells
- here, we have 3 ops : read, write, and erase (hdds don't have erase)
- we have to erase an entire block, and then only can we write to its pages
- Need to use a garbage collector (overhead)
- There's a limit to the amount of times that we can erase
![[Pasted image 20260127113113.png]]
MLC = 2 bits in a single cell (4 levels of the amount of electrons)
TLC = 3 bits...

# FTL
## Mapping
maps LBA to the ssd thing (Physical Base Address / Physical Page Address)

## Garbage Collection

## Wear Level
Every nand cells has a limited number of writes. Have to evenly distribute the wears.

Eg: A **1 TB TLC SSD** has a rated endurance ≈ **600 TBW** (terabytes written)


# FTL Mapping Strats (Page | Block | Hybrid)

## Page (didn't understand) (4 KB granularity)

## Block Level Mapping (256 KB granularity)

Each LBA is mapped to a block, instead of a page. 
Cuts RAM usage significantly (~64x less), compared to page level.

## Hybrid
1. Block Mapping Table (LBN -> PBN)
2. Log Page Mapping Table (LPMT) ((LBN, page_offset) → (LogBlock, page))

Eg:
```
LBN 10 → PBN 100

PBN 100 (DATA BLOCK):
  Page 0 → (10,0)
  Page 1 → (10,1)
  Page 2 → (10,2)
  Page 3 → (10,3)

Step 2: Overwrite ONE page

Host overwrites logical page (10,2).

What NAND forbids

Cannot overwrite page 2 in PBN 100

Cannot erase PBN 100 (it has valid data)


PBN 300 (LOG BLOCK):
  Page 0 → updated (10,2)

update log
(10,2) → (300,0)

resulting state
PBN 100 (DATA):
  Page 2 = OLD (invalid logically, but not erased)

PBN 300 (LOG):
  Page 0 = NEW (10,2)

---

# Step 3: Another overwrite (same block)

Host overwrites **logical page (10,0)**.

### Action

- Write into same log block
    

`PBN 300 (LOG BLOCK):   Page 0 → (10,2)   Page 1 → (10,0)`

### Update mapping

`(10,0) → (300,1)`

---

# Step 4: Reads (important!)

Read `(10,2)`:

1. Check log mapping → FOUND
    
2. Read from `PBN 300, Page 0`
    

Read `(10,1)`:

1. Not in log table
    
2. Read from `PBN 100, Page 1`
    

---

# Step 5: Log block fills up

Host overwrites `(10,1)` and `(10,3)`.

`PBN 300 (LOG BLOCK):   Page 0 → (10,2)   Page 1 → (10,0)   Page 2 → (10,1)   Page 3 → (10,3)`

Log block is **full**.

---

# Step 6: Merge operation (the unavoidable erase)

Now hybrid FTL **must clean up**.

### Case here: **Switch merge** (best case)

Because log block contains **ALL pages of LBN 10**.

### Actions

1. Update block mapping:
    

`LBN 10 → PBN 300`

2. Erase old data block:
    

`Erase PBN 100`

3. Log block becomes the new data block
    

🔥 **No page copying needed** — very cheap!

---

# Step 7: If log block were partial (worse case)

Assume only `(10,2)` and `(10,0)` were updated.

### Partial merge steps

1. Allocate new block `PBN 400`
    
2. Copy:
    
    - Updated pages from log block
        
    - Unchanged pages from old data block
        
3. Erase:
    
    - Old data block (100)
        
    - Log block (300)
        
4. Update:
    

`LBN 10 → PBN 400`

This is where write amplification happens.
```

# Out of Place Update: Elimination Erase before Write

# Memory Table Recovery, & Power Loss Protection
(out of band area in nand flash blocks)