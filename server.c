#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
*   COMPE560 - Spring 2025
*   Victor Hugo Silva - 826603798
*   Assignment: Introduction to Socket Programming 
*
*/


// Buffer size for reading from socket and stdout
#define BUF_SIZE 4096

/**
 * Prints an error message and exits.
 * @param msg  Context string to print before the system error.
 */
void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

/**
 * Reverse the contents of a buffer in place.
 * @param buf  Pointer to the data buffer.
 * @param len  Number of bytes in the buffer to reverse.
 */
void reverse_buffer(char *buf, ssize_t len) {
    for (ssize_t i = 0; i < len / 2; ++i) {
        char tmp = buf[i];              // store front element
        buf[i] = buf[len - 1 - i];      // copy back element to front
        buf[len - 1 - i] = tmp;         // restore front element to back
    }
}

/**
 * Main server entry point.
 * Usage: ./server <port>
 * Listens on the given TCP port, accepts a connection,
 * echoes and reverses any incoming data back to the client.
 */
 
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);  // Convert port argument to integer

    // Create a TCP socket (IPv4)
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0) {
        die("socket");
    }

    // Prepare server address structure (bind to all interfaces)
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;                // IPv4
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0 (all interfaces)
    addr.sin_port = htons(port);              // port in network byte order

    // Bind the socket to our address and port
    if (bind(serv_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        die("bind");
    }

    // Start listening, allow up to 5 pending connections
    if (listen(serv_sock, 5) < 0) {
        die("listen");
    }
    printf("Server listening on port %d\n", port);

    // Accept and handle clients in a loop
    while (1) {
        // accept() blocks until a client connects
        int clnt_sock = accept(serv_sock, NULL, NULL);
        if (clnt_sock < 0) {
            die("accept");
        }
        printf("Client connected\n");

        // Convert socket descriptors to FILE* streams for fread/fwrite
        FILE *in = fdopen(clnt_sock, "r");
        FILE *out = fdopen(dup(clnt_sock), "w");
        if (!in || !out) {
            die("fdopen");
        }

        char buf[BUF_SIZE];
        ssize_t n;

        // Read chunks until EOF (client closed write end)
        while ((n = fread(buf, 1, BUF_SIZE, in)) > 0) {
            // Print received data to server's stdout
            fwrite(buf, 1, n, stdout);
            fflush(stdout);

            // Reverse the data in place
            reverse_buffer(buf, n);

            // Send reversed data back to the client
            fwrite(buf, 1, n, out);
            fflush(out);
        }

        // Clean up this connection
        fclose(in);
        fclose(out);
        printf("Client disconnected\n");
    }

    // Close the listening socket 
    close(serv_sock);
    return 0;
}
