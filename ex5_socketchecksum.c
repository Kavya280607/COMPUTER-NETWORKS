#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define PORT 9000
#define SIZE 4096
void printBinary(char str[])
{
    int i;
    for(i = 0; str[i] != '\0'; i++)
    {
        printf("%c", str[i]);
        if((i + 1) % 8 == 0)
            printf(" ");
    }
    printf("\n");
}
int binaryToDecimal(char b[])
{
    int i, n = 0;
    for(i = 0; i < 8; i++)
    {
        n = n * 2;
        if(b[i] == '1')
            n++;
    }
    return n;
}
void decimalToBinary(int n, char b[])
{
    int i;
    for(i = 7; i >= 0; i--)
    {
        b[i] = (n % 2) + '0';
        n = n / 2;
    }
    b[8] = '\0';
}
void destuff(char input[], char output[])
{
    int i, j, k = 0;
    char byte[9];
    for(i = 0; input[i] != '\0'; i += 8)
    {
        for(j = 0; j < 8; j++)
            byte[j] = input[i + j];
        byte[8] = '\0';
        if(strcmp(byte, "01111101") == 0)
        {
            i += 8;
            for(j = 0; j < 8; j++)
                output[k++] = input[i + j];
        }
        else
        {
            for(j = 0; j < 8; j++)
                output[k++] = byte[j];
        }
    }
    output[k] = '\0';
}
int verify(char data[], char received[])
{
    int i, j, sum = 0, value;
    char byte[9];
    for(i = 0; data[i] != '\0'; i += 8)
    {
        for(j = 0; j < 8; j++)
            byte[j] = data[i + j];
        byte[8] = '\0';
        value = binaryToDecimal(byte);
        sum += value;
        if(sum > 255)
            sum = (sum & 255) + (sum >> 8);
    }
    value = binaryToDecimal(received);
    sum += value;
    if(sum > 255)
        sum = (sum & 255) + (sum >> 8);

    return sum;
}
void binaryToText(char binary[], char text[])
{
    int i, j, value, k = 0;
    for(i = 0; binary[i] != '\0'; i += 8)
    {
        value = 0;
        for(j = 0; j < 8; j++)
        {
            value = value * 2;
            if(binary[i + j] == '1')
                value++;
        }
        text[k++] = value;
    }
    text[k] = '\0';
}
int main(int argc, char *argv[])
{
    int serverSocket, clientSocket;
    int len, i, result;
    int port;
    char frame[SIZE];      
    char stuffed[SIZE];    
    char destuffed[SIZE];
    char data[SIZE];
    char checksum[9];
    char text[SIZE];
    char resultBinary[9];
    struct sockaddr_in server, client;
    socklen_t clientSize = sizeof(client);
    if(argc != 2)
    {
        printf("Usage: %s <Port Number>\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0)
    {
        perror("Socket error");
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if(bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind error");
        return 1;
    }
    listen(serverSocket, 5);
    printf("\n===== TCP CHECKSUM SERVER =====\n");
    printf("Waiting for client...\n");
    clientSocket = accept(serverSocket,(struct sockaddr *)&client,
                          &clientSize);
    if(clientSocket < 0)
    {
        perror("Accept error");
        return 1;
    }
    printf("Client connected.\n");
    recv(clientSocket, &len, sizeof(len), 0);
    len = ntohl(len);
    recv(clientSocket, frame, len, 0);
    frame[len] = '\0';
    printf("\nReceived PPP Frame:\n");
    printBinary(frame);
    int payloadLen = len - 32; 
    for(i = 0; i < payloadLen; i++)
    {
        stuffed[i] = frame[24 + i];
    }
    stuffed[payloadLen] = '\0';
    printf("\nExtracted Stuffed Data payload:\n");
    printBinary(stuffed);
    destuff(stuffed, destuffed);
    printf("\nAfter Destuffing:\n");
    printBinary(destuffed);
    for(i = 0; i < 8; i++)
        checksum[i] = destuffed[strlen(destuffed) - 8 + i];
    checksum[8] = '\0';
    for(i = 0; i < strlen(destuffed) - 8; i++)
        data[i] = destuffed[i];
    data[strlen(destuffed) - 8] = '\0';
    printf("\nOriginal Data:\n");
    printBinary(data);
    printf("\nReceived Checksum:\n");
    printf("%s\n", checksum);
    result = verify(data, checksum);
    decimalToBinary(result, resultBinary);
    printf("\nChecksum Verification:\n");
    printf("Result = %s\n", resultBinary);
    if(strcmp(resultBinary, "11111111") == 0)
        printf("CHECKSUM VALID - NO ERROR\n");
    else
        printf("CHECKSUM INVALID - ERROR DETECTED\n");
    binaryToText(data, text);
    printf("\nReceived Message: %s\n", text);
    close(clientSocket);
    close(serverSocket);
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define PORT 9000
#define SIZE 4096
void charToBinary(char ch, char b[])
{
    int i, n = ch;
    for(i = 7; i >= 0; i--)
    {
        b[i] = (n % 2) + '0';
        n = n / 2;
    }
    b[8] = '\0';
}
void stringToBinary(char text[], char binary[])
{
    int i;
    char temp[9];
    binary[0] = '\0';
    for(i = 0; text[i] != '\0'; i++)
    {
        charToBinary(text[i], temp);
        strcat(binary, temp);
    }
}
void printBinary(char str[])
{
    int i;
    for(i = 0; str[i] != '\0'; i++)
    {
        printf("%c", str[i]);
        if((i + 1) % 8 == 0)
            printf(" ");
    }
    printf("\n");
}
int binaryToDecimal(char b[])
{
    int i, n = 0;
    for(i = 0; i < 8; i++)
    {
        n = n * 2;
        if(b[i] == '1')
            n++;
    }
    return n;
}
void decimalToBinary(int n, char b[])
{
    int i;
    for(i = 7; i >= 0; i--)
    {
        b[i] = (n % 2) + '0';
        n = n / 2;
    }
    b[8] = '\0';
}
int calculateChecksum(char data[])
{
    int i, j, sum = 0, value;
    char byte[9];
    for(i = 0; data[i] != '\0'; i += 8)
    {
        for(j = 0; j < 8; j++)
            byte[j] = data[i + j];

        byte[8] = '\0';
        value = binaryToDecimal(byte);
        sum += value;
        if(sum > 255)
            sum = (sum & 255) + (sum >> 8);
    }
    return 255 - sum;
}
void stuffData(char data[], char stuffed[])
{
    int i, j, k = 0;
    char byte[9];
    for(i = 0; data[i] != '\0'; i += 8)
    {
        for(j = 0; j < 8; j++)
            byte[j] = data[i + j];
        byte[8] = '\0';
        if(strcmp(byte, "01111110") == 0)
        {
            strcpy(&stuffed[k], "01111101");
            k += 8;
            strcpy(&stuffed[k], "01111110");
            k += 8;
        }
        else if(strcmp(byte, "01111101") == 0)
        {
            strcpy(&stuffed[k], "01111101");
            k += 8;
            strcpy(&stuffed[k], "01111101");
            k += 8;
        }
        else
        {
            for(j = 0; j < 8; j++)
                stuffed[k++] = byte[j];
        }
    }
    stuffed[k] = '\0';
}
int main(int argc, char *argv[])
{
    int sock;
    int checksum, len, sendLen, choice, position;
    int port;
    char message[SIZE];
    char binary[SIZE];
    char checksumBinary[9];
    char combined[SIZE];
    char frame[SIZE];
    char stuffed[SIZE];
    char flag[9] = "01111110";
    char addr[9] = "11111111";
    char ct[9] = "00000011";
    struct sockaddr_in server;
    if(argc != 3)
    {
        printf("Usage: %s <IP Address> <Port Number>\n", argv[0]);
        return 1;
    }
    port = atoi(argv[2]);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("Socket error");
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);
    if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connection error");
        return 1;
    }
    printf("\n===== TCP CHECKSUM CLIENT =====\n");
    printf("\nEnter message: ");
    fgets(message, SIZE, stdin);
    message[strcspn(message, "\n")] = '\0';
    printf("\nOriginal Message:\n%s\n", message);
    stringToBinary(message, binary);
    printf("\nBinary Data:\n");
    printBinary(binary);
    checksum = calculateChecksum(binary);
    decimalToBinary(checksum, checksumBinary);
    printf("\nChecksum:\n%s\n", checksumBinary);
    strcpy(combined, binary);
    strcat(combined, checksumBinary);
    printf("\nDo you want to change any bit? (1-Yes / 0-No): ");
    scanf("%d", &choice);
    if(choice == 1)
    {
        printf("Enter bit position to change (1-%d): ",
               (int)strlen(combined));
        scanf("%d", &position);
        if(position >= 1 && position <= strlen(combined))
        {
            if(combined[position - 1] == '0')
                combined[position - 1] = '1';
            else
                combined[position - 1] = '0';

            printf("\nData after changing bit:\n");
            printBinary(combined);
        }
        else
        {
            printf("Invalid position.\n");
        }
    }
    printf("\nData + Checksum:\n");
    printBinary(combined);
    stuffData(combined, stuffed);
    printf("\nByte Stuffed Data:\n");
    printBinary(stuffed);
    strcpy(frame, flag);
    strcat(frame, addr);
    strcat(frame, ct);
    strcat(frame, stuffed);
    strcat(frame, flag);
    printf("\nPPP Frame:\n");
    printBinary(frame);
    len = strlen(frame);
    sendLen = htonl(len);
    send(sock, &sendLen, sizeof(sendLen), 0);
    send(sock, frame, len, 0);
    printf("\nData sent to server successfully.\n");
    close(sock);
    return 0;
}

