#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "utils.h"
#include "server.h"

void start_server(int port) {
    int server_sock_fd;
    int client_sock_fd[MAX_CLIENTS];
    char client_usernames[MAX_CLIENTS][MAX_USERNAME_LEN];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[MAX_MSG_LEN];
    int client_count = 0;

    printf("[Server] Starting server on port %d...\n", port);

    if ((server_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) handle_error("[Server] socket");

    int opt = 1;
    if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) handle_error("[Server] setsockopt");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) handle_error("[Server] bind");
    if (listen(server_sock_fd, MAX_CLIENTS) == -1) handle_error("[Server] listen");

    printf("[Server] Waiting for %d clients to connect...\n", MAX_CLIENTS);

    while (client_count < MAX_CLIENTS) {
        client_sock_fd[client_count] = accept(server_sock_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock_fd[client_count] == -1) handle_error("[Server] accept");
        
        printf("[Server] Client %d connected. Waiting for username...\n", client_count + 1);

        int bytes_received = recv(client_sock_fd[client_count], client_usernames[client_count], MAX_USERNAME_LEN - 1, 0);
        if (bytes_received <= 0) {
            printf("[Server] Failed to get username. Disconnecting client %d.\n", client_count + 1);
            close(client_sock_fd[client_count]);
            continue;
        }
        client_usernames[client_count][bytes_received] = '\0';
        trim_newline(client_usernames[client_count]);
        printf("[Server] Client %d registered as '%s'.\n", client_count + 1, client_usernames[client_count]);
        client_count++;
    }

    printf("[Server] Both clients connected. Starting communication relay.\n");

    fd_set read_fds;
    int max_sd = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) if (client_sock_fd[i] > max_sd) max_sd = client_sock_fd[i];

    while (1) {
        FD_ZERO(&read_fds);
        int active_clients = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sock_fd[i] > 0) {
                FD_SET(client_sock_fd[i], &read_fds);
                active_clients++;
            }
        }

        if (active_clients == 0) break;

        if (select(max_sd + 1, &read_fds, NULL, NULL, NULL) < 0) handle_error("[Server] select");

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sock_fd[i] > 0 && FD_ISSET(client_sock_fd[i], &read_fds)) {
                int bytes_received = recv(client_sock_fd[i], buffer, MAX_MSG_LEN - 1, 0);
                if (bytes_received <= 0) {
                    printf("[Server] '%s' disconnected.\n", client_usernames[i]);
                    close(client_sock_fd[i]);
                    client_sock_fd[i] = 0;
                } else {
                    buffer[bytes_received] = '\0';
                    char prefixed_msg[MAX_MSG_LEN + MAX_USERNAME_LEN + 3];
                    snprintf(prefixed_msg, sizeof(prefixed_msg), "%s: %s", client_usernames[i], buffer);
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_sock_fd[j] > 0) {
                           if (send(client_sock_fd[j], prefixed_msg, strlen(prefixed_msg), 0) == -1) perror("[Server] send");
                        }
                    }
                }
            }
        }
    }

    printf("[Server] All clients disconnected. Server shutting down.\n");
    close(server_sock_fd);
}
