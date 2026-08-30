# Lecture 2: Architecture of DBMS

---

## 1. Three-Schema Architecture (ANSI-SPARC Architecture)

The **ANSI-SPARC Three-Schema Architecture** defines a structural framework for database systems designed to separate user applications from the physical database storage layer. 

```
                                    +-----------------------+
                                    |    End Users / Apps   |
                                    +-----------------------+
                                                |
  +---------------------------------------------+---------------------------------------------+
  | EXTERNAL LEVEL                              |                                             |
  |             +-----------------------+       |       +-----------------------+             |
  |             |     External View 1   |       |       |     External View 2   |             |
  |             |  (e.g., Student View) |       |       |   (e.g., Finance View)|             |
  |             +-----------------------+       |       +-----------------------+             |
  +---------------------------------------------+---------------------------------------------+
                                                |  Logical Data Independence
                                                v
  +-------------------------------------------------------------------------------------------+
  | CONCEPTUAL LEVEL                            |                                             |
  |                                 +-----------------------+                                 |
  |                                 |   Conceptual Schema   |                                 |
  |                                 | (Entities, Constraints)|                                |
  |                                 +-----------------------+                                 |
  +-------------------------------------------------------------------------------------------+
                                                |  Physical Data Independence
                                                v
  +-------------------------------------------------------------------------------------------+
  | INTERNAL LEVEL                              |                                             |
  |                                 +-----------------------+                                 |
  |                                 |    Internal Schema    |                                 |
  |                                 | (B+ Trees, Blocks, OS)|                                 |
  |                                 +-----------------------+                                 |
  +-------------------------------------------------------------------------------------------+
                                                |
                                                v
                                    +-----------------------+
                                    |   Physical Database   |
                                    +-----------------------+
```

---

### 1.1 The Three Levels

#### 1. External Level (View Level)
* **Description**: Highest level of abstraction. Describes how specific user groups view data.
* **Key Characteristics**:
  * Customized views for tailored user permissions.
  * Hides non-relevant database fields and security-sensitive information.
  * Computed/Derived fields (e.g., `Age` computed dynamically from `Date_Of_Birth`).
* **Example**: A student viewing their portal sees `Roll_No`, `Name`, `CGPA`. The finance officer sees `Roll_No`, `Fee_Paid`, `Outstanding_Dues`. Neither sees raw disk byte pointers or table partitioning schemas.

#### 2. Conceptual Level (Logical Level)
* **Description**: Community view of the database. Describes **what** data is stored in the database and **what relationships** exist among the data.
* **Key Characteristics**:
  * Defines entities, data types, attributes, relationships, and integrity constraints.
  * Completely independent of hardware or physical storage implementations.
  * Maintained using a Data Definition Language (DDL).
* **Example**: 
  ```sql
  CREATE TABLE Students (
      Roll_No INT PRIMARY KEY,
      Name VARCHAR(100) NOT NULL,
      Date_Of_Birth DATE,
      Department_ID INT REFERENCES Departments(ID)
  );
  ```

#### 3. Internal Level (Physical Level)
* **Description**: Lowest level of abstraction. Describes **how** data is physically stored on disk or secondary storage.
* **Key Characteristics**:
  * Defines physical record formats, file organizations (Heap, ISAM, B+ Tree), page layouts, block sizes, clustering keys, indexes, and compression algorithms.
* **Example**:
  ```text
  File: /var/lib/mysql/data/students.ibd
  Storage Engine: InnoDB
  Block Size: 16 KB
  Primary Key Index: B+ Tree with page fill factor 80%
  Compression: ZLIB
  ```

---

## 2. Data Independence

Data Independence is the ability to modify a schema definition at one level of the database architecture without requiring modifications to schema definitions at the next higher level.

```
       [ External View Schema ]
                  ^
                  |  <--- Logical Data Independence
                  v
       [ Conceptual Schema ]
                  ^
                  |  <--- Physical Data Independence
                  v
        [ Internal Schema ]
```

### 2.1 Logical Data Independence
* **Definition**: The capacity to modify the **Conceptual Schema** without having to alter the **External Schemata** or application programs.
* **Why it is needed**: As business requirements evolve, new columns or tables may be added, or existing tables may be split/merged.
* **How it is achieved**: Via database Views (`CREATE VIEW`) and Object Mappings.
* **Concrete Example**:
  * **Original Conceptual Table**: `Employees(Emp_ID, Name, Phone, Salary, Department)`
  * **Change**: Splitting `Employees` into two normalized tables:
    * `Emp_Personal(Emp_ID, Name, Phone)`
    * `Emp_Work(Emp_ID, Salary, Department)`
  * **Preserving External Independence**: We construct a logical view for legacy application programs:
    ```sql
    CREATE VIEW Employees AS
    SELECT p.Emp_ID, p.Name, p.Phone, w.Salary, w.Department
    FROM Emp_Personal p JOIN Emp_Work w ON p.Emp_ID = w.Emp_ID;
    ```
  * Legacy programs reading `Employees` require zero code changes.

### 2.2 Physical Data Independence
* **Definition**: The capacity to modify the **Internal Schema** without requiring alterations to the **Conceptual Schema** (or External Schemata).
* **Why it is needed**: Essential for performance tuning, storage migration, indexing optimizations, and disk layout changes.
* **Concrete Example**:
  * Changing an index structure on `Students` table from a Heap file scan to a B+ Tree index.
  * Moving database data files from traditional HDD spinning disks to NVMe Flash SSD storage.
  * Adding a secondary index on `Students(Department_ID)`.
  * *Result*: SQL queries written against the conceptual schema (`SELECT * FROM Students WHERE Department_ID = 5`) remain completely unchanged, though performance improves.

