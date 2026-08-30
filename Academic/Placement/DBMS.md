# DBMS Interview Questions

## Introduction to DBMS (Data, Information, DBMS Basics)

1. What is the difference between data and information?  
    Data is raw, unorganized facts with no inherent meaning, while information is data that has been processed, organized, and interpreted to provide context and enable decision-making.
    
2. What is a DBMS?  
    A DBMS (Database Management System) is a collection of interrelated data plus a set of programs to access, store, and manage that data conveniently and efficiently.
    
3. What is the primary goal of a DBMS?  
    To provide a convenient and efficient way to store, retrieve, and manage database information for an enterprise.
    
4. What are the major disadvantages of file-processing systems that DBMS solves?  
    Data redundancy and inconsistency, difficulty in accessing data, data isolation, integrity problems, atomicity problems, concurrent-access anomalies, and security problems.
    
5. Why is data redundancy a problem in file systems?  
    The same data stored in multiple files can become inconsistent when updated in one place but not another, leading to unreliable and conflicting information.
    
6. What is meant by "atomicity problems" in file-processing systems?  
    Without a DBMS, if a failure occurs mid-operation (e.g., a bank transfer), there's no guarantee that either all steps complete or none do, potentially leaving data in an inconsistent state.
    
7. Give an example of quantitative vs qualitative data.  
    Quantitative data is numerical, like the cost of an item; qualitative data is descriptive and non-numerical, like a person's hair color.
    

---

## DBMS Architecture

1. What is the Three-Schema Architecture?  
    It's a framework dividing a database system into three levels — physical/internal, logical/conceptual, and view/external — to provide users with an abstract view of data while hiding storage details.
    
2. What is the main objective of the three-level architecture?  
    To let multiple users access the same underlying data with personalized views, while the actual data is stored only once.
    
3. What does the physical/internal level describe?  
    How data is actually stored, including low-level data structures, storage allocation, compression, and encryption; its goal is efficient data access.
    
4. What does the logical/conceptual level describe?  
    What data is stored in the database and what relationships exist among that data, without exposing physical storage details; the DBA works at this level.
    
5. What is the view/external level used for?  
    It provides different, simplified views (subschemas) of the database to different user groups, hiding irrelevant parts and providing a security mechanism.
    
6. What is the difference between a database instance and a database schema?  
    An instance is the actual data stored in the database at a specific moment, while a schema is the structural design/blueprint of the database, which rarely changes.
    
7. What is physical data independence?  
    The property that changes to the physical schema (storage structure) should not require changes to the logical schema or application programs.
    
8. What are the two main types of database languages?  
    Data Definition Language (DDL) to specify schema, and Data Manipulation Language (DML) to query and update data.
    
9. What are the functions of a Database Administrator (DBA)?  
    Schema definition, storage structure and access method decisions, schema/physical organization modifications, authorization control, and routine maintenance like backups and patches.
    
10. What is the difference between two-tier (T2) and three-tier (T3) application architecture?  
    In two-tier, the client directly invokes DB functionality via ODBC/JDBC; in three-tier, a client communicates with an application server (holding business logic) which then talks to the database, improving scalability, security, and data integrity.
    
11. Why is three-tier architecture preferred for web applications?  
    Because it offers scalability through distributed application servers, better data integrity via a middle layer, and improved security since clients never access the DB directly.
    

---

## Entity-Relationship (ER) Model

1. What is an entity?  
    A distinguishable "thing" or "object" in the real world with physical existence that can be uniquely identified, typically via a primary key.
    
2. What is the difference between a strong entity and a weak entity?  
    A strong entity can be uniquely identified by its own attributes, while a weak entity lacks sufficient attributes for unique identification and depends on a strong (owner) entity for its existence.
    
3. Give an example of a strong and weak entity relationship.  
    Loan is a strong entity and Payment is a weak entity, since installment numbers are generated separately per loan and can't uniquely identify a payment on their own.
    
4. What is an entity set?  
    A set of entities of the same type sharing the same attributes/properties, such as "Student" or "Customer."
    
5. What are the types of attributes in the ER model?  
    Simple, composite, single-valued, multi-valued, and derived attributes.
    
6. What is the difference between a simple and composite attribute?  
    A simple attribute cannot be divided further (e.g., roll number), while a composite attribute can be broken into subparts (e.g., Name into first, middle, last name).
    
7. What is a derived attribute? Give an example.  
    An attribute whose value can be computed from other related attributes, such as Age being derived from Date of Birth.
    
8. What does a NULL value indicate in the ER model?  
    Either "not applicable" (the attribute doesn't exist for that entity, e.g., no middle name) or "unknown" (the value exists but isn't currently known).
    
9. What is the degree of a relationship?  
    The number of entities participating in the relationship — unary (1), binary (2), or ternary (3); binary relationships are the most common.
    
10. What is mapping cardinality (cardinality ratio)?  
    It specifies the number of entities to which another entity can be associated via a relationship — one-to-one, one-to-many, many-to-one, or many-to-many.
    
11. Give a real-world example each of one-to-one and many-to-many relationships.  
    One-to-one: a Citizen has an Aadhar Card. Many-to-many: a Student attends multiple Courses and a Course has multiple Students.
    
12. What is the difference between total and partial participation?  
    Total participation means every entity in the set must be involved in at least one relationship instance, while partial participation means some entities need not be involved at all.
    
