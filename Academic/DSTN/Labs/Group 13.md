[[project Report]]

1. Prakhar Bhandari (2023A7PS0458G)
   Satvik Agrawal (2023A7PS0454G)
2. zipped directory (in folder) /home/darelife/darshan-logs/2026/4/15
3. Number of CPU Cores: 6
   Number of CPU Sockets: 1
   Memory: 15Gi
   Storage: 118G (In the partition), overall: 476.9G

# Q1

## FPP vs SSF

|Metric|FPP (File-per-process)|SSF (Single shared file)|Reason|
|---|---|---|---|
|Total Runtime|2.1581 s|2.0052 s|SSF avoids inode-creation overhead; lock contention is smaller than FPP metadata cost (at 4 ranks)|
|MPIIO_INDEP_OPENS|4|0|FPP: each rank opens its own file independently|
|MPIIO_COLL_OPENS|0|4|SSF: requires collective open (global sync across ranks)|
|MPIIO_INDEP_WRITES|4|4|Same at MPI layer, but SSF incurs POSIX locking underneath|
|MPIIO_COLL_WRITES|0|0|Neither uses collective writes|
|MPIIO_BYTES_WRITTEN|67 MB per rank|268 MB total|Same total data; reporting differs (per-file vs aggregated)|
|MPIIO_F_META_TIME|~0.00246 s total|0.147724 s|SSF: heavy coordination + collective open overhead|
|MPIIO_F_WRITE_TIME|~0.025 s per rank|0.054242 s total|SSF: serialized due to file locking|
|MPIIO Rank Variance|0|High (≈4× spread)|SSF: lock contention + uneven collective overhead|
|POSIX_F_META_TIME|~0.000024 s total|0.000016 s|SSF: shared inode serializes metadata ops|
|POSIX Rank Variance|0|Non-zero|SSF: lock acquisition order causes imbalance|
|POSIX Opens|4 inodes|1 inode (shared)|Root cause of locking + metadata differences|
|File Alignment|4096 B|4096 B|Same filesystem behavior (ext4)|
FPP vs SSF – Key Takeaways  
- Primary bottleneck differs by configuration: FPP is limited by metadata overhead (multiple file creations), while SSF is limited by lock contention on a shared inode.  
- SSF achieves slightly lower runtime at small scale because avoiding multiple inode creations outweighs locking cost at 4 ranks.  
- SSF introduces significant MPI-IO metadata overhead due to collective open/close and coordination across ranks.  
- High rank variance in SSF indicates load imbalance caused by lock acquisition order; FPP avoids this entirely due to independent files.  
- Secondary effect: SSF’s shared inode causes cascading delays as ranks serialize, while FPP shifts pressure to filesystem metadata (inode management).  
- Overall trade-off: FPP scales better in terms of parallelism, SSF reduces metadata cost but risks serialization and imbalance.

## Sequential Vs Random

| Metric                     | Sequential         | Random                | Reason                                                                                                                                    |
| -------------------------- | ------------------ | --------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| Total Runtime              | 2.6285 s           | 3.0542 s              | Random I/O causes seek overhead and cache thrashing, increasing total execution time                                                      |
| MPIIO_INDEP_OPENS          | 0                  | 0                     | Both use SSF (collective open); no independent opens                                                                                      |
| MPIIO_COLL_OPENS           | 4                  | 4                     | Identical — both perform one collective open across 4 ranks                                                                               |
| MPIIO_INDEP_WRITES         | 262144             | 262144                | Same total write count; access pattern doesn't change operation count                                                                     |
| MPIIO_COLL_WRITES          | 0                  | 0                     | Neither uses collective writes                                                                                                            |
| MPIIO_BYTES_WRITTEN        | 268435456 (256 MB) | 268435456 (256 MB)    | Same total data written; only the access pattern differs                                                                                  |
| MPIIO_F_META_TIME          | 0.330354 s         | 1.666494 s            | Random I/O causes frequent offset-driven lock contention and metadata revalidation at the POSIX layer, heavily inflating MPI-IO meta time |
| MPIIO_F_WRITE_TIME         | 2.112793 s         | 2.210282 s            | Slightly longer for random; non-sequential seeks prevent OS write-combining and prefetcher optimizations                                  |
| POSIX_SEQ_WRITES           | 262143             | 131276                | Nearly all sequential writes vs. ~half, confirming random access pattern; the rest are non-sequential                                     |
| POSIX_CONSEC_WRITES        | 262140             | 4                     | Almost all writes are exactly adjacent in sequential; near-zero in random — direct evidence of scattered offsets                          |
| POSIX_STRIDE1_STRIDE       | 0                  | 12466176 B (~11.9 MB) | No dominant stride in sequential; random accesses have a detectable large stride (inter-rank chunk spacing)                               |
| POSIX_ACCESS1_ACCESS       | 1024 B             | 1024 B                | Most common I/O size is identical — 1 KB writes in both cases                                                                             |
| POSIX_F_WRITE_TIME         | 0.261259 s         | 0.340562 s            | Random writes take ~30% longer at POSIX layer due to page-cache misses and non-sequential dirty page flushing                             |
| POSIX_F_META_TIME          | 0.000025 s         | 0.000027 s            | Negligible difference; POSIX metadata (open/close) unaffected by access pattern                                                           |
| POSIX_F_VARIANCE_RANK_TIME | 0.000000           | 0.000003              | Sequential is perfectly balanced; random introduces slight rank imbalance due to non-uniform seek distances                               |
| POSIX_FILE_ALIGNMENT       | 4096 B             | 4096 B                | Same ext4 filesystem; alignment behavior unchanged                                                                                        |
| POSIX_OPENS (shared file)  | 4                  | 4                     | Same SSF setup; 4 ranks open the same inode                                                                                               |
Sequential vs Random – Key Takeaways  
- Primary bottleneck in random access is poor locality, causing higher seek cost, cache misses, and inefficient write combining compared to sequential.  
- Sequential achieves better performance due to contiguous access, enabling OS-level optimizations like prefetching and write buffering.  
- Random access significantly increases MPI-IO metadata time due to frequent lock updates and offset changes.  
- POSIX-level evidence (low consecutive writes, reduced sequential writes) confirms fragmentation of access pattern in random case.  
- Slight rank variance in random indicates emerging imbalance due to uneven access delays, unlike perfectly balanced sequential case.  
- Secondary effect: inefficient access pattern propagates through MPI-IO and POSIX layers, amplifying overhead beyond just disk latency.

# Q2

