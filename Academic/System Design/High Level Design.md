
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

# Source
1. [System Design for Beginners (2026) By KodeKloud](https://www.youtube.com/watch?v=SE2KF-vxvS0)
2. 