
- 1.1.1.1 DNS cache (Big Pineapple) stores over 250 billion entries.
- Every wasted byte per entry = 250GB across fleet, so they went hunting for waste
- 5 changes, all in Rust memory layout, cut per-entry size by 56%

1. Both Vec and String have a capacity field which is unnecessary since the entries are write-once and are never modified after being inserted. We therefore replaced `Vec<T>` with `Box<[T]>` and String with `Box<str>`, got rid of the capacity field and stopped over-allocating heap space as a result, saving about 15TB solely from this change.

2. Previously the answer/authority/additional data was stored as three separate lists (three pointers plus three lengths); I have now changed it to a single combined list using u16 offsets to indicate where each section begins. This change saves about 28 bytes per entry.

3. The owner of DNS records (that is, the domain to which they belong) is generally the same as the domain that was queried, so rather than recording it each time, the owner is only stored when it is different (for example, in the case of CNAME chains) and is otherwise inferred from the cache key.

4.  They stored each type of DNS record (such as A, AAAA, TXT, NAPTR, etc.) as an enum variant. Since Rust makes the size of the enum equal to that of its largest variant—and NAPTR was 144 bytes—an A record (which only needs 4 bytes) was wasting over 120 bytes. The solution was to put the large and infrequent variants (NAPTR, SVCB, TXT) inside a box so that only a pointer remains in the inline section, while the smaller and more frequently used ones (A, AAAA) stay inline and unboxed.

5. Even when boxing individually, there are still scattered heap allocations (which results in poor cache locality) together with waste caused by the allocator's rounding. That is why they took further action: they now store the raw wire-format bytes (prefixed with a length) in a single `Box<[u8]>` rather than storing the parsed enum records. Only the CNAME/NS/MX/SOA (those that need name compression) records are parsed; the A/AAAA/TXT/DNSSEC records are simply memcopied into the responses.

The results show a per-entry footprint falling from 953B to 420B (a reduction of 56%), with the number of allocations decreasing by 58%, insert throughput increasing by 43%, and lookup latency dropping by 19%. In production, the p99 memory usage per instance decreased from 9.3GB to 5.3GB. A total of about 100TB of memory has been freed across the fleet.

It was released in stages from 18 May to 6 July 2026, each release including one optimization.
 

https://blog.cloudflare.com/dns-cache-memory-optimization-1111/