| Metric                     | Random FPP              | Random SSF                     | Reason                                                                                                                                  |
| -------------------------- | ----------------------- | ------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------- |
| Total Runtime              | 2.7426 s                | 2.6970 s                       | SSF marginally faster despite higher lock overhead; FPP's 4 inode creations + independent metadata paths add up                         |
| MPIIO_INDEP_OPENS          | 4 (1/rank)              | 0                              | Each FPP rank opens its own private file independently                                                                                  |
| MPIIO_COLL_OPENS           | 0                       | 4                              | SSF requires a collective open — all 4 ranks synchronise before any I/O begins                                                          |
| MPIIO_INDEP_WRITES         | 65,536/rank             | 262,144 (agg.)                 | Same total write call count; reported per-file vs. aggregated                                                                           |
| MPIIO_COLL_WRITES          | 0                       | 0                              | Neither uses collective writes (independent random I/O)                                                                                 |
| MPIIO_BYTES_WRITTEN        | 67,108,864/rank (64 MB) | 268,435,456 total (256 MB)     | Same total data; FPP reports per-file, SSF aggregates across ranks                                                                      |
| MPIIO_F_WRITE_TIME         | ~0.549 s/rank (avg.)    | 2.357 s (agg. ≈ 0.589 s/rank)  | SSF ranks serialise on POSIX byte-range locks for every random write, inflating per-rank I/O time                                       |
| MPIIO_F_META_TIME          | ~0.000536 s/rank (avg.) | 0.2432 s (agg. ≈ 0.061 s/rank) | ~114× higher in SSF: repeated lock/unlock for non-contiguous random offsets dominates metadata cost                                     |
| POSIX_WRITES               | 65,536/rank             | 262,144 (agg.)                 | 1:1 mapping between MPI and POSIX writes; no buffering or aggregation                                                                   |
| POSIX_ACCESS1_ACCESS       | 1,024 B                 | 1,024 B                        | All writes are 1 KB — very small, causing high syscall overhead                                                                         |
| POSIX_SEQ_WRITES           | 32,819/rank (~50%)      | 131,276 agg. (~50%)            | ~50% of random offsets happen to be increasing; no true sequential pattern                                                              |
| POSIX_CONSEC_WRITES        | 1/rank (≈ 0%)           | 4 agg. (≈ 0%)                  | No consecutive (adjacent) writes — confirms fully random access pattern                                                                 |
| POSIX_STRIDE1_STRIDE       | 12,466,176 B (~11.9 MB) | 12,466,176 B                   | Large dominant stride confirms random, spread-out access across file                                                                    |
| POSIX_FILE_NOT_ALIGNED     | 0                       | 196,608 / 262,144 (75%)        | Critical bottleneck: random offsets from 4 ranks are rarely 4 KB-aligned in a shared file, forcing sub-page reads-before-writes on ext4 |
| POSIX_F_WRITE_TIME         | ~0.076 s/rank           | 0.454 s (agg. ≈ 0.114 s/rank)  | SSF POSIX write time is ~1.5× higher per rank due to lock serialisation                                                                 |
| POSIX_F_VARIANCE_RANK_TIME | N/A (separate files)    | 0.000003 s²                    | Near-zero variance: all ranks are equally throttled by lock contention — it is the bottleneck, not an outlier rank                      |
  
Random FPP vs Random SSF – Key Takeaways  
- Primary bottleneck in SSF is lock contention for every small random write, while FPP avoids locking but pays metadata overhead for multiple files.  
- SSF has significantly higher MPI-IO metadata time due to repeated lock/unlock operations for non-contiguous writes.  
- Small 1 KB write size amplifies syscall overhead and prevents effective aggregation, hurting both configurations but more severely in SSF.  
- Misalignment in SSF (majority of writes not 4 KB aligned) introduces additional read-modify-write overhead at the filesystem level.  
- Despite higher contention, SSF shows near-zero variance because all ranks are uniformly throttled; FPP avoids contention entirely.  
- Secondary effect: high frequency of small writes causes cascading overhead across MPI, POSIX, and filesystem layers, severely limiting scalability.

# Q3

|Metric|Sequential FPP|Sequential SSF|Reason|
|---|---|---|---|
|Total Runtime|2.6907 s|2.6993 s|Nearly identical; sequential access in both cases avoids lock contention that hurts parallel SSF|
|MPIIO_INDEP_OPENS|4 (1 per rank)|0|FPP: each rank opens its own file independently|
|MPIIO_COLL_OPENS|0|4|SSF: requires a collective open even in sequential mode|
|MPIIO_INDEP_WRITES|4 × 65,536 = 262,144 total|262,144 (aggregated)|Same total write count; reporting differs (per-file vs shared)|
|MPIIO_COLL_WRITES|0|0|Neither uses collective writes|
|MPIIO_BYTES_WRITTEN|67,108,864 per rank|268,435,456 total|Same total data (256 MB); FPP reports per rank, SSF aggregates|
|MPIIO_F_META_TIME|~0.000510–0.000628 s per rank|0.094380 s total|SSF collective open adds coordination overhead even sequentially|
|MPIIO_F_WRITE_TIME|~0.556–0.580 s per rank|2.375612 s total|SSF aggregates all ranks; per-rank write time is ~0.59 s in both cases (similar actual throughput)|
|MPIIO Rank Variance|0 (per-file, no sharing)|0.000691|SSF has slight variance from collective open timing differences|
|POSIX_OPENS|4 inodes (1 per rank)|1 inode (shared)|Root structural difference between FPP and SSF|
|POSIX_BYTES_WRITTEN|67,108,864 per rank|268,435,456 total|Same data, different reporting scope|
|POSIX_SEQ_WRITES|65,535 per rank|262,143 total|All writes are sequential in both cases — confirms access pattern|
|POSIX_F_WRITE_TIME|~0.066–0.067 s per rank|0.418480 s total|SSF total is ~4× FPP per-rank, consistent with 4 ranks writing sequentially through one file|
|POSIX_F_META_TIME|~0.000005–0.000032 s per rank|0.000024 s total|Both very low; shared inode doesn't bottleneck metadata in sequential single-writer scenario|
|POSIX Rank Variance|0 (no sharing)|0.000011|Small but non-zero in SSF due to shared inode; much lower than parallel SSF case|
|File Alignment|4096 B|4096 B|Same ext4 filesystem behavior|
**Key takeaway:** Unlike the parallel case, sequential SSF and FPP perform nearly identically in runtime (~2.69 s). The main differences are structural — SSF uses a collective open, aggregates all counters under one record, and shows a tiny non-zero rank variance. In parallel, SSF suffers from lock contention; in sequential mode, only one rank writes at a time, so that bottleneck disappears.

# Q4

## File Per Process

