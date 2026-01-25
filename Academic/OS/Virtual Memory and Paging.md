[[Threads]]
# Virtual Memory

Procs think that they have access to the entire mem
But they actually just have access to the virtual mem space

Eg: 32 bit machines = 2^32 = 4GB = Max value

Virtual address space contains all the code/data that a process can access. CPU registers and pointer variables all access the virtual address space only. All the load and store instructions are also done on the VA's.

## The address space of OS

OS isn't a separate process, with its own address space, but instead OS is a part of the address space of every process. Each process sees the OS as a part of its code (like the shared libraries and all)

Page tables will later map the OS address to the OS code.

## Memory access using Virtual Addressing

The code + data in the memory image is assigned virtual address starting from 0. On each memory access, the VA is translated to PA by a special hardware called the MMU (Mem Management Unit)

### Address Translation
#### Simplest form : Base and Bound

Place the memory image \[0, N] contiguously starting at memory address base B. VA X just gets added to it.
`PA = B + X`
PA <= N (the bound)

When the CPU access a VA, MMU computes PA = VA+base, using 2 registers, base and bound


Suppose each process takes 16KB, and we have the base as 32 KB....so bound = 48 KB.

VA = 128

PA = 32KB + 128 = 32896


#### Role of OS vs MMU in the translation

OS allocates memory from a free list, builds translation information of process, but OS doesn't do the actual address translation on every mem access, since once the code is on the CPU, OS is out of the picture (unless it's a trap and it's called)

#### Problem with a single Base & Bound

There's a lot of free space between the heap and stack, which isn't being used, but takes up space in the mem. 

Lets use a different approach

#### Explicit Approach to pin point the segment
Eg: in 14 bits, we'll put 2 bits for the segment, and the remaining 12 for the offset. 00 = code, 01 = heap, and 10 = stack

```c
if (offset >= bounds[segment]) {
	RaiseException(PROTECTION_FAULT);
} else {
	PhysAddr = base[segment] + offset;
	Register = AccessMemory(PhysAddr);
}
```


We'll also add growth & sharing

| Segment    | Base | Size (max 4K) | Grows Positive | Protection   |
| ---------- | ---- | ------------- | -------------- | ------------ |
| Code (00)  | 32K  | 2K            | 1              | Read-Execute |
| Heap (01)  | 34K  | 3K            | 1              | Read-Write   |
| Stack (11) | 28K  | 2K            | 0              | Read-Write   |

#### Problems in Segmentation
- OS' task increases
	- VA space is contiguous : Map the VA space to different segments in the Phys Mem.
	- Set up all the segment Registers
- When the heap needs to grow,
	- OS needs to map more segments to the existing heap segment


# Paging

Chop up the mem into fixed sized pieces...still might have internal fragmentation

> [!info] Internal vs External Fragmentation
> **Internal Fragmentation** = Memory space > Mem process needs
> So, there's gap in the region
> 
> **External Fragmentation** = Free space is broken into small chunks, eventually leading to the case where no section is able to satisfy a request 

## Page Table

Each process has a Page Table, that can translate the VA address to the Physical Address. It stores the frame numbers for all pages of a process in an array.

The MMU has access to the PT of the current process, and uses it for the address translation via a special register by the OS...updated on every context switch/page table change.

VA = xxxxyyyyyyy
PA = AddressTranslation(xxxx)yyyyyyy


Page table is an array, where the ith entry contains the info (physical frame number, etc) of the ith page of the proc.

PT has entries for all pages, even if it's not used, or doesn't have a physical page number. 

