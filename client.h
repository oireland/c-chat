#ifndef CLIENT_H
#define CLIENT_H

/**
 * @brief Connects to a server, sends a username, and handles the chat session.
 * * @param server_ip The IP address of the server to connect to.
 * @param port The port number of the server.
 * @param username The username to register with the server.
 */
void join_server(const char *server_ip, int port, const char *username);

#endif // CLIENT_H
