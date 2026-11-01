Server

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8081
#define BUFFER_SIZE 100
int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    char str[BUFFER_SIZE];
    char result[100];
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&server,
         sizeof(server));
    listen(server_fd, 5);
    printf("Palindrome Server Waiting...\n");
    client_fd = accept(server_fd,
                (struct sockaddr *)&client, &len);
    recv(client_fd, str, sizeof(str) - 1, 0);
    str[strcspn(str, "\n")] = '\0';
    int i, j;
    int flag = 1;
    i = 0;
    j = strlen(str) - 1;
    while (i < j)
    {
        if (str[i] != str[j])
        {
            flag = 0;
            break;
        }
        i++;
        j--;
    }
    if (flag == 1)
        strcpy(result, "Palindrome");
    else
        strcpy(result, "Not a Palindrome");

    send(client_fd, result, strlen(result) + 1, 0);
    printf("Received String: %s\n", str);
    printf("Result: %s\n", result);
    close(client_fd);
    close(server_fd);
    return 0;
}


Client

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8081
#define BUFFER_SIZE 100
int main()
{
    int sock;
    struct sockaddr_in server;
    char str[BUFFER_SIZE];
    char result[BUFFER_SIZE];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr *)&server,
            sizeof(server));
    printf("Enter a string: ");
    fgets(str, sizeof(str), stdin);
    send(sock, str, strlen(str) + 1, 0);
    recv(sock, result, sizeof(result), 0);
    printf("Server Response: %s\n", result);
    close(sock);
    return 0;
}