> [!question] PT is an array with a fixed number of entries...it's not dynamic, whose structure depends on the number of used pages. Why?
> 
> We need superfast indexing, with constant lookup time O(1). Also, each VA must map deterministically. So, we want to use the VA directly as an index (ignoring the offset bits). Other than this, MMU is a hardware unit, and requires a simple fixed efficient formula to do it. Hardware must remain simple, fast and reliable. If they were dynamic, we would need a search structure, pointers, would have variable latency. We need to do it billions of times per second, so it would be unacceptable. 
> 
> Also, inorder to reduce the wastage of the huge PT in memory, we use Multi Level page tables. (we'll learn more about this)


## PTE (Page Table Entry)

Each PTE consists of the Page Frame Number, Valid Bit, Present bit, Dirty bit, etc

Valid Bit = 1 if it's part of the process.....helps especially when everything is just initialized...nothing is valid...so, we can use that space.
Present Bit = 1 if it's in the RAM..if it's 0, it could have been swapped out, or not allocated...or straight up illegal. If P = 0, page fault.

### Address Translation

```c
VA = (Virtual Page Number)(Offset)
PTE[Virtual Page Number] = {Valid, Physical Frame Number, ...}

Page Table Base Register from the MMU points to the Base of the PTE in the memory 

if (!valid) {
	trap
} else {
	PA = Physical Frame Number + Offset
}
```

### Size of the page table

In a 32 bit system, Virtual Address Space = 2^32 = 4GB

Assume Page Size = 4KB = 2^12

Number of PTE's = 2^(32-12) = 2^20 = 1024\*1024 = 1 M
If each PTE is 4 bytes, For each process, each PTE takes 4MB.

How do we store 4MB?? if the entire memory is broken in 4KB chunks??

#### Solution 1
Bigger Pages ->  internal fragmentation

#### Solution 2
Hybrid approach : Paging + Segmentation (i didn't understand this)

#### Solution 3
Multi level page table

4MB page table can be split into 1024 chunks of 4KB each. 1M PTEs split across 1024 pages, each containing 1024 PTEs. 

instead of a single 1024\*1024 page table, we'll have 1 page table pointing to 1024 page tables, with each of those 1024 PTs having 1024 PTEs.

Eg: address translation for a 2 level PT

32 bit VA -> 20 bit page number + 12 bit offset (12 bit cuz of 4KB)
20 bit index -> 10 bit (MSB side) for the outer page table, and the 10 bit (LSB side) for the inner page table.

MMU basically walks through the multiple levels of the page table, to translate the VA.

> [!question] 48 bit CPU, 4KB pages, 8 byte page table entries. How many levels??
> 
> 4KB = 2^2 * 1024 bytes = 2^12 => 12 bytes offset
> so, 48 - 12 = 36 bits for the page table. 
> Number of PTEs in 1 page/frame = 4KB/8 bytes = 512 = 2\^9
> Number of total PTEs = 2^36 = max number of possible pages
> Level last can overlook = 512 = 2^9
> Level 2nd last can overlook = 512\*512 = 2^18
> Level 3rd Last can overlook = 512 \* (512 \* 512) = 2^27
> Level 4th last can overlook = 512 \* (512 \* 512 \* 512) = 2^36
> 
> We are just multiplying it by 512 in each level.....don't accidentally do (lastLevel)^2. I accidentally did that initially
> 
> Hence we need a 4 level page table
> 
> For the number of layers....it's just 
> ciel(
> 	log2(total PTEs)/log2(PTEs in each page)
> ) 
> = 36/9 = 4
> 
> or, 
> 
> ciel(
> 	(VA bits - log2(pageSize)) / (log2(pageSize/PTEsize))
> )
> 
> For each layer, we need 9 bits too...cuz like, 512 entries in each

However, this can still be slow. This is where we involve a new component

## TLB (Translation Lookaside  Buffer)

TLB has only the VA-PA mapping....if it's a TLB miss, the MMU has to access 4 different memory locations for 4 levels of page table, in order to translate one VA to PA. MMU page table walks become even longer...hence TLB hit is critical.

TLB is basically a cache for the MMU

TLB gets flushed on context switches...mappings caches in the TLB change.

### TLB Hit

1. CPU accesses the VA
2. MMU looks up the page number in TLB
3. If TLB hit, page table entry is available. CPU directly access the required frame using the physical address.

### TLB Miss

1. CPU accesses the VA
2. MMU looks up the page number in TLB, and cant find the entry.
3. MMU looks up the page table in memory to find the PTE.
4. PTE gets populated in the TLB for future use.
5. MMU computes the PA for the CPU to access the main memory.

## OS is a part of the address space for every process

OS is mapped into the virtual address space of every process at addresses not used by the process ie. high VA. This is done so that we can easily jump to the OS code during a trap. OS is also mapped via the PTEs. There's only 1 copy of the OS in RAM, and is loaded at low PAs during system bootup. 

## Page Level Security

PT has permissions for every memory page. Whether it can be accessed in User mode, or read and write. MMU traps to the OS if any violation is detected during memory access. It ensures that user programs can only access memory they are permitted to access.

# Inverted Page Table

Instead of having many PT (one per process), we keep a single PT that has an entry for each physical page of the system. PTE also stores info about the proc, and which VA of that proc maps to the PA. It uses a hash table. The linear scan of this huge page is very expensive.

[[Locks]]