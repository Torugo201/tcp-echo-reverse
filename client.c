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


// Buffer size for stdin and socket I/O
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
 * Main client entry point.
 * Usage: ./client <server-IP> <port>
 * Connects to the server, sends one line from stdin,
 * receives reversed data, prints it, and exits.
 */
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server-IP> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // Create a TCP socket (IPv4)
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        die("socket");
    }

    // Specify server address
    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;                // IPv4
    serv_addr.sin_port = htons(port);              // port in network byte order
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        die("inet_pton");
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        die("connect");
    }
    printf("Connected to %s:%d\n", server_ip, port);

    // Wrap socket in FILE* streams for fread/fwrite
    FILE *out = fdopen(sock, "w");      // to send data
    FILE *in  = fdopen(dup(sock), "r"); // to receive data
    if (!in || !out) {
        die("fdopen");
    }

    // Read one line from stdin
    char buf[BUF_SIZE];
    if (fgets(buf, BUF_SIZE, stdin) == NULL) {
        fprintf(stderr, "No input provided\n");
        exit(EXIT_FAILURE);
    }
    size_t len = strlen(buf);

    // Send the input line to the server
    if (fwrite(buf, 1, len, out) != len) {
        die("fwrite to socket");
    }
    fflush(out);

    // Signal end-of-file to server (no more data)
    shutdown(sock, SHUT_WR);

    // Read reversed response and print to stdout
    ssize_t n;
    while ((n = fread(buf, 1, BUF_SIZE, in)) > 0) {
        if (fwrite(buf, 1, n, stdout) != n) {
            die("fwrite to stdout");
        }
        fflush(stdout);
    }

    // Clean up
    fclose(in);
    fclose(out);
    printf("Connection closed\n");

    return 0;
}