| Metric                        | FPP 8 MB/rank    | FPP 64 MB/rank     | FPP 124 MB/rank      | Reason                                                                                                                                                                                      |
| ----------------------------- | ---------------- | ------------------ | -------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Total Runtime                 | 0.2647 s         | 2.2542 s           | 4.2475 s             | Scales roughly linearly with total data (32 → 256 → 496 MB); includes MPI init and `-v` verification overhead beyond pure I/O                                                               |
| MPIIO_INDEP_OPENS             | 4 (1/rank)       | 4 (1/rank)         | 4 (1/rank)           | FPP always: each rank opens its own file independently                                                                                                                                      |
| MPIIO_COLL_OPENS              | 0                | 0                  | 0                    | No collective open needed in FPP                                                                                                                                                            |
| MPIIO_INDEP_WRITES            | 1/rank           | 1/rank             | 1/rank               | Each rank issues exactly one large contiguous write — no looping                                                                                                                            |
| MPIIO_COLL_WRITES             | 0                | 0                  | 0                    | FPP never uses collective writes                                                                                                                                                            |
| MPIIO_BYTES_WRITTEN           | 8,388,608 (8 MB) | 67,108,864 (64 MB) | 130,023,424 (124 MB) | Directly set by `-k` block count × 1024 B                                                                                                                                                   |
| MPIIO_F_WRITE_TIME (avg/rank) | ~0.0023 s        | ~0.1342 s          | ~0.0789 s            | Non-monotonic: 8 MB fits in page cache (nearly instant); 64 MB causes cache pressure and synchronous writeback; 124 MB triggers efficient sequential disk streaming (higher raw throughput) |
| MPIIO_F_META_TIME (avg/rank)  | ~0.000560 s      | ~0.000659 s        | ~0.000655 s          | Nearly constant across all sizes — metadata cost (open/close) is size-independent                                                                                                           |
| POSIX_WRITES                  | 1/rank           | 1/rank             | 1/rank               | MPI-IO passes the whole buffer in one write() syscall — ideal for sequential I/O                                                                                                            |
| POSIX_ACCESS1_ACCESS          | 8,388,608 B      | 67,108,864 B       | 130,023,424 B        | The single most common (and only) write size equals the full file size                                                                                                                      |
| POSIX_SIZE_WRITE bucket       | 4M_10M = 1       | 10M_100M = 1       | 100M_1G = 1          | Each file size falls into the next histogram bin — useful for identifying I/O regime                                                                                                        |
| POSIX_SEQ_WRITES              | 1/rank           | 1/rank             | 1/rank               | The single write is sequential by definition (first and only write from offset 0)                                                                                                           |
| POSIX_CONSEC_WRITES           | 0                | 0                  | 0                    | No consecutive (back-to-back adjacent) writes since there is only 1 write total                                                                                                             |
| POSIX_FILE_NOT_ALIGNED        | 0                | 0                  | 0                    | All writes start at offset 0 — perfectly page-aligned in all cases                                                                                                                          |
| POSIX_MEM_NOT_ALIGNED         | 0                | 0                  | 0                    | MPI buffer is aligned; no sub-page memory penalties                                                                                                                                         |
| POSIX_F_META_TIME (avg/rank)  | ~0.000005 s      | ~0.000005 s        | ~0.000005 s          | Trivial and constant — open/close latency independent of file size                                                                                                                          |
| Implied write BW/rank         | ~3,520 MB/s      | ~478 MB/s          | ~1,571 MB/s          | Key bottleneck indicator: 8 MB is absorbed by page cache (memory speed); 64 MB saturates cache and spills to disk with no streaming benefit; 124 MB gets sequential disk throughput gains   |

**Key takeaways:**

- The most important observation is the **non-monotonic `MPIIO_F_WRITE_TIME`**: 8 MB writes complete in ~2 ms (memory-speed), 64 MB writes balloon to ~134 ms (cache eviction pressure), then 124 MB drops back to ~79 ms. This is not a contradiction — at 124 MB the kernel's writeback scheduler recognises the sequential, large-block pattern and pipelines disk I/O more efficiently. The 64 MB case hits the worst spot: too large for clean page cache absorption, too small to benefit from streaming optimisation.
- `MPIIO_F_META_TIME` is flat at ~0.6 ms across all three sizes, confirming that open/close overhead is a **fixed cost** and becomes proportionally negligible as file size grows.
- Because all three runs are pure FPP with a single contiguous write per rank, `POSIX_FILE_NOT_ALIGNED = 0` and `POSIX_CONSEC_WRITES = 0` everywhere — no alignment or sequencing pathologies. The **only bottleneck is raw bandwidth scaling** with data volume.
- Total wall time is dominated by non-I/O work (MPI init, verification) at small sizes; by 124 MB the `MPIIO_F_WRITE_TIME` becomes a meaningful fraction of the total runtime.

## Single Shared File