### Comparison Summary

| Attribute | Logical Data Independence | Physical Data Independence |
| :--- | :--- | :--- |
| **Schema Layer Modified** | Conceptual Schema | Internal Schema |
| **Schema Layer Unaffected**| External Schemata / Client Applications | Conceptual Schema & External Schemata |
| **Difficulty to Achieve** | High (Business logic/queries affected) | Moderate/Low (Standard DBMS feature) |
| **Common Trigger** | Changing business requirements | Performance tuning, hardware upgrade |

---

## 3. The Role of a Database Administrator (DBA)

A **Database Administrator (DBA)** is the person or operational team responsible for the design, management, maintenance, performance, security, and reliability of an enterprise database infrastructure.

```
                         +-----------------------+
                         |  DATABASE ADMIN (DBA) |
                         +-----------------------+
                                     |
    +-----------------+--------------+--------------+-----------------+
    |                 |                             |                 |
    v                 v                             v                 v
[ Security & ]   [ Performance ]              [ Backup & ]      [ Schema & ]
[ Access Ctrl]   [ Tuning & Indexing ]        [ Recovery ]      [ Migrations]
```

### Core Responsibilities of a DBA

1. **Schema Definition & Modification**: Authorizes conceptual DDL execution, defines internal physical parameters (table spaces, file extents, buffer pool sizes).
2. **Security & Access Authorization**: Grants roles, privileges (`GRANT`, `REVOKE`), and manages column/row-level security policies to prevent unauthorized access.
3. **Performance Monitoring & Tuning**:
   * Analyzes slow query logs (`EXPLAIN` plan analysis).
   * Creates/drops indexes, updates table statistics for the Query Optimizer.
   * Tunes buffer pool cache memory allocation (`innodb_buffer_pool_size`).
4. **Data Availability, Backup, and Disaster Recovery**:
   * Configures point-in-time recovery (PITR) backups (WAL archiving, snapshots).
   * Orchestrates high-availability (HA) failovers and disaster recovery (DR) drills.
5. **Capacity Planning**: Monitors database storage growth trends and provisions hardware/cloud resources before disk space saturation occurs.
6. **Data Integrity & Audit Enforcement**: Sets up integrity constraints, triggers, and auditing mechanisms for compliance (GDPR, HIPAA, PCI-DSS).

---

## 4. Conceptual Practice Exercises

1. Why is Logical Data Independence significantly harder to achieve than Physical Data Independence in commercial software applications?
2. If a database index is dropped to save write latency, which level of the ANSI-SPARC architecture is affected, and does the application code need to be rewritten?
3. Draw a mapping diagram illustrating how a user query traverses from the External level down to physical storage bytes.

---

## 5. Hard Placement & Interview Questions (FAANG Level)

### Q1: Suppose a database table `Orders` is sharded across 16 database nodes based on `Hash(Customer_ID) % 16`. At which level of the 3-Schema Architecture does sharding operate, and how is Data Independence preserved for an application developer writing standard SQL queries?
**Answer:**
* **Architecture Level**: Sharding is primarily an **Internal / Physical Schema** concern (though distributed routers may expose logical routing layers).
* **Data Independence Preservation**:
  1. Distributed DBMS middleware (e.g., Vitess, Citus, CockroachDB gateway) abstracts physical node partition mappings behind a unified **Conceptual Schema**.
  2. The application developer issues a standard query (`SELECT * FROM Orders WHERE Order_ID = 1001`).
  3. The distributed query planner intercepts the SQL statement, inspects metadata catalogs, routes the execution to the appropriate physical partition node, aggregates responses, and presents the result tuple set to the external view.
  4. If the cluster expands from 16 to 32 nodes (resharding), physical data independence ensures application code remains untouched.

---

### Q2: What happens when a view depends on a table whose schema is modified by dropping a column? How does a DBMS handle this broken mapping, and how can a DBA maintain Logical Data Independence?
**Answer:**
* **DBMS Behavior**: If a table column referenced by a view is dropped (`ALTER TABLE T DROP COLUMN C`), the view becomes **invalidated** (e.g., PostgreSQL marks view as broken, Oracle sets view status to `INVALID`). Subsequent queries against the view fail with a compilation or execution runtime error (`Column not found`).
* **Maintaining Logical Independence**:
  1. If column `C` is replaced or split into `C1` and `C2`, the DBA must update the View definition using `CREATE OR REPLACE VIEW` to compute or coalesce `C` from `C1` and `C2` (e.g., `CONCAT(C1, ' ', C2) AS C`).
  2. By updating the view mapping at the External-Conceptual boundary, downstream client applications invoking the view continue functioning without code alterations.

---

### Q3: Explain why catalog/metadata management (Data Dictionary) is the critical backbone of the ANSI-SPARC architecture.
**Answer:**
The **Data Dictionary** (System Catalog) stores metadata describing schemas, data types, user privileges, storage physical paths, and index structures.
Without the system catalog:
1. The DBMS cannot map external queries (`SELECT Name FROM View1`) to conceptual tables (`Students`) or internal storage blocks (`Block #4092`).
2. The Query Optimizer cannot perform type checking or access path optimization.
3. The catalog dynamically binds the levels of the 3-schema architecture at query execution time.
