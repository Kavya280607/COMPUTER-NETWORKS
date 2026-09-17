#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define MAX 100
#define SERV_PORT 8081
int main()
{
    int listenSocket;
    int clientSocket;
    int array[MAX];
    int count;
    int i;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("172.16.16.200");
    serverAddress.sin_port = htons(SERV_PORT);
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listenSocket < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    if(bind(listenSocket,
            (struct sockaddr *)&serverAddress,
            sizeof(serverAddress)) < 0)
    {
        perror("Binding failed");
        exit(1);
    }
    if(listen(listenSocket, 5) < 0)
    {
        perror("Listening failed");
        exit(1);
    }
    printf("Array Transfer Server started...\n");
    printf("Waiting for client...\n");
    clientAddressLength = sizeof(clientAddress);
    clientSocket = accept(listenSocket,(struct sockaddr *)&clientAddress,
                          &clientAddressLength);
    if(clientSocket < 0)
    {
        perror("Accept failed");
        exit(1);
    }
    printf("Client connected.\n");
    recv(clientSocket, &count, sizeof(count), 0);
    recv(clientSocket, array, sizeof(int) * count, 0);
    printf("\nArray received from client:\n");
    for(i = 0; i < count; i++)
    {
        printf("%d", array[i]);

        if(i < count - 1)
            printf(",");
    }
    printf("\n");
    send(clientSocket, &count, sizeof(count), 0);
    send(clientSocket, array, sizeof(int) * count, 0);
    printf("Array sent back to client.\n");
    close(clientSocket);
    close(listenSocket);
    printf("Server closed.\n");
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define MAX 100
#define SERV_PORT 8081
int main()
{
    int clientSocket;
    int array[MAX];
    int count;
    int i;
    struct sockaddr_in serverAddress;
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("172.16.16.200");
    serverAddress.sin_port = htons(SERV_PORT);
    if(connect(clientSocket,(struct sockaddr *)&serverAddress,
               sizeof(serverAddress)) < 0)
    {
        perror("Connection failed");
        exit(1);
    }
    printf("Connected to TCP Server.\n");
    printf("Enter number of elements: ");
    scanf("%d", &count);
    printf("Enter %d elements separated by comma: ", count);
    for(i = 0; i < count; i++)
    {
        scanf("%d", &array[i]);

        if(i < count - 1)
            scanf(",");
    }
    send(clientSocket, &count, sizeof(count), 0);
    send(clientSocket, array, sizeof(int) * count, 0);
    printf("\nArray sent to server.\n");
    recv(clientSocket, &count, sizeof(count), 0);
    recv(clientSocket, array, sizeof(int) * count, 0);
    printf("\nArray received from server:\n");
    for(i = 0; i < count; i++)
    {
        printf("%d", array[i]);

        if(i < count - 1)
            printf(",");
    }
    printf("\n");
    close(clientSocket);
    printf("Client closed.\n");
    return 0;
}

