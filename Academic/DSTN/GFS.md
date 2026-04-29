GFS is a distributed file system. It's to access a large amount of data that can't be stored in a single disk, and also for other purposes.

Assumes that there are many concurrent reads and writes, and that most of the writes are mostly append only.

It also sacrifices some consistency, for performance and availability

There are a bunch of servers. All of them are called chunk servers, as it's gonna store many chunks of large files.

The default size of each chunk is 64 MB, as all of them are stored as a plain linux file in the chunk server.

Each chunk is stored on 3 different chunk servers, as replicas for availability.

We also have a GFS Master. It saves in memory the meta datas of all the chunk. It stores a mapping of the file names and the lists of their chunk server ids. It also stores a mapping of the chunk id to the version number, and the replicas, and which one is the primary replica.