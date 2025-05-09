# README

## Overview

This project includes two C programs—a TCP **server** and **client**—that exchange text and return its reversal over a network connection.

## Compilation

Compile on any Linux system (e.g., Jason/Volta) or WSL:

```bash
gcc server.c -o server
gcc client.c -o client
```

## Usage

Open **two terminals** (or MobaXterm splits/tabs):

1. **Start the server** on port `5000`:
   ```bash
   ./server 5000
   ```
   Expected output:
   ```
   Server listening on port 5000
   ```

2. **Run the client** against the server:
   ```bash
   ./client 127.0.0.1 5000
   ```

### Interactive Mode

- **Enter** your message (e.g. `Hello World`) and press **Enter**.  
- **Press** `Ctrl+D` to signal end-of-input.  
- The client displays the reversed text and exits:
  ```
  dlroW olleH
  Connection closed
  ```

### Non‑interactive Mode

Send data via a shell pipe:
```bash
echo "Hello World" | ./client 127.0.0.1 5000
```
Prints reversed text immediately.

## Example

**Server terminal:**
```bash
$ ./server 5000
Server listening on port 5000
Client connected
Client disconnected
```

**Client terminal:**
```bash
$ echo "Hello World" | ./client 127.0.0.1 5000
dlroW olleH
Connection closed
```

## Notes
- Uses standard glibc—no external libraries needed.  
- Ensure your port (e.g., `5000`) is free.  
- To connect across hosts, replace `127.0.0.1` with the server’s IP address.
