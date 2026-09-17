#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define SIZE 1024
struct DNS
{
    char domain[50];
    char ip[20];
};
int main()
{
    int server;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen; // Reset inside the loop for each client
    char domain[SIZE];
    char ip[SIZE];
    int i, found, port;
    struct DNS table[] =
    {
        {"google.com", "142.250.195.14"},
        {"facebook.com", "157.240.241.35"},
        {"instagram.com", "157.240.241.174"},
        {"youtube.com", "142.250.72.14"},
        {"yahoo.com", "98.137.11.163"},
        {"gmail.com", "142.250.195.18"}
    };
    int count = sizeof(table) / sizeof(table[0]);
    printf("Enter Port number to bind DNS server (e.g., 8082): ");
    if (scanf("%d", &port) != 1) {
        printf("Invalid port input.\n");
        return 1;
    }
    getchar();
    server = socket(AF_INET, SOCK_DGRAM, 0);
    if(server < 0)
    {
        perror("Socket creation failed");
        return 1;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if(bind(server, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        close(server);
        return 1;
    }
    printf("DNS Server is running on port %d...\n", port);
    while(1)
    {
        clientLen = sizeof(clientAddr); // Reset size for every new incoming message
        memset(domain, 0, SIZE);
        
        recvfrom(server, domain, SIZE - 1, 0,(struct sockaddr *)&clientAddr,
                 &clientLen);       
        printf("\nRequested Domain from [%s:%d]: %s\n", 
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), domain);
        if(strcmp(domain, "exit") == 0)
        {
            strcpy(ip, "exit");
            sendto(server, ip, strlen(ip), 0,(struct sockaddr *)&clientAddr,
                   clientLen);
            printf("Client disconnected.\n");
            continue; // Go back to waiting for other clients
        }        
        found = 0;
        for(i = 0; i < count; i++)
        {
            if(strcmp(domain, table[i].domain) == 0)
            {
                strcpy(ip, table[i].ip);
                found = 1;
                break;
            }
        }        
        if(found == 0)
            strcpy(ip, "Domain not found");           
        printf("IP Address sent: %s\n", ip);       
        sendto(server, ip, strlen(ip), 0,(struct sockaddr *)&clientAddr,
               clientLen);
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
    socklen_t serverLen = sizeof(serverAddr);
    char domain[SIZE];
    char ip[SIZE];
    char ip_address[50];
    int port;
    printf("Enter DNS Server IP Address (e.g., 127.0.0.1): ");
    scanf("%49s", ip_address);
    printf("Enter DNS Server Port Number (e.g., 8082): ");
    if (scanf("%d", &port) != 1) {
        printf("Invalid port input.\n");
        return 1;
    }
    getchar();    
    client = socket(AF_INET, SOCK_DGRAM, 0);
    if(client < 0)
    {
        perror("Socket creation failed");
        return 1;
    }    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip_address);    
    printf("\nDNS Client Started. Target: %s:%d\n", ip_address, port);   
    while(1)
    {
        printf("\nEnter domain: ");
        scanf("%s", domain);        
        sendto(client, domain, strlen(domain), 0,(struct sockaddr *)&serverAddr,
               serverLen);               
        memset(ip, 0, SIZE);
        recvfrom(client, ip, SIZE - 1, 0,(struct sockaddr *)&serverAddr,
                 &serverLen);                 
        printf("DNS Response: %s\n", ip);        
        if(strcmp(domain, "exit") == 0)
            break;
    }
    close(client);
    return 0;
}