13. Why does a weak entity always have total participation?  
    Because a weak entity cannot exist without its owning strong entity, so every weak entity instance must participate in the identifying relationship.
    

---

## Extended ER Features

1. What is specialization in the ER model?  
    The top-down process of dividing an entity set into sub-entity sets (subclasses) based on distinguishing characteristics, connected via an "is-a" relationship.
    
2. What is generalization, and how does it differ from specialization?  
    Generalization is the bottom-up reverse of specialization — combining entity sets with common attributes into a single generalized superclass to avoid data repetition.
    
3. What is attribute inheritance?  
    In both specialization and generalization, lower-level (subclass) entity sets inherit all attributes of the higher-level (superclass) entity set.
    
4. What is participation inheritance?  
    If a parent entity set participates in a relationship, its child entity sets automatically participate in that same relationship too.
    
5. What is aggregation in the ER model, and why is it used?  
    Aggregation treats a relationship as a higher-level abstract entity so that relationships among relationships can be modeled, avoiding redundancy.
    

---

## Relational Model & Keys

1. What is a tuple, and how does it differ from a relation?  
    A tuple is a single row representing one unique record, while a relation (table) is the collection of such tuples along with the schema defining its structure.
    
2. What is the degree and cardinality of a relation?  
    Degree is the number of attributes/columns in the relation; cardinality is the total number of tuples/rows in it.
    
3. What is a super key vs a candidate key?  
    A super key is any combination of attributes that uniquely identifies a tuple, while a candidate key is a minimal super key with no redundant attributes.
    
4. What is the difference between a primary key and an alternate key?  
    The primary key is the candidate key chosen (with the fewest attributes) to uniquely identify tuples; all other candidate keys not chosen become alternate keys.
    
5. What is a foreign key, and what purpose does it serve?  
    An attribute in one relation (referencing/child) that refers to the primary key of another relation (referenced/parent), used to cross-reference and enforce relationships between tables.
    
6. What is the difference between a composite key and a compound key?  
    A composite key is a primary key made of at least two attributes; a compound key is specifically a primary key formed using two foreign keys.
    
7. What is a surrogate key?  
    A synthetic, system-generated primary key (usually an auto-incrementing integer) that has no business meaning but uniquely identifies each tuple.
    
8. Can a foreign key value be NULL?  
    Yes — a foreign key value must either be NULL or match an existing value in the referenced relation's primary key.
    
9. What is the difference between domain, entity, and referential integrity constraints?  
    Domain constraints restrict attribute values to a defined type/range; entity constraints require every relation to have a non-null primary key; referential constraints ensure foreign key values match a tuple in the referenced relation (or are NULL).
    
10. List the key constraints available in SQL and briefly describe each.  
    NOT NULL disallows nulls, UNIQUE ensures distinct values (nulls allowed), DEFAULT sets a fallback value, CHECK enforces a condition, PRIMARY KEY uniquely identifies rows (not null + unique), and FOREIGN KEY enforces referential integrity to another table.
    
11. Why must every tuple in a table be unique?  
    Because relations are defined as sets of tuples representing distinct facts; duplicate tuples would represent the same relationship redundantly and violate relational model properties.
    

---

## Transforming ER Model to Relational Model

1. How is a strong entity converted into a relational table?  
    It becomes an individual table where attributes become columns, the entity's primary key becomes the table's primary key, and foreign keys are added for relationships.
    
2. How is a weak entity converted into a relational table?  
    A table is created with all its attributes, the owning strong entity's primary key is added as a foreign key, and the table's primary key is a composite of that foreign key plus the weak entity's partial (discriminator) key.
    
3. How are composite attributes handled during ER-to-relational conversion?  
    Each component of the composite attribute is added as a separate column in the relation, and the composite attribute itself is not represented directly.
    
4. How are multivalued attributes handled during ER-to-relational conversion?  
    A new table is created named after the attribute, containing the owning entity's primary key as a foreign key plus a column for the multivalued attribute, with the primary key being the combination of both.
    
5. Are derived attributes stored as columns in the relational schema?  
    No — derived attributes are typically not stored as columns since their values can be computed from other attributes.
    
6. Describe Method-1 for converting a generalization hierarchy to tables.  
    Create one table for the higher-level entity set with its own attributes, and a separate table for each lower-level entity set containing its specific attributes plus the primary key of the higher-level entity.
    
7. What is the drawback of Method-2 (no table for the higher-level entity) when generalization is overlapping or incomplete?  
    If overlapping, shared attributes get stored redundantly across multiple lower-level tables; if incomplete, entities that don't belong to any lower-level set can't be represented at all.
    
