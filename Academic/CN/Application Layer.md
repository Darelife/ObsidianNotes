[[Physical Layer|Previous Slide]]
# Client Server
Regular Stuff (HTTP, IMAP, FTP)

## Server
- Always On, with a permanent IP

## Client
- Contacts the server

# Peer to Peer
- no server
- peers are intermittently connected, and change IPs
- eg: P2P file sharing (bit torrent)

# Sockets
- procs can send/receive msgs to/from its socket

# TCP & UDP
## TCP
- reliable transport between sending and receiving process
- flow control: sender won’t overwhelm receiver 
- congestion control: throttle sender when network overloaded
- connection-oriented: setup required between client and server processes
- does not provide: timing, minimum throughput guarantee, security

## UDP

- unreliable data transfer between sending and receiving process
- does not provide: reliability, flow control, congestion control, timing, throughput guarantee, security, or connection setup.