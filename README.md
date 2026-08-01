# SubFinder - Multithreaded Subdomain Discovery Tool

**SubFinder** is a high‑performance, multithreaded C++ subdomain enumeration tool that uses DNS resolution to discover subdomains for a given target domain.

It is **free to use** and you are welcome to modify and update it for your own purposes. No license fees, no restrictions on personal or internal commercial use.

## Features

- **Multithreaded** – Automatically calculates and uses hardware concurrency (`thread::hardware_concurrency()`) to set the thread count
- **Thread-Safe Queue** – Implements a producer‑consumer pattern with `SafeQueue<T>` using mutexes and condition variables
- **Progress Reporting** – Real‑time output showing processed count, found subdomains, and elapsed time
- **Cross‑Platform** – Supports both Linux (POSIX) and Windows (Winsock)
- **Subdomain Wordlist** – Includes a comprehensive wordlist (`microws.txt`) with 4000 common subdomain entries

## How It Works

1. Loads a wordlist of potential subdomain names from a text file.
2. For each word, constructs `word.target.com` and attempts DNS resolution via `getaddrinfo`.
3. Worker threads pull from a shared queue and resolve concurrently.
4. All discovered subdomains are stored in a thread‑safe set and displayed to the user.

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

## Free to Use & Modify

This tool is provided free of charge. You are permitted to use, copy, modify, and update it for your own needs. If you make improvements, you are encouraged to share them back (though it's not required).

---

**Happy Subdomain Hunting!** 🕵️