| Counter                              | SSF s8 (8 KiB/rank) | SSF s64 (64 KiB/rank) | SSF s128 (124 KiB/rank) | Reason                                                                                                               |
| ------------------------------------ | ------------------- | --------------------- | ----------------------- | -------------------------------------------------------------------------------------------------------------------- |
| Data written (total, 4 ranks)        | 32 MiB              | 256 MiB               | 496 MiB                 | Directly scales with per-rank buffer size (-k parameter × 4 ranks)                                                   |
| Run time                             | 0.2539 s            | 2.1404 s              | 4.9765 s                | Grows super-linearly due to increasing lock contention and metadata overhead at larger sizes                         |
| POSIX_OPENS (shared file)            | 4                   | 4                     | 4                       | Each rank opens the single shared file once regardless of data size                                                  |
| POSIX_WRITES (aggregated)            | 4                   | 4                     | 4                       | Each rank performs exactly one write call; count is independent of transfer size                                     |
| POSIX_BYTES_WRITTEN                  | 33,554,432 B        | 268,435,456 B         | 520,093,696 B           | Total bytes at POSIX layer matches MPI-IO layer; no extra buffering                                                  |
| POSIX_MAX_BYTE_WRITTEN               | 41,943,039 B        | 335,544,319 B         | 650,117,119 B           | Highest file offset written; grows as more data is appended to the shared file                                       |
| POSIX_ACCESS1_ACCESS (dominant size) | 8 MiB               | 64 MiB                | ~124 MiB                | Single large contiguous write per rank; access size equals per-rank buffer size                                      |
| POSIX_F_WRITE_TIME (cumulative)      | 0.005937 s          | 0.102469 s            | 1.380161 s              | Serialized writes to shared inode cause cumulative time to scale steeply with data size                              |
| POSIX_F_META_TIME (cumulative)       | 0.000017 s          | 0.000022 s            | 0.000019 s              | Metadata ops (open/close) are near-constant; unaffected by data volume                                               |
| POSIX_F_FASTEST_RANK_TIME            | 0.000620 s          | 0.009645 s            | 0.029662 s              | Fastest rank gets the lock first and finishes quickly; still grows with write size                                   |
| POSIX_F_SLOWEST_RANK_TIME            | 0.002337 s          | 0.040040 s            | 0.885865 s              | Last rank waits for all others to release the lock; wait time explodes at large sizes                                |
| POSIX_F_VARIANCE_RANK_TIME           | 0.000000            | 0.000132              | 0.113929                | Near-zero at small sizes; grows sharply as lock serialization creates large timing spread across ranks               |
| POSIX_F_VARIANCE_RANK_BYTES          | 0.000000            | 0.000000              | 0.000000                | All ranks write equal amounts; byte distribution is perfectly balanced                                               |
| FILE_ALIGNMENT                       | 4096 B              | 4096 B                | 4096 B                  | Filesystem (ext4) block size is fixed; unaffected by transfer size                                                   |
| MPIIO_COLL_OPENS                     | 4                   | 4                     | 4                       | All ranks participate in one collective open; count is always equal to nprocs                                        |
| MPIIO_INDEP_WRITES                   | 4                   | 4                     | 4                       | Each rank issues one independent (non-collective) write; count fixed at nprocs                                       |
| MPIIO_COLL_WRITES                    | 0                   | 0                     | 0                       | No collective write calls used; application uses independent MPI_File_write                                          |
| MPIIO_BYTES_WRITTEN                  | 33,554,432 B        | 268,435,456 B         | 520,093,696 B           | Matches POSIX bytes written exactly; MPI-IO passes data through without extra copies                                 |
| MPIIO_ACCESS1_ACCESS (dominant size) | 8 MiB               | 64 MiB                | ~124 MiB                | Reflects per-rank transfer size; single dominant access pattern across all ranks                                     |
| MPIIO_ACCESS1_COUNT                  | 4                   | 4                     | 4                       | All 4 ranks use the same access size, so the single dominant size has a count of 4                                   |
| MPIIO_MAX_WRITE_TIME_SIZE            | 8,388,608 B         | 67,108,864 B          | 130,023,424 B           | Size of the single slowest write operation; equals per-rank buffer as there is only one write per rank               |
| MPIIO_F_WRITE_TIME (cumulative)      | 0.006017 s          | 0.102647 s            | 1.380310 s              | Closely tracks POSIX write time; overhead of lock acquisition serializes all ranks                                   |
| MPIIO_F_META_TIME (cumulative)       | 0.029640 s          | 0.156375 s            | 2.743926 s              | Grows sharply with data size; shared file collective open/close requires distributed coordination that scales poorly |
| MPIIO_F_MAX_WRITE_TIME               | 0.002354 s          | 0.040081 s            | 0.885896 s              | Duration of the single slowest individual write; dominated by lock wait time at larger sizes                         |
| MPIIO_F_FASTEST_RANK_TIME            | 0.003006 s          | 0.041098 s            | 0.886667 s              | Includes open+write+close for the luckiest rank; grows with data size but stays well below slowest                   |
| MPIIO_F_SLOWEST_RANK_TIME            | 0.015899 s          | 0.104057 s            | 1.154008 s              | Slowest rank accumulates full lock wait plus its own write time; diverges significantly at large sizes               |
| MPIIO_F_VARIANCE_RANK_TIME           | 0.000022            | 0.000610              | 0.009391                | Increases with data size; reflects growing imbalance caused by lock contention among ranks                           |
| MPIIO_F_VARIANCE_RANK_BYTES          | 0.000000            | 0.000000              | 0.000000                | Equal data per rank by design; no byte imbalance regardless of total data size                                       |
Single Shared File – Key Takeaways
- Primary bottleneck is lock contention on the shared inode; writes are effectively serialized, causing runtime to grow super-linearly with data size.
- Even though each rank performs only one large contiguous write, the shared file forces strict ordering, eliminating parallelism at the filesystem level.
- Performance degrades significantly from s8 → s64 → s128 due to increasing wait time for lock acquisition, not due to lack of bandwidth.
- MPI-IO metadata time increases sharply with data size, indicating high coordination overhead during collective open/close operations.
- Large divergence between fastest and slowest ranks shows severe load imbalance caused by lock waiting; later ranks accumulate cascading delays.
- Secondary effect: increasing data size amplifies serialization, which increases variance across ranks and further reduces effective throughput.
- Despite perfect byte balance, time imbalance dominates performance, showing that synchronization—not data distribution—is the limiting factor.
- Overall, scalability is poor because contention and coordination overheads grow faster than useful work, making SSF inefficient at large scales.

## Random

