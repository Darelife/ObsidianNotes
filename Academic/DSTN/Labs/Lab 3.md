# lscpu
❯ lscpu
Architecture:                x86_64
  CPU op-mode(s):            32-bit, 64-bit
  Address sizes:             48 bits physical, 48 bits virtual
  Byte Order:                Little Endian
CPU(s):                      12
  On-line CPU(s) list:       0-11
Vendor ID:                   AuthenticAMD
  Model name:                AMD Ryzen 5 5600H with Radeon Graphics
    CPU family:              25
    Model:                   80
    Thread(s) per core:      2
    Core(s) per socket:      6
    Socket(s):               1
    Stepping:                0
    Microcode version:       0xa500012
    Frequency boost:         enabled
    CPU(s) scaling MHz:      49%
    CPU max MHz:             4280.9849
    CPU min MHz:             412.6250

# free -h
❯ free -h
               total        used        free      shared  buff/cache   available
Mem:            15Gi        13Gi       630Mi        33Mi       1.4Gi       1.7Gi
Swap:           11Gi       6.2Gi       5.3Gi

# lsblk
❯ lsblk
NAME        MAJ:MIN RM   SIZE RO TYPE MOUNTPOINTS
zram0       253:0    0   7.5G  0 disk [SWAP]
nvme0n1     259:0    0 476.9G  0 disk 
├─nvme0n1p1 259:1    0   260M  0 part /boot/efi
├─nvme0n1p2 259:2    0    16M  0 part 
├─nvme0n1p3 259:3    0 357.5G  0 part 
├─nvme0n1p4 259:4    0   914M  0 part 
├─nvme0n1p5 259:5    0   118G  0 part /
└─nvme0n1p6 259:6    0   260M  0 part 

# Q1



| FPP                                                                                                                                              | SSF                                                                                                          |
| ------------------------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------ |
| Independent Output file for each process which avoids file level contention, but increases meta data                                             | Writes to a single shared files. Simplifies data management but can lead to contention due to locking        |
| Has a lower runtime (2.0052 seconds)                                                                                                             | Has a higher run time (2.1581) due to the time it spends on locking.                                         |
| POSIX_OPENS: 1 open per file per rank<br>Massive meta data load                                                                                  | POSIX_OPENS: 1 open for the shared file per rank (we have 4 processes)                                       |
| POSIX_F_META_TIME: Higher in FPP because the file system must create and track many separate FDs and inodes (0.000006 per file..we have 4 files) | POSIX_F_META_TIME: 0.000016 (FPP is 2.66 times its value)                                                    |
| POSIX_FILE_ALIGNMENT: 4096                                                                                                                       | POSIX_FILE_ALIGNMENT: 4096<br>(the file system doesn't need to do any extra work to handle unaligned writes) |
| POSIX_F_VARIANCE_RANK_TIME:0 for this                                                                                                            | POSIX_F_VARIANCE_RANK_TIME:0.000034 as it depends on the lock acquire and release time.                      |

- `./fileperprocess.sh -c 1 -r 4 && ./singleshared.sh -c 1 -r 4`
- FPP: 5854.5 MB/s
- SSF: 5366.9 MB/s
- FPP: MPIIO_F_VARIANCE_RANK_TIME  0.000000
- SSF: MPIIO_F_VARIANCE_RANK_TIME  0.001362
- Important topics: POSIX* OPEN, READ, WRITE, BYTE WRITES, BYTE READS, 

## 📦 POSIX Metrics

| Index                   | Description                                  | Expected Trend (FPP vs SSF)                                      | FPP     | SSF     |
| ----------------------- | -------------------------------------------- | ---------------------------------------------------------------- | ------- | ------- |
| **POSIX_OPENS**         | Number of file open operations               | **FPP > SSF** (each rank opens its own file → more metadata ops) | 16      | 16      |
| **POSIX_WRITES**        | Total number of write calls                  | ~Similar (same workload, different contention behavior)          | 1       | 4       |
| **POSIX_BYTES_WRITTEN** | Total bytes written                          | Same (data volume unchanged)                                     | k       | 4k      |
| **POSIX_SEQ_WRITES**    | Sequential write operations                  | **FPP > SSF** (no interleaving across ranks)                     | 1       | 4       |
| **POSIX_CONSEC_WRITES** | Consecutive write operations                 | **FPP > SSF** (less fragmentation)                               | 0       | 0       |
| **POSIX_F_WRITE_TIME**  | Time spent in write operations               | **SSF ≥ FPP** (shared file contention, locking)                  | 0.026   | 0.157   |
| **POSIX_F_META_TIME**   | Time spent in metadata ops (open/close/stat) | **FPP ≥ SSF** (many files → metadata overhead)                   | 22*1e-6 | 77*1e-6 |

---

## 📦 MPI-IO Metrics

| Index                   | Description                           | Expected Trend (FPP vs SSF)                                | FFP | SSF |
| ----------------------- | ------------------------------------- | ---------------------------------------------------------- | --- | --- |
| **MPIIO_INDEP_READS**   | Independent (non-collective) read ops | **SSF ≥ FPP** (less coordination, more fragmented access)  |     |     |
| **MPIIO_INDEP_WRITES**  | Independent write ops                 | **SSF ≥ FPP** (contention due to independent writes)       |     |     |
| **MPIIO_COLL_READS**    | Collective read ops                   | **SSF ≥ FPP** (shared file benefits more from aggregation) |     |     |
| **MPIIO_COLL_WRITES**   | Collective write ops                  | **SSF ≥ FPP** (collective buffering improves SSF)          |     |     |
| **MPIIO_BYTES_READ**    | Bytes read via MPI-IO                 | Same (logical data unchanged)                              |     |     |
| **MPIIO_BYTES_WRITTEN** | Bytes written via MPI-IO              | Same                                                       |     |     |

---

## ⏱️ Timing & Bottleneck Indicators

| Index                          | Description                                | Expected Trend (FPP vs SSF)                                  | FFP | SSF |
| ------------------------------ | ------------------------------------------ | ------------------------------------------------------------ | --- | --- |
| **Total I/O Time**             | POSIX_F_READ_TIME + WRITE_TIME + META_TIME | **SSF ≥ FPP** (contention dominates despite fewer opens)     |     |     |
| **MPIIO_F_VARIANCE_RANK_TIME** | Variance of I/O time across ranks          | **SSF > FPP** (load imbalance due to shared file contention) |     |     |
| **I/O vs Compute Ratio**       | Fraction of time spent in I/O              | **SSF ≥ FPP** (more I/O overhead → more I/O-bound)           |     |     |

---

## 📊 Fillable Table (for submission)

|Index|FPP|SSF|
|---|---|---|
|POSIX_OPENS|||
|POSIX_READS|||
|POSIX_WRITES|||
|POSIX_BYTES_READ|||
|POSIX_BYTES_WRITTEN|||
|POSIX_SEQ_READS|||
|POSIX_SEQ_WRITES|||
|POSIX_CONSEC_READS|||
|POSIX_CONSEC_WRITES|||
|POSIX_F_READ_TIME|||
|POSIX_F_WRITE_TIME|||
|POSIX_F_META_TIME|||
|MPIIO_INDEP_READS|||
|MPIIO_INDEP_WRITES|||
|MPIIO_COLL_READS|||
|MPIIO_COLL_WRITES|||
|MPIIO_BYTES_READ|||
|MPIIO_BYTES_WRITTEN|||
|MPIIO_F_VARIANCE_RANK_TIME|||


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
[[Group 13]]