#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define PORT 8086
#define MAX_SUBNETS 20
#define SIZE 4096
struct Request
{
    int hosts;
    int subnets;
    int requestedSubnet;
};
struct Subnet
{
    char network[20];
    char first[20];
    char last[20];
    char broadcast[20];
    int assigned;
};
struct Subnet pool[MAX_SUBNETS];
int poolCreated = 0;
int totalSubnets = 0;
int newPrefix;
void makeIP(unsigned int ip, char result[])
{
    sprintf(result, "%u.%u.%u.%u",
            (ip >> 24) & 255,
            (ip >> 16) & 255,
            (ip >> 8) & 255,
            ip & 255);
}
void createPool(int hosts, int subnets)
{
    int addresses = 1;
    int bits = 0;
    int i;
    while (addresses < hosts + 2)
    {
        addresses = addresses * 2;
        bits++;
    }
    newPrefix = 32 - bits;
    unsigned int blockSize = 1U << bits;
    unsigned int baseIP = 0xC0A80100; 
    totalSubnets = subnets;
    for (i = 0; i < subnets; i++)
    {
        unsigned int network;
        unsigned int first;
        unsigned int last;
        unsigned int broadcast;
        network = baseIP + (i * blockSize);
        first = network + 1;
        broadcast = network + blockSize - 1;
        last = broadcast - 1;
        makeIP(network, pool[i].network);
        makeIP(first, pool[i].first);
        makeIP(last, pool[i].last);
        makeIP(broadcast, pool[i].broadcast);
        pool[i].assigned = 0;
    }
    poolCreated = 1;
}
void displayStatus()
{
    int i;
    printf("\n============================================\n");
    printf(" CURRENT SUBNET STATUS\n");
    printf("============================================\n");
    for (i = 0; i < totalSubnets; i++)
    {
        if (pool[i].assigned == 1)
            printf("Subnet %d : NOT AVAILABLE\n", i + 1);
        else
            printf("Subnet %d : AVAILABLE\n", i + 1);
    }
    printf("============================================\n");
}
int main()
{
    int server;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t len;
    struct Request req;
    char response[SIZE];
    server = socket(AF_INET, SOCK_DGRAM, 0);
    if (server < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server,
             (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        close(server);
        exit(1);
    }
    printf("============================================\n");
    printf(" DHCP SERVER\n");
    printf("============================================\n\n");
    printf("DHCP Server socket created successfully\n");
    printf("DHCP Server started successfully\n");
    printf("Port : %d\n", PORT);
    printf("Network : 192.168.1.0/24\n");
    printf("Mode : ITERATIVE UDP\n");
    printf("Waiting for DHCP Client...\n");
    while (1)
    {
        len = sizeof(clientAddr);
        memset(&req, 0, sizeof(req));
        recvfrom(server,
                 &req,
                 sizeof(req),
                 0,
                 (struct sockaddr *)&clientAddr,
                 &len);
        printf("\n\n============================================\n");
        printf(" CLIENT CONNECTED\n");
        printf("============================================\n");
        printf("Client IP : %s\n",
               inet_ntoa(clientAddr.sin_addr));
        printf("Client Port : %d\n",
               ntohs(clientAddr.sin_port));
        printf("\n--------------------------------------------\n");
        printf(" DHCP REQUEST INFORMATION\n");
        printf("--------------------------------------------\n");
        printf("Number of Hosts : %d\n", req.hosts);
        printf("Original Prefix : /24\n");
        printf("Number of Subnets : %d\n", req.subnets);
        if (poolCreated == 0)
        {
            int addresses = 1;
            int bits = 0;
            int i;
            while (addresses < req.hosts + 2)
            {
                addresses = addresses * 2;
                bits++;
            }
            newPrefix = 32 - bits;
            unsigned int blockSize = 1U << bits;
            if (req.subnets * blockSize > 256)
            {
                sprintf(response,
                        "ERROR: Requested subnets cannot fit inside 192.168.1.0/24.");
                sendto(server,
                       response,
                       strlen(response) + 1,
                       0,
                       (struct sockaddr *)&clientAddr,
                       len);
                printf("\n%s\n", response);
                continue;
            }
            printf("\nCalculating Address Requirement...\n");
            printf("Requested Hosts : %d\n", req.hosts);
            printf("Required Addresses : %d\n", addresses);
            printf("New Prefix : /%d\n", newPrefix);
            createPool(req.hosts, req.subnets);
            printf("\n============================================\n");
            printf(" INITIAL SUBNET POOL\n");
            printf("============================================\n");
            for (i = 0; i < totalSubnets; i++)
            {
                printf("\nSubnet %d : AVAILABLE\n",
                       i + 1);
                printf(" Network : %s\n",
                       pool[i].network);
                printf(" First IP : %s\n",
                       pool[i].first);
                printf(" Last IP : %s\n",
                       pool[i].last);
                printf(" Broadcast : %s\n",
                       pool[i].broadcast);
            }
            printf("============================================\n");
        }
        else
        {
            printf("\nAddress pool already exists.\n");
            printf("Using existing subnet allocation table.\n");
        }
        printf("\n--------------------------------------------\n");
        printf(" SUBNET REQUEST\n");
        printf("--------------------------------------------\n");
        printf("Requested Subnet : %d\n",
               req.requestedSubnet);
        if (req.requestedSubnet < 1 ||
            req.requestedSubnet > totalSubnets)
        {
            sprintf(response,
                    "ERROR: Invalid subnet number!");
            printf("\n%s\n", response);
            sendto(server,
                   response,
                   strlen(response) + 1,
                   0,
                   (struct sockaddr *)&clientAddr,
                   len);
            continue;
        }
        int index = req.requestedSubnet - 1;
        if (pool[index].assigned == 1)
        {
            sprintf(response,
                    "ERROR: Subnet %d is already assigned!\n"
                    "DHCP Request rejected.\n"
                    "This subnet is NOT AVAILABLE.",
                    req.requestedSubnet);
            printf("\n%s\n", response);
            sendto(server,
                   response,
                   strlen(response) + 1,
                   0,
                   (struct sockaddr *)&clientAddr,
                   len);
        }
        else
        {
            pool[index].assigned = 1;
            printf("\n============================================\n");
            printf(" DHCP ADDRESS ASSIGNED\n");
            printf("============================================\n");
            printf("Subnet Number : %d\n",
                   req.requestedSubnet);
            printf("Network Address : %s\n",
                   pool[index].network);
            printf("First Address : %s\n",
                   pool[index].first);
            printf("Last Address : %s\n",
                   pool[index].last);
            printf("Broadcast Address : %s\n",
                   pool[index].broadcast);
            printf("Status : ASSIGNED\n");
            printf("\nDHCP ACK Sent\n");
            sprintf(response,
                    "DHCP ACK\n"
                    "Subnet %d assigned successfully.\n"
                    "Network : %s\n"
                    "First IP : %s\n"
                    "Last IP : %s\n"
                    "Broadcast : %s",
                    req.requestedSubnet,
                    pool[index].network,
                    pool[index].first,
                    pool[index].last,
                    pool[index].broadcast);
            sendto(server,
                   response,
                   strlen(response) + 1,
                   0,
                   (struct sockaddr *)&clientAddr,
                   len);
        }
        printf("\nSubnet Status after request:\n");
        displayStatus();
        printf("\nClient completed successfully.\n");
        printf("--------------------------------------------\n");
        printf("Client disconnected.\n");
        printf("Server is ready for next client.\n");
        printf("--------------------------------------------\n");
    }
    close(server);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8086
#define SIZE 4096
struct Request
{
    int hosts;
    int subnets;
    int requestedSubnet;
};
int main()
{
    int client;
    struct sockaddr_in serverAddr;
    struct Request req;
    char response[SIZE];
    client = socket(AF_INET, SOCK_DGRAM, 0);
    if (client < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr =
        inet_addr("172.16.16.200");
    printf("============================================\n");
    printf(" DHCP CLIENT\n");
    printf("============================================\n\n");
    printf("Enter Number of Hosts : ");
    scanf("%d", &req.hosts);
    printf("Enter Number of Subnets : ");
    scanf("%d", &req.subnets);
    printf("Enter Requested Subnet : ");
    scanf("%d", &req.requestedSubnet);
    sendto(client,
           &req,
           sizeof(req),
           0,
           (struct sockaddr *)&serverAddr,
           sizeof(serverAddr));
    memset(response, 0, SIZE);
    recvfrom(client,
             response,
             SIZE - 1,
             0,
             NULL,
             NULL);
    printf("\n============================================\n");
    printf(" DHCP RESPONSE\n");
    printf("============================================\n");
    printf("%s\n", response);
    close(client);
    return 0;
}