|Counter|Random r8 (8 KiB/rank)|Random r64 (64 KiB/rank)|Random r128 (128 KiB/rank)|Reason|
|---|---|---|---|---|
|Data written (total, 4 ranks)|32 MiB|256 MiB|512 MiB|Scales directly with per-rank buffer size × 4 ranks|
|Run time|0.3252 s|2.7008 s|5.4210 s|Grows super-linearly; more small random writes at larger sizes amplify lock contention and seek overhead|
|POSIX_OPENS (shared file)|4|4|4|Each rank opens the single shared file once; unchanged by data size|
|POSIX_WRITES (aggregated)|32,768|262,144|524,288|Many small 1 KiB writes per rank (buffer_size / 1024); count scales linearly with total data|
|POSIX_BYTES_WRITTEN|33,554,432 B|268,435,456 B|536,870,912 B|Total bytes match MPI-IO layer exactly; no extra buffering|
|POSIX_MAX_BYTE_WRITTEN|33,554,431 B|268,435,455 B|536,870,911 B|Highest offset touched equals total file size minus 1; all of the address space is covered|
|POSIX_ACCESS1_ACCESS (dominant size)|1,024 B (1 KiB)|1,024 B (1 KiB)|1,024 B (1 KiB)|Fixed small random write size of 1 KiB regardless of total buffer; characteristic of random I/O pattern|
|POSIX_ACCESS1_COUNT|32,768|262,144|524,288|Total number of 1 KiB writes; equals POSIX_WRITES and scales with data size|
|POSIX_FILE_NOT_ALIGNED|24,576|196,608|393,216|Random offsets frequently land mid-block; count scales with number of writes as misalignment probability is constant|
|POSIX_SEQ_WRITES|16,276|131,276|261,322|~50% of writes happen to be at increasing offsets by chance; proportion stays roughly constant as total writes scale|
|POSIX_CONSEC_WRITES|12|4|4|Very few writes land at exactly adjacent offsets; near-zero as expected for random access|
|POSIX_MAX_WRITE_TIME_SIZE|1,024 B|1,024 B|1,024 B|Slowest single write is always a 1 KiB op; size is fixed, only contention time varies|
|POSIX_F_WRITE_TIME (cumulative)|0.045667 s|0.470393 s|1.710111 s|Grows steeply; many small random writes serialized through shared inode lock accumulate high latency|
|POSIX_F_META_TIME (cumulative)|0.000034 s|0.000023 s|0.000031 s|Near-constant; only open/close ops contribute and their count is fixed at 4|
|POSIX_F_MAX_WRITE_TIME|0.000648 s|0.010116 s|0.013193 s|Single slowest write grows with contention as more ranks compete for the lock at larger sizes|
|POSIX_F_FASTEST_RANK_TIME|0.011104 s|0.116759 s|0.421990 s|Luckiest rank still bears full write overhead; grows proportionally with data size|
|POSIX_F_SLOWEST_RANK_TIME|0.012078 s|0.117909 s|0.433440 s|Slowest rank very close to fastest; random writes interleave more evenly than sequential, limiting extreme stragglers|
|POSIX_F_VARIANCE_RANK_TIME|0.000000|0.000000|0.000017|Very low variance across all sizes; random offsets distribute lock wait time evenly among ranks|
|POSIX_F_VARIANCE_RANK_BYTES|0.000000|0.000000|0.000000|All ranks write equal amounts; byte balance is perfect by design|
|FILE_ALIGNMENT|4096 B|4096 B|4096 B|ext4 block size is fixed; unaffected by access pattern or data size|
|MPIIO_COLL_OPENS|4|4|4|All ranks participate in one collective open; always equals nprocs|
|MPIIO_INDEP_WRITES|32,768|262,144|524,288|Each rank issues one independent MPI_File_write per 1 KiB chunk; scales with total data|
|MPIIO_COLL_WRITES|0|0|0|No collective writes used; application uses independent MPI_File_write_at|
|MPIIO_BYTES_WRITTEN|33,554,432 B|268,435,456 B|536,870,912 B|Matches POSIX layer exactly; MPI-IO passes each 1 KiB write straight through|
|MPIIO_ACCESS1_ACCESS (dominant size)|1,024 B (1 KiB)|1,024 B (1 KiB)|1,024 B (1 KiB)|Fixed random write granularity visible at MPI-IO layer; confirms no batching or aggregation by ROMIO|
|MPIIO_ACCESS1_COUNT|32,768|262,144|524,288|Matches POSIX access count; every MPI write maps 1:1 to a POSIX write|
|MPIIO_MAX_WRITE_TIME_SIZE|1,024 B|1,024 B|1,024 B|Slowest individual MPI write is always 1 KiB; latency varies but size does not|
|MPIIO_F_WRITE_TIME (cumulative)|0.289097 s|2.460754 s|5.720702 s|Much higher than POSIX write time; includes per-call MPI overhead × tens of thousands of calls, grows steeply|
|MPIIO_F_META_TIME (cumulative)|0.036968 s|0.077049 s|0.134553 s|Grows moderately with data size; collective open/close coordination cost increases with larger shared file|
|MPIIO_F_MAX_WRITE_TIME|0.000657 s|0.010128 s|0.013201 s|Single worst-case MPI write time grows with lock contention at larger data sizes|
|MPIIO_F_FASTEST_RANK_TIME|0.074616 s|0.614694 s|1.439018 s|Includes all MPI overhead for luckiest rank; grows proportionally but faster than POSIX due to per-call MPI cost|
|MPIIO_F_SLOWEST_RANK_TIME|0.090854 s|0.664433 s|1.535825 s|Slowest rank moderately above fastest; random access distributes contention more evenly than sequential|
|MPIIO_F_VARIANCE_RANK_TIME|0.000037|0.000341|0.001729|Grows with data size but stays low; random offsets prevent any one rank from monopolizing the lock|
|MPIIO_F_VARIANCE_RANK_BYTES|0.000000|0.000000|0.000000|Equal bytes per rank by design; no imbalance regardless of access pattern|
  
Random Access – Key Takeaways  
- Primary bottleneck is I/O inefficiency: many small, scattered writes cause high seek cost and poor cache utilization.  
- Performance is worse than sequential because lack of locality prevents aggregation into large contiguous operations.  
- High contention and frequent lock acquisition lead to partial serialization, especially as ranks increase.  
- Load imbalance appears due to irregular access timing, introducing variability in rank completion times.  
- Secondary effect: large number of I/O calls amplifies MPI overhead, causing cumulative slowdown beyond pure disk cost.  
- Cascading impact: contention + imbalance + high call frequency combine to reduce scalability much earlier than in sequential access.

## Sequential

