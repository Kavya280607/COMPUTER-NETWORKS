#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define PORT 8080
#define SIZE 1024
int main()
{
    int server;
    struct sockaddr_in serverAddr, clientAddr, activeClientAddr;
    socklen_t clientLen;
    char message[SIZE];
    char reply[SIZE];
    int has_active_client = 0; // Tracks if a client is currently locked in chat
    server = socket(AF_INET, SOCK_DGRAM, 0);
    if (server < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server, (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }
    printf("========== CHAT SERVER ==========\n");
    printf("Server running on port %d\n", PORT);
    printf("Waiting for clients...\n\n");
    while (1)
    {
        clientLen = sizeof(clientAddr);
        memset(message, 0, SIZE);
        recvfrom(server, message, SIZE - 1, 0,
                 (struct sockaddr *)&clientAddr,
                 &clientLen);
        if (!has_active_client)
        {
            activeClientAddr = clientAddr;
            has_active_client = 1;
            printf("\n--- New Session Started ---\n");
        }
        else if (clientAddr.sin_addr.s_addr != activeClientAddr.sin_addr.s_addr || 
                 clientAddr.sin_port != activeClientAddr.sin_port)
        {
            char busy_msg[] = "Server is busy with another client. Try again later.";
            sendto(server, busy_msg, strlen(busy_msg), 0,
                   (struct sockaddr *)&clientAddr, clientLen);
            continue; // Skip processing and wait for the active client
        }
        printf("\nClient [%s:%d]: %s\n",
               inet_ntoa(activeClientAddr.sin_addr),
               ntohs(activeClientAddr.sin_port),
               message);
        if (strcmp(message, "bye") == 0)
        {
            strcpy(reply, "bye");
            sendto(server, reply, strlen(reply), 0,
                   (struct sockaddr *)&activeClientAddr,
                   clientLen);
            printf("Client left the chat.\n");
            has_active_client = 0;
            printf("\nWaiting for new clients...\n\n");
            continue;
        }
        printf("Server: ");
        fgets(reply, SIZE, stdin);
        reply[strcspn(reply, "\n")] = '\0';
        sendto(server, reply, strlen(reply), 0,
               (struct sockaddr *)&activeClientAddr,
               clientLen);
        if (strcmp(reply, "bye") == 0)
        {
            printf("Chat ended with this client.\n");
            has_active_client = 0; 
            printf("\nWaiting for new clients...\n\n");
        }
    }
    close(server);
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define SIZE 1024
int main()
{
    int client;
    struct sockaddr_in serverAddr;
    socklen_t serverLen;
    char message[SIZE];
    char ip_address[50];
    int port;
    printf("Enter Server IP Address: ");
    scanf("%49s", ip_address);
    printf("Enter Server Port Number: ");
    scanf("%d", &port);
    getchar();
    client = socket(AF_INET, SOCK_DGRAM, 0);
    if (client < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip_address);
    serverLen = sizeof(serverAddr);
    printf("\nConnected to server %s:%d\n", ip_address, port);
    printf("Type 'bye' to exit.\n\n");
    while (1)
    {
        printf("Client: ");
        fgets(message, SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';
        if (strlen(message) == 0)
            continue;
        sendto(client, message, strlen(message), 0,
               (struct sockaddr *)&serverAddr,
               serverLen);
        if (strcmp(message, "bye") == 0)
            break;
        memset(message, 0, SIZE);
        recvfrom(client, message, SIZE - 1, 0,
                 (struct sockaddr *)&serverAddr,
                 &serverLen);

        printf("Server: %s\n", message);
        if (strcmp(message, "bye") == 0 || strcmp(message, "Server is busy with another client. Try again later.") == 0)
            break;
    }
    close(client);
    return 0;
}

