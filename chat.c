#include <arpa/inet.h>
#include <sys/wait.h>
#include "utils.h"
#include "client.h"
#include "server.h"

int main() {
    char choice_buf[10];
    char port_buf[10];
    char username_buf[MAX_USERNAME_LEN];
    char ip_buf[INET_ADDRSTRLEN];
    int choice;
    int port;

    printf("Welcome to C-Chat!\n");
    printf("1. Start a new chat (you will be the host)\n");
    printf("2. Join an existing chat\n");
    printf("Enter your choice (1 or 2): ");
    
    fgets(choice_buf, sizeof(choice_buf), stdin);
    choice = atoi(choice_buf);

    if (choice == 1) { // Start a new server
        printf("Enter a port to host on (e.g., 12345): ");
        fgets(port_buf, sizeof(port_buf), stdin);
        port = atoi(port_buf);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number.\n");
            return 1;
        }

        printf("Enter your username: ");
        fgets(username_buf, sizeof(username_buf), stdin);
        trim_newline(username_buf);

        printf("\n--------------------------------------------------------------\n");
        printf("Attempting to find your Public IPv4 address...\n");
        printf("Your friend should use this IP to connect.\n");
        printf("(This requires 'curl' to be installed on your system.)\n\n");
        fflush(stdout);
        system("curl -4 -s ifconfig.me ; echo");
        printf("\nNOTE: You may still need to configure port forwarding on your router for port %d.\n", port);
        printf("--------------------------------------------------------------\n\n");

        pid_t pid = fork();
        if (pid == -1) {
            handle_error("fork");
        } else if (pid == 0) {
            // Child process: Become the first client
            sleep(1);
            join_server("127.0.0.1", port, username_buf);
            exit(0);
        } else {
            // Parent process: Become the server
            start_server(port);
            wait(NULL);
        }

    } else if (choice == 2) { // Join an existing server
        printf("Enter the server IP address: ");
        fgets(ip_buf, sizeof(ip_buf), stdin);
        trim_newline(ip_buf);
        
        printf("Enter the server port: ");
        fgets(port_buf, sizeof(port_buf), stdin);
        port = atoi(port_buf);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number.\n");
            return 1;
        }

        printf("Enter your username: ");
        fgets(username_buf, sizeof(username_buf), stdin);
        trim_newline(username_buf);

        join_server(ip_buf, port, username_buf);

    } else {
        fprintf(stderr, "Invalid choice. Please run the program again.\n");
        return 1;
    }

    return 0;
}