|Counter|Sequential s8 (8 KiB/rank)|Sequential s64 (64 KiB/rank)|Sequential s128 (128 KiB/rank)|Reason|
|---|---|---|---|---|
|Data written (total, 4 ranks)|32 MiB|256 MiB|512 MiB|Scales directly with per-rank buffer size × 4 ranks|
|Run time|0.3139 s|2.7040 s|5.3279 s|Grows roughly linearly; sequential access avoids seek overhead and lock thrashing|
|POSIX_OPENS (shared file)|4|4|4|Each rank opens the shared file once; independent of data size|
|POSIX_WRITES (aggregated)|32,768|262,144|524,288|Same number of 1 KiB writes as Random case; determined by buffer_size / 1024|
|POSIX_BYTES_WRITTEN|33,554,432 B|268,435,456 B|536,870,912 B|Total bytes equal logical dataset size|
|POSIX_MAX_BYTE_WRITTEN|33,554,431 B|268,435,455 B|536,870,911 B|Final offset equals total file size minus 1|
|POSIX_ACCESS1_ACCESS (dominant size)|1,024 B (1 KiB)|1,024 B (1 KiB)|1,024 B (1 KiB)|Application writes fixed 1 KiB chunks|
|POSIX_ACCESS1_COUNT|32,768|262,144|524,288|One POSIX write per MPI write call|
|POSIX_FILE_NOT_ALIGNED|24,576|196,608|393,216|1 KiB writes often fall between 4 KiB ext4 blocks; proportion constant|
|POSIX_SEQ_WRITES|32,764|262,140|524,284|Nearly all writes follow increasing offsets; characteristic of sequential pattern|
|POSIX_CONSEC_WRITES|32,760|262,136|524,280|Majority of writes occur immediately after previous offset|
|POSIX_MAX_WRITE_TIME_SIZE|1,024 B|1,024 B|1,024 B|Individual operation size unchanged|
|POSIX_F_WRITE_TIME (cumulative)|0.041 s|0.463 s|1.642 s|Grows proportionally with total data written|
|POSIX_F_META_TIME (cumulative)|0.000047 s|0.000008 s|0.000040 s|Metadata cost nearly constant; only open/close involved|
|POSIX_F_MAX_WRITE_TIME|0.00052 s|0.00890 s|0.01210 s|Slowest single write increases slightly with contention|
|POSIX_F_FASTEST_RANK_TIME|0.0109 s|0.112 s|0.408 s|Sequential layout distributes work evenly across ranks|
|POSIX_F_SLOWEST_RANK_TIME|0.0117 s|0.115 s|0.420 s|Minimal skew; no rank waits excessively|
|POSIX_F_VARIANCE_RANK_TIME|0.000000|0.000000|0.000012|Sequential pattern keeps rank timing very consistent|
|POSIX_F_VARIANCE_RANK_BYTES|0.000000|0.000000|0.000000|Equal bytes written per rank|
|FILE_ALIGNMENT|4096 B|4096 B|4096 B|ext4 filesystem block size constant|
|MPIIO_COLL_OPENS|4|4|4|All ranks participate in single collective open|
|MPIIO_INDEP_WRITES|32,768|262,144|524,288|One MPI_File_write per chunk|
|MPIIO_COLL_WRITES|0|0|0|Application uses independent writes|
|MPIIO_BYTES_WRITTEN|33,554,432 B|268,435,456 B|536,870,912 B|Matches POSIX layer exactly|
|MPIIO_ACCESS1_ACCESS (dominant size)|1,024 B|1,024 B|1,024 B|MPI layer preserves application granularity|
|MPIIO_ACCESS1_COUNT|32,768|262,144|524,288|One MPI write per POSIX write|
|MPIIO_MAX_WRITE_TIME_SIZE|1,024 B|1,024 B|1,024 B|Largest single MPI write remains fixed|
|MPIIO_F_WRITE_TIME (cumulative)|0.273 s|2.418 s|5.611 s|MPI overhead accumulates over many calls|
|MPIIO_F_META_TIME (cumulative)|0.034 s|0.075 s|0.132 s|Collective open cost increases slightly with file size|
|MPIIO_F_MAX_WRITE_TIME|0.00054 s|0.00901 s|0.01234 s|Larger file → slightly higher worst-case latency|
|MPIIO_F_FASTEST_RANK_TIME|0.071 s|0.598 s|1.401 s|Includes MPI overhead for best-performing rank|
|MPIIO_F_SLOWEST_RANK_TIME|0.084 s|0.642 s|1.502 s|Sequential access minimizes rank imbalance|
|MPIIO_F_VARIANCE_RANK_TIME|0.000032|0.000298|0.001612|Small variance; balanced workload|
|MPIIO_F_VARIANCE_RANK_BYTES|0.000000|0.000000|0.000000|Perfect data balance|

Key takeaways

1. Sequential access shows near-linear runtime scaling as data size increases, unlike random access which suffers additional seek overhead.
2. Almost all writes are sequential and consecutive, confirming the expected behaviour of a sequential workload.
3. The number of writes is identical to the random case because the application still performs fixed 1 KiB operations.
4. Rank-level timing variance remains extremely low, meaning all processes progress at almost the same speed.
5. MPI-IO cumulative write time is significantly larger than POSIX write time because each small write includes MPI overhead.
6. File alignment remains constant at 4096 B, matching the ext4 filesystem block size.
7. Sequential workloads generally achieve better performance stability due to predictable offset progression and reduced lock contention.

# Q5

## sequential

| Metric         | se2 (2 ranks) | se6 (6 ranks) | se10 (10 ranks) | Trend / Observation                                |
| -------------- | ------------- | ------------- | --------------- | -------------------------------------------------- |
| Runtime        | 0.3005 s      | 0.3448 s      | 0.6761 s        | Increases with ranks (contention + overhead grows) |
| Scaling        | Good          | Moderate      | Poor            | Diminishing returns after moderate parallelism     |
| POSIX Opens    | 2             | 6             | 10              | Linear increase (FPP behavior)                     |
| Access Pattern | Sequential    | Sequential    | Sequential      | Highly cache-friendly                              |
| Contention     | Low           | Low           | Moderate        | Still manageable due to sequential writes          |
| Load Balance   | High          | High          | High            | Uniform work distribution                          |
| Efficiency     | High          | Medium        | Lower           | Overhead dominates at higher ranks                 |
Sequential Access – Key Takeaways  
- Bottleneck shifts from I/O to overhead as ranks increase; metadata operations and process coordination dominate at higher ranks (6 → 10).  
- Good initial scaling is due to high spatial locality, which enables effective caching and minimizes disk seek overhead.  
- Low contention because accesses are well-ordered; no significant locking or serialization at the filesystem level.  
- Performance degradation at higher ranks is mainly due to diminishing parallel efficiency rather than disk limitations.  
- Uniform workload distribution avoids stragglers, so no imbalance-related delays. 
- Secondary effect: increasing number of files (FPP) raises metadata pressure, which contributes to runtime growth.

## random

| Metric            | r2 (2 ranks) | r6 (6 ranks) | r10 (10 ranks) | Trend / Observation                                  |
|------------------|-------------|-------------|---------------|------------------------------------------------------|
| Runtime          | 0.3068 s    | 0.3497 s    | 0.6701 s      | Similar growth but slightly worse than sequential    |
| Scaling          | Moderate    | Poor        | Poor          | Random access hurts scalability early               |
| POSIX Opens      | 2           | 6           | 10            | Linear increase (same as sequential)                |
| Access Pattern   | Random      | Random      | Random        | Cache-unfriendly, scattered writes                  |
| Contention       | Moderate    | High        | High          | Increased disk + lock contention                    |
| Load Balance     | Medium      | Lower       | Lower         | Irregular access → imbalance                        |
| Efficiency       | Medium      | Low         | Low           | Poor locality dominates performance                 |
  
Random Access – Key Takeaways 
- Primary bottleneck is I/O inefficiency caused by poor locality, leading to frequent disk seeks and cache misses. 
- Scalability degrades earlier than sequential due to increased contention and inefficient access patterns. 
- Higher ranks amplify filesystem contention and locking overhead, causing serialization effects. 
- Load imbalance emerges as some ranks experience more delays due to irregular access patterns. 
- Efficiency drops due to both MPI overhead and underlying storage inefficiencies. 
- Secondary effect: contention and imbalance cascade into higher variance across ranks, further reducing overall throughput.

## file per process

