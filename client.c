#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "utils.h"
#include "client.h"

void join_server(const char *server_ip, int port, const char *username) {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[MAX_MSG_LEN];

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) handle_error("[Client] socket");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) handle_error("[Client] Invalid address");

    printf("[Client] Connecting to %s:%d...\n", server_ip, port);
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) handle_error("[Client] connect");
    
    printf("[Client] Connected to server. Sending username '%s'...\n", username);
    if (send(sock_fd, username, strlen(username), 0) == -1) handle_error("[Client] send username");

    printf("--- Chat session started. You can now send messages. ---\n");

    fd_set read_fds;
    int max_fd = (STDIN_FILENO > sock_fd) ? STDIN_FILENO : sock_fd;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(sock_fd, &read_fds);

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) handle_error("[Client] select");

        if (FD_ISSET(sock_fd, &read_fds)) {
            int bytes_received = recv(sock_fd, buffer, MAX_MSG_LEN - 1, 0);
            if (bytes_received <= 0) {
                printf("--- Server disconnected. ---\n");
                break;
            }
            buffer[bytes_received] = '\0';
            printf("%s", buffer);
            fflush(stdout);
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buffer, MAX_MSG_LEN, stdin) != NULL) {
                if (send(sock_fd, buffer, strlen(buffer), 0) == -1) {
                    perror("[Client] send");
                    break;
                }
                printf("\033[A\033[2K\r");
                fflush(stdout);
            } else break;
        }
    }
    close(sock_fd);
}
