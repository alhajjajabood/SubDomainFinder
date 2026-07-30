# SubFinder - Multithreaded Subdomain Discovery Tool

A high-performance, multithreaded C++ subdomain enumeration tool that uses DNS resolution to discover subdomains for a given target domain.

## Features

- **Multithreaded** – Uses a configurable number of threads (defaults to hardware concurrency) for concurrent DNS lookups
- **Thread-Safe Queue** – Implements a producer-consumer pattern with `SafeQueue<T>` using mutexes and condition variables
- **Progress Reporting** – Real-time output showing processed count, found subdomains, and elapsed time
- **Cross-Platform** – Supports both Linux (POSIX) and Windows (Winsock)
- **Subdomain Wordlist** – Includes a comprehensive wordlist (`microws.txt`) with 4000 common subdomain entries

## How It Works

1. Loads a wordlist of potential subdomain names from a text file.
2. For each word, constructs `word.target.com` and attempts DNS resolution via `getaddrinfo`.
3. Worker threads pull from a shared queue and resolve concurrently.
4. All discovered subdomains are stored in a thread-safe set and displayed to the user.

## Usage

```bash
g++ -std=c++11 -pthread -o subfinder subfinder.cpp
./subfinder [target_domain] [wordlist_path]
```

If arguments are omitted, the program will prompt for them interactively.

### Example

```bash
./subfinder example.com microws.txt
```

## Files

| File | Description |
|------|-------------|
| `subfinder.cpp` | Main C++ source code |
| `microws.txt` | Wordlist of 4000 common subdomains |

## Building

### Linux
```bash
g++ -std=c++11 -pthread -o subfinder subfinder.cpp
```

### Windows (MinGW)
```bash
g++ -std=c++11 -lws2_32 -o subfinder.exe subfinder.cpp
```

## Requirements

- C++11 compatible compiler
- pthread (Linux) / Winsock2 (Windows)
