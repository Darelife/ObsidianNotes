I've skipped some stuff that's very commonly known 
# Problems
1. Out of Memory / CPU : More servers & load balancing
2. DB Overloads : Caching
3. Machine Dies (single point of failure for DB) : 
   - Replica of DB
   - Photos to S3, and store links in DB

# Functional & Non Functional Requirements

- Functional: All the features in the app (likes, comments, etc)
- None Functional: Speed, Reliability, etc (caching, queue, load balancing, replicas, etc)

Non functional requirements always fight with each other, and require a lot of money.

1. How many users? and how fast is it growing?
2. Is it read heavy or write heavy?
3. What data can you never lose? and what can you afford to lose? (likes being wrong by 2-3 doesn't matter, but pics being lost matters)
4. How much latency can you afford? (upload can be slow, but read must be fast)
5. What does it cost? (Should solve the first 4 qs, with the least amount of money)

# Monolith vs Micro services

1. **Monolith**
	1. One single program, where everything is connected.
	2. Any change, will require for the entire program to be rebuilt again.
	3. All services can call each other internally
	4. Less complex
2. Micro services
	1. Split into many small programs
	2. Each service is deployed separately
	3. Talk to each other over networks

For a few thousand users, monolith is the best. A small team can move fast in monolith

## Reasons for Micro Service Architecture

1. Independent Scaling : Suppose we need 10 servers for read & 2 for write
2. Team Size : Each team can work on its own thing, and all permissions, and it'll reduce hastle.

## Problems with Micro Services

1. They make network calls, which can be slow, timeout, or fail.

## What Most Projects Do

Start with a monolith, and split them out (satellites), only when required.

# Horizontal & Vertical Scaling

1. Vertical Scaling
	1. Add more CPU, RAM, and faster disk
	2. Initially it's better
	3. Once you've purchased the biggest machine, u can't scale anymore
	4. Price initially is cheap, but then, increases like crazy
	5. If it goes down, everything else goes down too.
2. Horizontal Scaling
	1. Add a lot of small machines.
	2. No upper limit
	3. No single point of failure
	4. Which server will handle the request though? (load balancing, but all the servers can't keep their own copy of the data....)

# Load Balancing

- All reqs go through the LB, and it directs the user to the server.
- It keeps running health checks for all the servers (every few seconds).

1. Round Robbin
2. Least Connections
3. Weighted Distribution

- Nginx
- HAProxy
- AWS Elastic LB

> [!question] What if the LB itself goes down?
> There isn't just 1 load balancer. There are multiple LBs.

# Stateless vs Stateful

- If stateful, use sticky sessions, or just switch to stateless connections.
- Login sessions can be stored in a shared store, instead of local storage, in each individual servers (if stateless).

# NoSQL vs SQL

1. Questions about the access type
2. Data Shape
3. Technology (nosql or sql)

Is the data structured (SQL: mysql, postgresql, sql server, oracle, etc) or un-structured (NoSQL: mongo, cassandra, dynamo, elastic)

user preferences, analytics: nosql
photos, and users: sql

## SQL

- Ensures ACID
- Slows with more users
	- Start indexing
	- More indexing is bad though
		- more complex
		- every new write requires extra work to keep the indices updated
		- takes up more space
		- for photos, index by posted_by, and upload_time
		- identify slow queries, and index the relevant columns

# Cache

- A small, fast data store, that's just before the db.
- Eg: redis.
- Small, read constantly, and rarely changes

Problem: 2 copies (keep cache, and db in sync)
- eg: if the user deletes from db, it'll also need to be deleted from the cache, otherwise, it'll be inconsistent.
	- Option 1: TTL (time to live, expiry time) (lazy fix, because it waits for the data to expire). Also, when the TTL expires, and it's hot, there will suddenly be a lot of cache misses (cache stampede). Ensure that the TTLs for different objects are different.
	- Option 2: Active Invalidation. When the DB is updated, the copy from the cache is deleted. More work though.

> There are only two things in CS, and one of them is cache invalidation. 

If a few seconds of stale is fine, go with TTL. For privacy settings, cache invalidation is better.
Home feed: 30 seconds
Profile card: a few mins
Trending list: 60 seconds
Privacy / permissions: no TTL (invalidate)

- lru
- cache stampede: 1 req to rebuild the entry, while the others wait, or refresh before the expiry, and also, add randomness to the TTLs, to ensure that a lot of them don't expire at the same time (jitter)

# Replication

DB is a single point of failure.
1 primary, and 2 replicas, is the industry standard, so 3 dbs. (just like how in GFS, it's stored in 3 places.)

add a load balancer.

read data can be easily distributed, but what about write reqs?

primary will only write.  All replicas have an open connection with the primary, and the replicas will always stay in sync with the primary.

There'll be a **replication lag**. This is, **Eventual Consistency**.
 If the user just updated his/her own data, there might be a lag if read from another db. We can solve this, by either maintaining a cache at the user level, which isn't really preferred, or routing reqs where the user reads his/her own data, to the primary db, as we the user knows that the data has been updated, and we can't show anything wrong to him/her. Others might face replication lag though.

For **Strong Consistency**, read from the primary db. Here, only the main user who updated it, needs it, for that data specifically. We have to decide which part of the data requires which consistency.

If the primary DB dies, promote a replica to be the primary: **Fail over**
Replication is NOT a backup though. If you we do `DROP TABLE photos;`, the primary db will get empty, and within a few miliseconds, the replicas will also be wiped out.

Hence, we also, need to store backups regularly.

# Sharding

Split the data across many dbs.
no single db stores everything, but together, they store everything.

`shard = id % number of shards`

problems
1. hot shard: change the hash policy
2. if we add more shards, the mod hash shard key will fail: do consistent hashing. Each shard has a small range of user ids. A new shard, will just take a small portion from another shard, moving a small amount of data. 

do sharding as late as possible, and also mongo has it by default.

---

1. CAP
2. ACID Implementation

--- 

# Source
1. [System Design for Beginners (2026) By KodeKloud](https://www.youtube.com/watch?v=SE2KF-vxvS0)
2. [System Design Primer](https://github.com/donnemartin/system-design-primer)
3. 