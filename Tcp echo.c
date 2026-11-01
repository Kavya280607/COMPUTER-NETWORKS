Server

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024
int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    char buffer[BUFFER_SIZE];
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    listen(server_fd, 5);
    printf("Echo Server Waiting...\n");
    client_fd = accept(server_fd,
                (struct sockaddr *)&client, &len);
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(client_fd, buffer,
                     sizeof(buffer) - 1, 0);
        if (n <= 0)
            break;
        buffer[n] = '\0';
        printf("Client: %s\n", buffer);
        send(client_fd, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "exit") == 0)
            break;
    }
    close(client_fd);
    close(server_fd);
    return 0;
}


Client

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024
int main()
{
    int sock;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr *)&server,
            sizeof(server));
    while (1)
    {
        printf("Enter message: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        int n = recv(sock, buffer,
                     sizeof(buffer) - 1, 0);
        if (n <= 0)
            break;
        buffer[n] = '\0';
        printf("Server Echo: %s\n", buffer);
        if (strcmp(buffer, "exit") == 0)
            break;
    }
    close(sock);
    return 0;
}