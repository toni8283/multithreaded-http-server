# Multithreaded HTTP Server in C

A simple multithreaded HTTP server built from scratch in C using POSIX sockets and pthreads.

I built this project to better understand what actually happens behind web servers and HTTP frameworks. Instead of using a web framework or an existing server library, this project handles TCP connections, HTTP requests, responses, and concurrent clients directly in C.

## Features

* Creates a TCP server using POSIX sockets
* Listens for incoming connections on port `8080`
* Accepts multiple client connections
* Handles each connected client in a separate thread using pthreads
* Receives and displays incoming HTTP requests
* Serves an HTML page from the `public` directory
* Generates HTTP response headers manually
* Returns a `404 Not Found` response when the HTML file is unavailable
* Uses dynamic memory allocation for handling client sockets and file contents
* Supports quick server restarts using `SO_REUSEADDR`

## How It Works

The server follows a basic request-response flow:

1. A TCP socket is created using `socket()`.
2. The socket is bound to port `8080`.
3. The server starts listening for incoming connections.
4. When a client connects, `accept()` creates a new client socket.
5. A new thread is created to handle the connected client.
6. The thread receives the HTTP request using `recv()`.
7. The server reads the HTML file from the `public` directory.
8. HTTP headers and the HTML content are sent back to the client.
9. The client connection is closed and the thread finishes execution.

This allows the main server thread to continue accepting new connections while existing requests are handled concurrently.

## Project Structure

```text
multithreaded-http-server/
├── public/
│   └── index.html
├── .gitignore
├── Makefile
├── README.md
└── server.c
```

## Requirements

The project is intended for Linux or other POSIX-compatible systems.

You will need:

* GCC
* GNU Make
* POSIX Threads library

## Build

Clone the repository and enter the project directory:

```bash
git clone <repository-url>
cd multithreaded-http-server
```

Compile the server using:

```bash
make
```

## Run

Start the server with:

```bash
./server
```

The server will start listening on port `8080`.

Open the following address in a web browser:

```text
http://localhost:8080
```

You can also test the server using `curl`:

```bash
curl http://localhost:8080
```

To compile and run the server with a single command:

```bash
make run
```

To remove the compiled binary:

```bash
make clean
```

## Example Request Flow

```text
Browser / curl
      |
      | HTTP Request
      v
TCP Server (Port 8080)
      |
      | Accept Connection
      v
Create Client Thread
      |
      | Receive Request
      v
Read public/index.html
      |
      | Build HTTP Response
      v
Send Response to Client
      |
      v
Close Connection
```

## What I Learned

Building this project helped me understand several concepts that are normally handled internally by web frameworks and server libraries, including:

* TCP socket programming
* The client-server communication model
* How HTTP requests and responses are transferred over a network connection
* Binding a server process to a network port
* Accepting and handling incoming connections
* Concurrent request handling using pthreads
* Dynamic memory allocation in a network application
* Basic HTTP response construction
* Compiling and managing a C project using Make

## Current Limitations

This is currently a basic implementation created for learning systems programming and networking concepts.

Some current limitations are:

* Each connection creates a separate thread
* Only a single HTML file is served
* Only basic HTTP request handling is implemented
* Persistent HTTP connections are not supported
* There is currently no thread pool or bounded request queue

## Future Improvements

I plan to extend the project with:

* HTTP request parsing
* Support for multiple static files and routes
* MIME type detection
* A fixed-size thread pool
* A thread-safe request queue
* Mutexes and condition variables for synchronization
* Better error handling
* Request logging
* Graceful server shutdown
* Load testing and performance benchmarking

## Author

Toni Blair
