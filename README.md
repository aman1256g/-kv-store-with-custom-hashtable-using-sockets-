# Multi-Client TCP Server (C++)

A TCP server built from raw POSIX sockets in C++, designed to explore how a 
server handles multiple simultaneous client connections. Built while working 
through networking fundamentals — inspired by the build-your-own-redis 
approach to systems programming.

## Why this project

Most tutorial-level networking code handles one client at a time. Real 
servers don't get that luxury. This project exists to answer a concrete 
question: what are the actual trade-offs between the different ways a 
single-threaded (or multi-threaded) process can serve many clients at once? 
Rather than pick one approach, I implemented all three of the classic 
patterns and tested each one end-to-end.

## Concurrency models implemented

1. **`select()`-based I/O multiplexing** (`project.cpp`) — single thread, 
   single event loop. The server registers all active client file 
   descriptors and uses `select()` to block until any one of them has data 
   ready, avoiding the cost of a thread per client. This is the canonical 
   version used going forward.

2. **Non-blocking I/O with polling** (`multiple.cpp`) — sockets are set to 
   non-blocking mode and the server actively polls for incoming data rather 
   than sleeping until `select()` wakes it. Useful for understanding the 
   difference between event-driven and polling-based I/O.

3. **Thread-per-client** (`thread.cpp`) — each accepted connection spawns a 
   dedicated `std::thread`. Simplest mental model, but doesn't scale past a 
   moderate number of clients due to thread overhead.

All three were tested with multiple simultaneous client connections via 
`client.cpp`, confirming correct accept/receive/disconnect/slot-reuse 
behavior in each model.

## Protocol

Messages are framed using a **length-prefixed protocol**: each message is 
preceded by a 4-byte header specifying its length, so the receiver knows 
exactly how many bytes to read before the message is complete. This is why 
plain `nc` (netcat) can't talk to this server — it doesn't send that header.

## Files
- `project.cpp` — canonical select()-based multi-client server (main line of development)
- `thread.cpp` — thread-per-client concurrency model
- `multiple.cpp` — non-blocking I/O polling model
- `client.cpp` — test client implementing the length-prefixed protocol
- `run.sh` — build/run script

## Build & Run
\`\`\`bash
./run.sh
\`\`\`
(replace with your actual g++ command if run.sh doesn't cover it)

## Status / Next steps
The networking and protocol layer is complete and tested. Next step is 
adding a command-parsing layer on top (`GET`/`SET`/`DEL`) to turn this into 
an actual key-value store, following the build-your-own-redis book.

## What I learned
- Trade-offs between event-driven (`select()`), polling, and threaded 
  concurrency models for handling I/O
- Why message framing matters over TCP — TCP is a byte stream, not a 
  message stream, so the application layer has to define its own boundaries
- Socket lifecycle management (accept, read, write, disconnect, and 
  cleanly reusing client slots)