8. How is aggregation represented in the relational model?  
    A table is created for the relationship set itself, containing the primary keys of the participating entity sets (including the aggregated relationship's entities) plus any descriptive attributes.
    

---

## SQL Basics & Data Types

1. What does SQL stand for, and is it a database itself?  
    SQL stands for Structured Query Language; it is not a database but a query language used to access and manipulate data in a relational database.
    
2. What is the difference between SQL and MySQL?  
    SQL is the language used to perform CRUD operations on a relational database, while MySQL is an actual RDBMS software that implements and manages databases using SQL.
    
3. What are the four CRUD operations and their corresponding SQL statement types?  
    Create (INSERT), Read (SELECT), Update (UPDATE), and Delete (DELETE).
    
4. What is the difference between CHAR and VARCHAR?  
    CHAR is a fixed-length string type that always uses the declared size, whereas VARCHAR is variable-length and only occupies space equal to the actual data stored.
    
5. Why are variable-length data types like VARCHAR generally preferred over fixed-length types?  
    Because they occupy storage space proportional to the actual data size rather than always reserving the maximum declared length, saving space.
    
6. What is the difference between TEXT and BLOB in SQL?  
    Both can store large amounts of data of similar size ranges, but TEXT is meant for character/string data (with collation), while BLOB stores raw binary data.
    
7. What is an ENUM data type used for?  
    It restricts a column's value to one of a predefined set of preset values.
    

---

## SQL Command Categories

1. What are the five categories of SQL commands?  
    DDL (Data Definition Language), DRL/DQL (Data Retrieval/Query Language), DML (Data Manipulation Language), DCL (Data Control Language), and TCL (Transaction Control Language).
    
2. What operations fall under DDL?  
    CREATE, ALTER TABLE, DROP, TRUNCATE, and RENAME — commands that define or modify the relation schema.
    
3. What is the difference between DROP and TRUNCATE?  
    DROP deletes the entire table structure along with its data, while TRUNCATE removes all rows/tuples but keeps the table structure intact.
    
4. What is the difference between DELETE and TRUNCATE?  
    DELETE is a DML command that can remove specific rows via a WHERE clause and can be rolled back, while TRUNCATE is a DDL command that removes all rows at once and is generally not row-filterable.
    
5. What commands fall under DCL, and what do they do?  
    GRANT (gives access privileges to a user) and REVOKE (removes previously granted privileges).
    
6. What commands fall under TCL, and what do they do?  
    START TRANSACTION (begins a transaction), COMMIT (permanently applies changes), ROLLBACK (undoes changes), and SAVEPOINT (marks a point to roll back to within a transaction).
    

---

## SQL DRL/DQL & Filtering Clauses

1. What is the order of execution of a SELECT query's clauses?  
    Conceptually FROM, WHERE, GROUP BY, HAVING, SELECT, then ORDER BY — execution flows from the data source outward to final formatting, not top-to-bottom textually.
    
2. Can you use SELECT without a FROM clause? How?  
    Yes, using MySQL's DUAL table concept, e.g., `SELECT 55 + 11;` or `SELECT NOW();` for computing expressions without referencing a user table.
    
3. What is the difference between WHERE and HAVING?  
    WHERE filters individual rows before grouping and cannot use aggregate functions, while HAVING filters groups after GROUP BY and can use aggregate functions.
    
4. Can HAVING be used without GROUP BY?  
    No — using HAVING requires a GROUP BY clause to be meaningful (though in some engines it may work on the whole table as one group).
    
5. What is the difference between the IN and BETWEEN operators?  
    IN checks membership in an explicit list of values (reducing multiple OR conditions), while BETWEEN checks if a value falls within an inclusive numeric or date range.
    
6. What do the wildcard characters `%` and `_` mean in a LIKE clause?  
    `%` matches zero or more characters of any kind, while `_` matches exactly one character.
    
7. How can you retrieve distinct values from a column, and what's an alternative to DISTINCT?  
    Use `SELECT DISTINCT(col) FROM table;`; alternatively, `GROUP BY col` without an aggregate function produces the same distinct result set.
    
8. What is the purpose of ORDER BY, and what do ASC/DESC mean?  
    It sorts the result set by one or more columns; ASC sorts ascending (default) and DESC sorts descending.
    
9. What rule must be followed for columns used with GROUP BY?  
    All non-aggregated columns listed in the SELECT clause must also appear in the GROUP BY clause for the query to execute correctly.
    

---

## SQL Constraints & Alter Operations

1. What are the properties of a PRIMARY KEY constraint?  
    It must be NOT NULL and UNIQUE, and a table can have only one primary key (though it may span multiple columns).
    
2. Can a table have multiple UNIQUE constraints? Can UNIQUE columns hold NULL?  
    Yes, a table can have multiple UNIQUE attributes, and unlike a primary key, a UNIQUE column can hold NULL values.
    
3. What is the purpose of the CHECK constraint?  
    It enforces a condition on the values allowed in a column (e.g., `CHECK (age > 12)`), maintaining data integrity before and after CRUD operations.
    
4. What does the DEFAULT constraint do?  
    It sets a default value for a column that will be used automatically if no value is provided during insertion.
    
5. Can a column be both a primary key and a foreign key in the same table?  
    Yes, an attribute can simultaneously serve as a primary key and a foreign key (common in weak entity/composite key scenarios).
    
6. How do you rename a column vs renaming a table using ALTER?  
    `ALTER TABLE t CHANGE COLUMN old new datatype;` renames a column, while `ALTER TABLE t RENAME TO new_name;` renames the whole table.
    
7. What does ON UPDATE CASCADE do?  
    If the primary key of the referenced (parent) table is updated, this constraint automatically updates the matching foreign key values in the child table.
    
8. What is the difference between ON DELETE CASCADE and ON DELETE SET NULL?  
    ON DELETE CASCADE deletes child rows automatically when the referenced parent row is deleted; ON DELETE SET NULL instead sets the foreign key in child rows to NULL, keeping the child rows intact.
    
9. What does the REPLACE statement do, and how does it differ from UPDATE/INSERT?  
    REPLACE inserts a new tuple if no duplicate primary key exists, or replaces (deletes and re-inserts) an existing tuple if a duplicate is found — essentially UPDATE and INSERT combined based on primary key matching.
    

---

## SQL Joins & Set Operations

1. What is an INNER JOIN?  
    It returns only the rows that have matching values in both joined tables based on the join condition.
    
2. What is the difference between LEFT JOIN and RIGHT JOIN?  
    LEFT JOIN returns all rows from the left table plus matched rows from the right table (NULLs where no match); RIGHT JOIN does the same but keeps all rows from the right table.
    
3. Does MySQL natively support FULL JOIN? How is it emulated?  
    No, MySQL doesn't natively support FULL JOIN; it's emulated using a LEFT JOIN UNION-ed with a RIGHT JOIN of the same tables.
    
4. What is the difference between UNION and UNION ALL?  
    UNION combines result sets and removes duplicate rows, while UNION ALL combines them and keeps all duplicates, making it faster.
    
5. What is a CROSS JOIN, and when is it used?  
    It returns the Cartesian product of two tables (every row from one paired with every row from the other); it's rarely used practically since it multiplies row counts (e.g., 10 × 5 = 50 rows).
    
6. What is a SELF JOIN, and how is it implemented?  
    A join of a table with itself, typically emulated using an INNER JOIN with two aliases of the same table to compare rows within it.
    
7. Can you perform a join without using explicit JOIN keywords? How?  
    Yes, by listing multiple tables in the FROM clause separated by commas and specifying the join condition in the WHERE clause.
    
8. What are the key differences between JOIN and SET operations?  
    Joins combine data column-wise based on a matching condition and allow different data types/column counts between tables, while set operations (UNION, INTERSECT, MINUS) combine data row-wise, always return distinct rows (except UNION ALL), and require the same number and type of columns.
    
9. How is INTERSECT emulated in MySQL, and what does it return?  
    Using `SELECT DISTINCT column_list FROM table1 INNER JOIN table2 USING(join_cond);` — it returns rows common to both tables.
    
10. How is MINUS emulated in MySQL, and what does it return?  
    Using a LEFT JOIN with a WHERE clause checking that the second table's join column IS NULL — it returns distinct rows present in the first table but absent in the second.
    

---

## SQL Subqueries & Views

1. What is a subquery, and why might you use one instead of a JOIN?  
    A subquery is a nested query where the outer query depends on the inner query's result; it can be easier to understand and implement than a join, though generally slower since it shifts calculation burden to the application/user logic.
    
2. In which three clauses can a subquery typically appear?  
    Inside a WHERE clause, inside a FROM clause, or inside a SELECT clause.
    
3. What is a correlated subquery, and how does it differ from a normal nested subquery?  
    A correlated subquery references the outer query and re-executes once for each row considered by the outer query, whereas a normal subquery executes only once independently and its result is reused by the outer query.
    
4. What is a derived table/subquery used in a FROM clause?  
    A subquery result treated as a temporary named table (aliased) that the outer query can select from, e.g., for computing MAX over a filtered subset.
    
5. Between JOIN and subqueries, which is generally faster and why?  
    JOINs are generally faster because the DBMS optimizes and executes them in a single combined operation, whereas subqueries can require repeated execution and shift computation responsibility to the query layer.
    
6. What is a MySQL VIEW?  
    A virtual table defined by a stored query that presents rows and columns from one or more base tables without storing any data of its own.
    
7. What happens to a VIEW if the underlying base table data changes?  
    The changes are automatically reflected in the view, since a view is just a saved query definition, not stored data.
    
8. Can a view be created using a JOIN of multiple tables?  
    Yes — for example, a view can select and combine columns from two tables joined on a common condition.
    

---

## Normalisation

1. What is a functional dependency (FD)?  
    A relationship where the value of one attribute (or set of attributes), the determinant, determines the value of another attribute, the dependent, typically written as X → Y.
    
2. What is the difference between a trivial and a non-trivial functional dependency?  
    A trivial FD (A → B) exists when B is a subset of A; a non-trivial FD exists when B is not a subset of A (their intersection is empty).
    
3. State Armstrong's three axioms for functional dependencies.  
    Reflexivity (if B is a subset of A, then A → B holds), Augmentation (if A → B holds, then AX → BX also holds), and Transitivity (if A → B and B → C, then A → C).
    
4. Why is normalization performed?  
    To minimize data redundancy and eliminate undesirable characteristics like insertion, update, and deletion anomalies, which improves database performance and consistency.
    
5. What are the three types of anomalies caused by data redundancy?  
    Insertion anomaly (can't insert data without unrelated data being present), deletion anomaly (deleting a row unintentionally loses other important data), and update anomaly (a single logical update requires changing multiple rows, risking inconsistency).
    
6. What is required for a relation to be in 1NF?  
    Every cell must contain only atomic (indivisible) values, and the relation must not have multi-valued attributes.
    
7. What additional condition must be satisfied for 2NF beyond 1NF?  
    The relation must be in 1NF and have no partial dependency — every non-prime attribute must depend on the whole primary key, not just part of it.
    
8. What additional condition must be satisfied for 3NF beyond 2NF?  
    The relation must be in 2NF and have no transitive dependency — a non-prime attribute must not depend on another non-prime attribute.
    
9. How does BCNF differ from 3NF?  
    BCNF requires that for every functional dependency A → B, A must be a super key, which is a stricter condition than 3NF and eliminates certain anomalies 3NF can still allow.
    
10. What are the main advantages of normalization?  
    It minimizes data redundancy, improves overall database organization, and maintains data consistency.
    

---

## Transactions & ACID Properties

1. What is a transaction in DBMS?  
    A logical unit of work containing one or more SQL statements that either completes fully (commits) or is entirely undone (rolled back) as a single unit.
    
2. What does the "A" in ACID stand for, and what does it guarantee?  
    Atomicity — it guarantees that either all operations of a transaction are reflected in the database, or none are.
    
3. What does Consistency mean in the context of ACID?  
    Integrity constraints must be maintained before and after a transaction, so the database moves from one valid consistent state to another.
    
4. What does Isolation guarantee in concurrent transactions?  
    That even though multiple transactions execute concurrently, each transaction behaves as if it ran either completely before or completely after any other overlapping transaction, unaware of concurrent execution.
    
5. What does Durability guarantee?  
    Once a transaction commits successfully, its changes persist permanently even if the system subsequently crashes.
    
6. What are the possible states of a transaction?  
    Active, Partially Committed, Committed, Failed, Aborted, and Terminated.
    
7. What is the difference between the "Partially Committed" and "Committed" states?  
    In Partially Committed, changes are saved only in the main-memory buffer after execution; in Committed, changes have been made permanent on disk and the transaction cannot be rolled back anymore.
    
8. Can a transaction be rolled back once it reaches the Committed state?  
    No — rollback is only possible before commit; once committed, a new consistent state is achieved and cannot be undone via rollback.
    
9. What happens when a transaction enters the Failed state?  
    It's determined that normal execution can no longer proceed due to an error, so the changes made so far in the buffer must be reversed, leading to the Aborted state after rollback.
    

---

## Atomicity & Durability Implementation (Recovery)

1. What DBMS component is responsible for ensuring atomicity and durability?  
    The Recovery Mechanism component of the DBMS.
    
2. What is the shadow-copy scheme?  
    A recovery technique where a transaction creates a full copy of the DB to update, leaving the original (shadow) copy untouched; a db-pointer is switched to the new copy only once all updates are safely written, ensuring atomicity and durability.
    
3. Why must the db-pointer update in the shadow-copy scheme be atomic?  
    Because the entire scheme's correctness depends on the pointer switch happening as a single indivisible operation; this is achieved by ensuring the pointer lies entirely within a single disk sector/block.
    
4. What is a major drawback of the shadow-copy scheme?  
    It is inefficient because the entire database must be copied for every transaction.
    
5. What is log-based recovery, and why must logs be written before the actual DB update?  
    It's a technique where a sequential record (log) of every operation is maintained in stable storage before the operation is applied to the database, so that failures can be recovered from the log ("write-ahead logging").
    
6. What is the difference between Deferred and Immediate database modification techniques?  
    In Deferred modification, actual writes to the DB are postponed until the transaction fully completes (logs used to redo on completion); in Immediate modification, writes may happen while the transaction is still active (uncommitted), requiring undo capability if it fails.
    
7. In Immediate DB modification, how does the system recover from a crash before a transaction completes?  
    It uses the "old value" field recorded in the log to undo (restore) the modifications made by that incomplete transaction.
    
8. In Deferred DB modification, what happens if the system crashes after the transaction commits but before all writes are applied?  
    The system performs a redo using the log records associated with that transaction to complete the deferred writes.
    

---

## Indexing in DBMS

1. What is indexing, and why is it used?  
    Indexing is a data structure technique used to optimize database performance by minimizing the number of disk accesses needed to locate and retrieve data for a query.
    
2. What are the two components of an index record?  
    The search key (a copy of the primary/candidate key or another attribute) and the data reference (a pointer to the disk block where the actual record is stored).
    
3. Is indexing mandatory, and is the index file sorted?  
    No, indexing is optional (a secondary means of access, not the primary one), but the index file itself is always kept sorted.
    
4. What is the difference between a dense index and a sparse index?  
    A dense index has an index entry for every distinct search-key value in the data file, while a sparse index has entries for only some values, covering a range of records per entry.
    
5. What is a primary (clustering) index, and what does the term "primary" actually refer to?  
    It's an index whose search key also defines the sequential/physical order of the data file; note the term is often loosely (and non-standardly) used to mean an index on the primary key specifically.
    
6. When the data file is sorted on a key attribute (primary key), is the resulting index dense or sparse?  
    Sparse — since the primary key uniquely orders the file, one index entry per data block suffices.
    
7. When the data file is sorted on a non-key attribute, is the resulting index dense or sparse?  
    Dense — because every unique non-key attribute value needs its own entry in the index file.
    
8. Why is a multi-level index used?  
    When a single-level index grows large enough that binary searching it becomes slow, breaking the index into multiple levels reduces the search time by narrowing the search space at each level.
    
9. What is a secondary (non-clustering) index, and why is it always dense?  
    It's an index built when the data file is unsorted (primary indexing isn't possible), so every record needs its own index entry — making the number of index entries equal to the number of records in the data file.
    
10. What are the main disadvantages of indexing?  
    It requires additional storage space for the index structures, and it decreases performance for INSERT, DELETE, and UPDATE operations since indexes must also be maintained.
    

---

## B-Trees & Indexing Internals

1. Why are B-Trees (and B+ Trees) commonly used for database indexing instead of plain binary search trees?  
    Because they are balanced, multi-way trees optimized for disk-based storage — each node can hold many keys, drastically reducing tree height and the number of disk I/O operations needed per lookup.
    
2. What is the key structural property of a B-Tree that keeps it balanced?  
    All leaf nodes are at the same depth, and every node (except possibly the root) has a minimum and maximum number of children, so insertions/deletions trigger splits or merges to maintain balance.
    
3. What is the main difference between a B-Tree and a B+ Tree used in databases?  
    In a B+ Tree, all actual data (or data pointers) are stored only in the leaf nodes, which are also linked together sequentially, while internal nodes hold only keys for navigation — making range queries and sequential scans much faster than a standard B-Tree, which can store data in internal nodes too.
    
4. Why do B+ Trees suit range queries better than hash indexes?  
    Because B+ Tree leaves are sorted and linked in sequence, so once you find the start of a range you can simply traverse the linked leaves, whereas hash indexes provide no ordering and can't efficiently support range scans.
    
5. How does node splitting work when inserting into a full B-Tree node?  
    When a node exceeds its maximum key capacity, it's split into two nodes, and the median key is pushed up to the parent node, potentially cascading splits up to the root.
    
6. Why does increasing the "fanout" (branching factor) of a B-Tree reduce query time?  
    A higher fanout means each node holds more keys, reducing the tree's height for a given number of records, which directly reduces the number of disk block reads required to reach a leaf.
    

---

## NoSQL Databases

1. What does NoSQL stand for, and what are its main characteristics?  
    "Not only SQL" — non-tabular databases that are schema-free, use flexible/dynamic data structures, handle huge volumes of data (big data), and typically support horizontal scaling.
    
2. What are the four main types of NoSQL data models?  
    Document, Key-Value, Wide-Column (Columnar), and Graph.
    
3. How does a Key-Value store work, and what are common use cases?  
    Every data element is stored as a key paired with a value (which can be simple or complex); common use cases include shopping carts, caching, and user session/profile data.
    
4. What is a Document-based NoSQL store, and does it support ACID transactions?  
    It stores data as documents similar to JSON objects with field-value pairs; document stores like MongoDB do support ACID properties, making them suitable for transactional use cases.
    
5. What is a Wide-Column (Columnar) store optimized for?  
    Analytics and aggregation queries — since data is organized by columns rather than rows, reading a subset of columns avoids loading unnecessary data, and compression is more efficient.
    
6. What is a Graph database optimized for, and give a use case.  
    It focuses on relationships between data elements, storing connections as first-class links rather than implied joins; use cases include fraud detection, social networks, and knowledge graphs.
    
7. Is it true that NoSQL databases can't model relationship data well? Why or why not?  
    This is a misconception — NoSQL databases can store relationship data, just differently, often nesting related data within a single document/structure, which some find easier to model than splitting data across relational tables.
    
8. Is it true that no NoSQL database supports ACID transactions?  
    No, that's a misconception too — some NoSQL databases, like MongoDB, do support ACID transactions, even though many other NoSQL systems don't guarantee full ACID compliance.
    
9. Why did NoSQL databases emerge, historically?  
    As storage costs dropped, developer productivity became the primary cost driver, so NoSQL databases traded strict schema/normalization for flexibility, faster iteration, unstructured data support, and cloud-native horizontal scalability.
    
10. What are the main disadvantages of NoSQL databases?  
    Higher data redundancy (optimized for query speed, not deduplication), costly update/delete operations, no single NoSQL model fits every use case, and generally no ACID guarantees or consistency constraints.
    
11. In SQL vs NoSQL, how do they differ in scaling approach?  
    SQL databases traditionally scale vertically (scale-up, more powerful hardware), while NoSQL databases are designed to scale horizontally (scale-out across commodity servers).
    

---

## Types of Databases

1. What are the key characteristics of Relational Databases?  
    They're based on the relational model, use SQL for CRUD operations, are highly optimized for structured data with strong normalization guarantees, but face scalability (horizontal scaling) issues as data grows.
    
2. What is an Object-Oriented Database, and what OOP concepts does it borrow?  
    A database model based on the object-oriented programming paradigm, borrowing inheritance, object-identity, and encapsulation to store data as complete "objects" rather than splitting it across tables.
    
3. What is a major disadvantage of Object-Oriented Databases compared to relational ones?  
    High complexity slows down read/write/update/delete operations, and they have much less community support and adoption; they also lack view support like relational databases.
    
4. What is a Hierarchical Database, and what is its structural rule?  
    A tree-like database where each parent record can have multiple child records, but each child record can only have exactly one parent; e.g., IBM IMS.
    
5. What is the major disadvantage of Hierarchical Databases?  
    Their inflexible one-to-many structure can't represent complex relationships where a child has multiple parents, and traversal is time-consuming, top-to-bottom, often requiring redundant data storage.
    
6. How does a Network Database differ from a Hierarchical Database?  
    A Network Database is an extension of the hierarchical model that allows child records to associate with multiple parent records, organizing data in a graph structure instead of a strict tree.
    
7. What is a disadvantage of Network Databases?  
    Many-to-many (M:N) links can cause slow retrieval, maintenance is tedious, and there's limited modern web community support.
    

---

## Clustering in DBMS

1. What is database clustering?  
    The process of combining more than one server/instance connected to a single logical database (forming replica-sets) to handle data or request volumes a single server cannot manage alone.
    
2. How does clustering provide data redundancy, and is this a bad thing?  
    It replicates the same dataset across multiple servers, so if one server fails, data remains available from another; this redundancy is intentional and beneficial (not the harmful kind caused by lack of normalization).
    
3. What is load balancing in a database cluster, and how does it relate to high availability?  
    It's the practice of distributing workload across cluster servers so no single machine gets overwhelmed by traffic spikes; effective load balancing directly enables and supports high availability.
    
4. How does clustering achieve high availability?  
    By having multiple redundant machines and load balancing, so if one server goes down, another can still serve requests, keeping the database accessible.
    
5. What happens during a node failure in a well-designed cluster architecture?  
    Requests are handled by another available node, minimizing or eliminating downtime and reducing the chance of an absolute system failure.
    

---

## Partitioning & Sharding

1. What is database partitioning, and why is it useful?  
    The technique of dividing a large database into smaller, manageable slices called partitions, which increases performance, controllability, and manageability without needing to alter application queries.
    
2. What is the difference between vertical and horizontal partitioning?  
    Vertical partitioning slices a relation column-wise (requiring access to multiple servers to reconstruct a full tuple); horizontal partitioning slices it row-wise, storing independent chunks of complete tuples in different servers.
    
3. Under what conditions is partitioning typically applied?  
    When the dataset becomes too large to manage efficiently, or when request volume is high enough that a single DB server's response time becomes unacceptably slow.
    
4. What are the main advantages of partitioning?  
    Parallelism, availability, performance, manageability, and reduced cost compared to expensive vertical scaling.
    
5. What is Sharding, and how does it relate to horizontal partitioning?  
    Sharding is the specific technique used to implement horizontal partitioning — data is split across multiple DB instances, with a routing layer directing requests to the instance holding the relevant data.
    
6. What are the pros and cons of Sharding?  
    Pros are scalability and availability; cons include added complexity (partition mapping and routing layer), the need for re-sharding when data becomes non-uniform, and poor support for analytical queries spanning shards (the scatter-gather problem).
    
7. What is a distributed database, and how does it relate to clustering, partitioning, and sharding?  
    It's a single logical database spread across multiple locations/servers, logically interconnected by a network — essentially the product of applying clustering, partitioning, and sharding together.
    

---

## Database Scaling Patterns (Case Study Progression)

1. What performance problems arise as request volume grows on a single small DB server?  
    Increased API latency, transactions facing deadlocks and starvation, sluggish application experience, and eventually customer dissatisfaction due to frequent failures.
    
2. What is the first low-effort optimization pattern typically applied before scaling infrastructure?  
    Query optimization and connection pooling — caching frequently accessed non-dynamic data and using connection pool libraries so multiple application threads can reuse cached DB connections instead of creating new ones each time.
    
3. What is Vertical Scaling (Scale-up), and what is its main limitation?  
    Upgrading the existing machine's hardware (more RAM, faster SSD, etc.); it's pocket-friendly initially, but cost increases exponentially as you scale up further, and there's a hard ceiling on a single machine's capacity.
    
4. What is CQRS (Command Query Responsibility Segregation) as a scaling pattern, and what problem can it introduce?  
    It separates read and write operations onto different physical machines — writes go to a primary and reads go to replicas; the main issue is replication lag between primary and replicas, which can serve stale data and impact user experience.
    
5. What is Multi-Primary Replication, and how does it differ from single-primary (CQRS) replication?  
    All machines act as both primary and replica in a logical ring, allowing writes to any node instead of a single bottlenecked primary, with reads served by whichever node responds first to a broadcast.
    
6. What is Partitioning of Data by Functionality as a scaling pattern?  
    Separating different types of data (e.g., location tables vs booking tables) into separate database schemas/machines, requiring the application/backend layer to take responsibility for joining results across them.
    
7. What is the difference between scaling vertically and scaling horizontally in this progression of patterns?  
    Vertical scaling upgrades a single machine's resources with a hard cost/capacity ceiling, while horizontal scaling (sharding) adds many machines, each holding only part of the data, offering much greater scalability at the cost of higher architectural complexity.
    
8. Why is Sharding described as "generally hard to apply" despite its benefits?  
    Because it requires careful data locality design, routing logic, and handling of cross-shard queries and re-sharding — significant engineering complexity compared to simpler scaling patterns.
    
9. What problem does Data Centre Wise Partitioning solve, and what is its key benefit?  
    It addresses high latency from requests traveling across continents by distributing traffic across geographically distributed data centers; a key benefit is that cross-data-center replication also enables disaster recovery and maintains system availability.
    

---

## CAP Theorem

1. What does the CAP theorem state?  
    A distributed system can only guarantee two out of three properties simultaneously: Consistency, Availability, and Partition Tolerance.
    
2. Define Consistency in the context of CAP theorem.  
    All nodes see the same data at the same time; a read always returns the value of the most recent write, regardless of which node is queried.
    
3. Define Availability in the context of CAP theorem.  
    Every request receives a response (though not necessarily the most recent data), meaning the system stays operational even if some individual nodes are down.
    
4. Define Partition Tolerance in the context of CAP theorem.  
    The system continues to operate correctly despite network partitions — dropped or delayed messages between nodes — by replicating data across combinations of nodes.
    
5. Why is Partition Tolerance essentially non-negotiable in real distributed systems?  
    Because network partitions are inevitable in any real-world distributed system, so practically the meaningful tradeoff is between Consistency and Availability when a partition occurs.
    
6. What is a CP database, and give an example along with a suitable use case.  
    A CP database favors Consistency and Partition Tolerance over Availability, turning off inconsistent nodes during a partition; MongoDB is an example, well-suited for systems like banking where consistency outweighs availability.
    
7. What is an AP database, and give an example along with a suitable use case.  
    An AP database favors Availability and Partition Tolerance, keeping all nodes responsive even if not fully updated, syncing later for eventual consistency; Apache Cassandra is an example, suited for apps like social media (e.g., Facebook) where availability matters more than instant consistency.
    
8. Why is a pure CA database impractical in real distributed systems?  
    Because CA databases can't handle partitions — but partitions are bound to happen in any distributed system, making fault tolerance essential; some centralized relational databases (like a single-node MySQL/PostgreSQL) can be considered CA since they don't face network partitions.
    
9. In MongoDB's CP design, how does write handling work in a replica set?  
    Only one primary node receives all write requests in a given replica set; secondary nodes replicate data from the primary, and if the primary fails, a secondary can be promoted to stand in as the new primary.
    
10. Why does Cassandra's AP design have no single primary node?  
    Because all nodes remain equally available for both reads and writes, allowing the system to stay responsive during partitions; consistency across nodes is achieved eventually through re-syncing after the partition resolves.
    

---

## Replication & Master-Slave Architecture (incl. GCP Datastream context)

1. What is the Master-Slave database concept, and what problem does it solve?  
    An architecture where write operations go to a single Master database and read operations are served only from Slave replicas, optimizing I/O so a single server doesn't get overloaded by combined read and write traffic.
    
2. Why is the Master-Slave pattern considered the same as CQRS?  
    Because it segregates command (write) responsibility to the master and query (read) responsibility to the slaves, which is exactly the Command Query Responsibility Segregation pattern.
    
3. What is the difference between synchronous and asynchronous replication?  
    In synchronous replication, the master waits for acknowledgment from replicas before confirming a write as complete (stronger consistency, higher latency); in asynchronous replication, the master commits and returns immediately while replicas catch up independently (lower latency, risk of replication lag/stale reads).
    
4. What is replication lag, and why does it matter for application design?  
    It's the delay between when data is written to the master and when it becomes visible on a replica; if an application reads from a replica immediately after writing to the master, it may see stale data, so read-after-write consistency needs special handling.
    
5. In a Change Data Capture (CDC) pipeline like GCP Datastream, what is the core idea behind how it replicates data?  
    Datastream reads database transaction/change logs (similar to log-based recovery logs) from a source database and streams row-level insert/update/delete events downstream to a destination in near real-time, rather than doing full-table batch copies each time.
    
6. Why is log-based CDC (like what Datastream uses) generally preferred over polling-based replication?  
    Because it captures changes with minimal load on the source database (reading logs is far cheaper than repeated SELECT queries), delivers lower latency, and preserves the exact order and nature (insert/update/delete) of changes.
    
7. What source-side requirement is typically needed for a database to support log-based CDC replication?  
    The database must have some form of durable change/transaction log available for reading (e.g., MySQL binary log, PostgreSQL WAL, or Oracle redo logs) with adequate retention configured.
    
8. How does eventual consistency in a replicated system like Datastream-fed pipelines differ from the strict consistency of a CP database?  
    Eventually consistent systems allow downstream replicas/destinations to temporarily lag behind the source, converging to the same state given enough time, whereas a CP database blocks or restricts access to nodes that aren't immediately consistent.
    
9. What role does a "primary" or source database play if a downstream analytics destination (like BigQuery) is fed via replication/streaming?  
    The source database remains the single system of truth for transactional writes, while the destination serves as a read-optimized, eventually consistent copy for analytics — mirroring the master-slave pattern of directing writes to one place and reads to another.
    
10. Why might replication be combined with partitioning/sharding in a large-scale system?  
    Each shard can have its own set of replicas for fault tolerance and read scaling, so the system gets both horizontal scalability (via sharding) and high availability/durability (via replication) simultaneously.
    
11. What is a key risk of asynchronous multi-region replication for disaster recovery, and how is it usually mitigated?  
    Data written just before a failure might not have replicated to the backup region yet, risking data loss; this is mitigated by tuning replication frequency, using semi-synchronous replication for critical writes, and monitoring replication lag closely.
    
12. How does the Master-Slave/replication pattern support disaster recovery?  
    Because data centre wise partitioning enables cross-data-centre replication, if one data centre or region fails, another replica location already holds a recent copy of the data and can take over, maintaining availability.