|Counter|FPP r2 (2 ranks)|FPP r6 (6 ranks)|FPP r10 (10 ranks)|Reason|
|---|---|---|---|---|
|Data written (per rank)|64 MiB|64 MiB|64 MiB|Each rank writes the same buffer size independently|
|Run time (total job)|1.8674 s|2.1521 s|4.3985 s|Runtime increases as more ranks access filesystem simultaneously|
|Files created per rank|1|1|1|Each rank creates exactly one file|
|POSIX_OPENS (per rank)|1|1|1|Each rank independently opens its own file|
|POSIX_WRITES (per rank)|65,536|65,536|65,536|Fixed 1 KiB write size → constant number of writes per rank|
|POSIX_BYTES_WRITTEN (per rank)|67,108,864 B|67,108,864 B|67,108,864 B|Each rank writes identical data volume|
|POSIX_MAX_BYTE_WRITTEN|67,108,863 B|67,108,863 B|67,108,863 B|Final offset equals file size minus 1|
|POSIX_ACCESS1_ACCESS|1,024 B|1,024 B|1,024 B|Application performs fixed 1 KiB writes|
|POSIX_ACCESS1_COUNT|65,536|65,536|65,536|Same number of writes per rank|
|POSIX_FILE_NOT_ALIGNED|49,152|49,152|49,152|1 KiB writes often not aligned to 4 KiB blocks|
|POSIX_SEQ_WRITES|65,530|65,530|65,530|Writes occur sequentially within each rank's file|
|POSIX_CONSEC_WRITES|65,520|65,520|65,520|Majority of writes are adjacent|
|POSIX_F_WRITE_TIME (per rank)|0.305 s|0.336 s|0.438 s|Write time per rank increases slightly with system load|
|POSIX_F_META_TIME|0.000015 s|0.000017 s|0.000019 s|Metadata cost small but grows slightly with number of files|
|POSIX_F_MAX_WRITE_TIME|0.011 s|0.013 s|0.018 s|Worst individual write slightly slower at higher concurrency|
|POSIX_F_FASTEST_RANK_TIME|0.28 s|0.31 s|0.36 s|Fastest rank runtime slightly increases with contention|
|POSIX_F_SLOWEST_RANK_TIME|0.31 s|0.36 s|0.44 s|Slowest rank affected by filesystem contention|
|POSIX_F_VARIANCE_RANK_TIME|0.00011|0.00016|0.00029|Small increase in variability across ranks|
|POSIX_F_VARIANCE_RANK_BYTES|0.000000|0.000000|0.000000|Each rank writes equal data|
|FILE_ALIGNMENT|4096 B|4096 B|4096 B|ext4 block size constant|
|MPIIO_INDEP_OPENS|1|1|1|Each rank independently opens its file|
|MPIIO_INDEP_WRITES|65,536|65,536|65,536|Same number of MPI writes per rank|
|MPIIO_COLL_WRITES|0|0|0|No collective operations used|
|MPIIO_BYTES_WRITTEN|67,108,864 B|67,108,864 B|67,108,864 B|MPI layer writes same amount per rank|
|MPIIO_ACCESS1_ACCESS|1,024 B|1,024 B|1,024 B|Same granularity as POSIX layer|
|MPIIO_ACCESS1_COUNT|65,536|65,536|65,536|One MPI write per POSIX write|
|MPIIO_F_WRITE_TIME (per rank)|0.86 s|0.93 s|1.18 s|MPI overhead increases slightly with higher concurrency|
|MPIIO_F_META_TIME|0.010 s|0.013 s|0.019 s|Metadata overhead increases with number of files|
|MPIIO_F_FASTEST_RANK_TIME|0.52 s|0.61 s|0.78 s|Includes MPI overhead per rank|
|MPIIO_F_SLOWEST_RANK_TIME|0.61 s|0.72 s|0.95 s|Some ranks affected more by contention|
|MPIIO_F_VARIANCE_RANK_TIME|0.00038|0.00055|0.00088|Slight increase in timing spread|
|MPIIO_F_VARIANCE_RANK_BYTES|0.000000|0.000000|0.000000|Equal data per rank|

Key takeaways
- Main bottleneck: storage bandwidth saturation rather than locking. Since each rank writes to its own file, there is no shared inode contention; performance degradation at 10 ranks mainly occurs due to more concurrent writes competing for disk throughput.
- Performance difference vs SSF: FPP scales better than single shared file because independent files eliminate serialization caused by file locks, allowing ranks to perform parallel writes more efficiently.
- Cascading effect 1 – increased metadata operations: more ranks create more files, increasing number of POSIX_OPENS and slightly increasing metadata overhead, but this remains very small compared to write cost.
- Cascading effect 2 – bandwidth contention: as ranks increase from 2 → 10, more processes issue writes simultaneously, increasing pressure on filesystem bandwidth and increasing MPIIO_F_WRITE_TIME.
- Cascading effect 3 – stable sequential locality: each rank writes sequentially to its own file, resulting in high POSIX_SEQ_WRITES and POSIX_CONSEC_WRITES, allowing filesystem optimizations like write buffering and reduced seek overhead.
- Scaling behaviour: per-core workload remains constant (same bytes and write count per rank), while total runtime increases moderately due to shared hardware resource contention rather than synchronization overhead.
- Low rank imbalance: very small MPIIO_F_VARIANCE_RANK_TIME indicates that workload is evenly distributed and no rank becomes a major straggler.
- Overall conclusion: FPP demonstrates better scalability with increasing cores because it avoids shared-file locking, and performance degradation is mainly limited by physical I/O bandwidth rather than synchronization costs.

## Single Shared File

|Counter|SSF r2 (2 ranks)|SSF r6 (6 ranks)|SSF r10 (10 ranks)|Reason|
|---|---|---|---|---|
|Data written (per rank)|64 MiB|64 MiB|64 MiB|Each rank writes same amount of data into a shared file|
|Total data written|128 MiB|384 MiB|640 MiB|Total file size increases with number of ranks|
|Run time|1.9514 s|2.2184 s|5.7332 s|Runtime increases non-linearly due to increasing lock contention|
|POSIX_OPENS|2|6|10|All ranks open same shared file|
|POSIX_WRITES|131,072|393,216|655,360|Each rank performs fixed number of 1 KiB writes|
|POSIX_BYTES_WRITTEN|134,217,728 B|402,653,184 B|671,088,640 B|Total bytes scale with number of ranks|
|POSIX_MAX_BYTE_WRITTEN|134,217,727 B|402,653,183 B|671,088,639 B|Final offset equals total file size minus 1|
|POSIX_ACCESS1_ACCESS|1,024 B|1,024 B|1,024 B|Write granularity fixed at 1 KiB|
|POSIX_ACCESS1_COUNT|131,072|393,216|655,360|Total number of writes scales with ranks|
|POSIX_FILE_NOT_ALIGNED|98,304|294,912|491,520|Many writes not aligned to 4 KiB filesystem blocks|
|POSIX_SEQ_WRITES|65,540|196,620|327,700|Sequential pattern per rank but interleaving reduces global sequentiality|
|POSIX_CONSEC_WRITES|8|6|4|Very few writes are consecutive due to interleaving among ranks|
|POSIX_F_WRITE_TIME|0.64 s|1.02 s|3.41 s|Shared-file locking overhead increases rapidly with more ranks|
|POSIX_F_META_TIME|0.000051 s|0.000031 s|0.000029 s|Metadata cost small relative to write cost|
|POSIX_F_MAX_WRITE_TIME|0.012 s|0.018 s|0.041 s|Worst-case write slower due to lock contention|
|POSIX_F_FASTEST_RANK_TIME|0.41 s|0.53 s|0.88 s|Fastest rank still impacted by shared lock delays|
|POSIX_F_SLOWEST_RANK_TIME|0.46 s|0.62 s|1.11 s|Some ranks wait longer for file lock access|
|POSIX_F_VARIANCE_RANK_TIME|0.00021|0.00049|0.00210|Rank timing variation increases with contention|
|POSIX_F_VARIANCE_RANK_BYTES|0.000000|0.000000|0.000000|Equal data written by each rank|
|FILE_ALIGNMENT|4096 B|4096 B|4096 B|ext4 block size constant|
|MPIIO_COLL_OPENS|2|6|10|All ranks collectively open shared file|
|MPIIO_INDEP_WRITES|131,072|393,216|655,360|Writes performed independently|
|MPIIO_COLL_WRITES|0|0|0|No collective write aggregation used|
|MPIIO_BYTES_WRITTEN|134,217,728 B|402,653,184 B|671,088,640 B|Matches POSIX layer|
|MPIIO_ACCESS1_ACCESS|1,024 B|1,024 B|1,024 B|MPI layer preserves application write size|
|MPIIO_ACCESS1_COUNT|131,072|393,216|655,360|Same number of MPI writes as POSIX writes|
|MPIIO_F_WRITE_TIME|1.83 s|2.11 s|5.52 s|MPI overhead amplified by lock contention|
|MPIIO_F_META_TIME|0.041 s|0.066 s|0.121 s|Collective open coordination cost increases slightly|
|MPIIO_F_FASTEST_RANK_TIME|0.74 s|0.88 s|1.64 s|Best rank still affected by synchronization overhead|
|MPIIO_F_SLOWEST_RANK_TIME|0.86 s|1.02 s|1.98 s|Slowest rank experiences highest contention delay|
|MPIIO_F_VARIANCE_RANK_TIME|0.00044|0.00088|0.00302|Rank imbalance increases with more processes|
|MPIIO_F_VARIANCE_RANK_BYTES|0.000000|0.000000|0.000000|Equal data per rank|

Key takeaways

1. Main bottleneck: shared-file locking. As ranks increase from 2 → 10, more processes compete for the same inode lock, causing serialization of writes and sharp runtime growth.
2. Performance difference vs FPP: single shared file scales worse than file-per-process because all ranks target one file, creating contention, while FPP distributes writes across multiple files and avoids lock conflicts.
3. Cascading effect 1 – increased MPI overhead: higher contention increases waiting time per MPI_File_write call, which increases cumulative MPIIO_F_WRITE_TIME disproportionately compared to data growth.
4. Cascading effect 2 – reduced sequential locality: although each rank writes sequentially, interleaving across ranks reduces global consecutiveness (low POSIX_CONSEC_WRITES), weakening filesystem prefetch and write-combining efficiency.
5. Cascading effect 3 – queueing delays amplify variance: more ranks cause longer wait queues for file locks, increasing MPIIO_F_VARIANCE_RANK_TIME and POSIX_F_SLOWEST_RANK_TIME.
6. Scaling behaviour: total data increases linearly with ranks, but runtime increases super-linearly at 10 ranks, confirming lock contention as dominant bottleneck.
7. Metadata is not a bottleneck: POSIX_F_META_TIME remains negligible compared to write time, indicating performance degradation is dominated by data access synchronization.
8. Overall conclusion: SSF shows good scalability at low core counts but degrades at higher concurrency due to shared-file synchronization costs, while FPP provides better scalability by eliminating shared locking.

# Q6
1. Well Pinned: Each rank gets a dedicated core (minimal context switching)
2. Poorly Pinned: Multiple ranks compete for cores (heavy context switching)

Well Pinned
```
export MV2_ENABLE_AFFINITY=1
export MV2_CPU_BINDING_POLICY=bind_to_core
export MV2_CPU_BINDING_LEVEL=core
mpirun -np 4 ./a.out
```

Poorly Pinned
- disable binding
```
export MV2_ENABLE_AFFINITY=0
mpirun -np 4 ./a.out
```
- over subscribing
```
mpirun -np 8 ./a.out   # on 4 cores
```


**WELL PINNED**
FPP: total_POSIX_WRITES: 8
SSF: total_POSIX_WRITES: 5
Because SSF does meta data write only once, but FPP has to do it 4 times....Both do the write 4 times for each process.

FPP: total_POSIX_BYTES_WRITTEN: 268435616
SSF: total_POSIX_BYTES_WRITTEN: 268435496
FPP writes a bit more meta data

|Index|SSF Value|FPP Value|Reason|
|---|---|---|---|
|POSIX Writes|5|8|Same access pattern (design-level)|
|Write Time|0.043496|0.076208|Better locality + sync across ranks|
|MPI-IO Opens|4 collective|4 independent|Same behavior|
|MPI-IO Metadata Time|0.134146|0.002136|Less coordination overhead than poor bind|
|Slowest Rank Time|0.092382|0.000000|Reduced imbalance|

**POORLY PINNED**

FPP:
total_POSIX_F_WRITE_TIME: 0.113667
total_POSIX_F_META_TIME: 0.000182

SSF:
total_POSIX_F_WRITE_TIME: 0.074452
total_POSIX_F_META_TIME: 0.000241

| Index                | SSF Value    | FPP Value     | Reason                                          |
| -------------------- | ------------ | ------------- | ----------------------------------------------- |
| POSIX Writes         | 5            | 8             | SSF aggregates I/O; FPP does independent writes |
| Write Time           | 0.053097     | 0.088762      | Fragmented writes in FPP increase cost          |
| MPI-IO Opens         | 4 collective | 4 independent | Collective merges requests                      |
| MPI-IO Metadata Time | 0.294467     | 0.002470      | Coordination overhead in SSF                    |
| Slowest Rank Time    | 0.138288     | 0.000000      | Poor binding causes imbalance                   |

|Index|Well Binded|Poorly Binded|Reason|
|---|---|---|---|
|SSF Write Time|0.043496|0.053097|Better cache locality + aligned I/O phases|
|FPP Write Time|0.076208|0.088762|Reduced contention and context switching|
|SSF Metadata Time|0.134146|0.294467|Less synchronization delay|
|SSF Slowest Rank|0.092382|0.138288|Better load balance across cores|
|Runtime (SSF)|2.0517|2.1889|Improved overall efficiency due to pinning|
