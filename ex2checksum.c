#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 1000
char flag[]="01111110";
char escape[]="01111101";
char message[100];
char binary[MAX]="";
char stuffed[MAX];
char frame[MAX];
void charToBinary(char ch, char bin[])
{
    int i;
    for(i=7; i>=0; i--)
    {
        if((ch>>i)&1)
            bin[7-i]='1';
        else
            bin[7-i]='0';
    }
    bin[8]='\0';
}
void messageToBinary(char msg[], char binary[])
{
    int i;
    char temp[9];
    binary[0]='\0';
    for(i=0; msg[i]!='\0' && msg[i]!='\n' && msg[i]!='\r'; i++)
    {
        charToBinary(msg[i], temp);
        strcat(binary, temp);
    }
}
void byteStuffing(char input[], char output[])
{
    int i=0, j=0;
    char byte[9];
    while(input[i] != '\0')
    {
        int k;
        for(k=0; k<8; k++)
            byte[k]=input[i+k];
        byte[8]='\0';
        if(strcmp(byte, flag)==0 || strcmp(byte, escape)==0)
        {
            strcpy(&output[j], escape);
            j+=8;
        }
        strcpy(&output[j], byte);
        j+=8;
        i+=8;
    }
    output[j]='\0';
}
unsigned short calculateChecksum(char stuffedData[])
{
    unsigned long sum = 0;
    int len = strlen(stuffedData);
    int i, bit;
    for (i = 0; i < len; i += 16)
    {
        unsigned short word = 0;
        for (bit = 0; bit < 16; bit++)
        {
            word <<= 1;
            if (i + bit < len && stuffedData[i + bit] == '1') {
                word |= 1;
            }
        }
        sum += word;
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (unsigned short)(~sum);
}
void checksumToBinaryString(unsigned short checksum, char outputStr[])
{
    int i;
    for (i = 15; i >= 0; i--) {
        outputStr[15 - i] = ((checksum >> i) & 1) ? '1' : '0';
    }
    outputStr[16] = '\0';
}
int main()
{
    FILE *fp;
    char checksumBinary[17];
    unsigned short checksumVal;
    int msgLen, stuffedLen, remainder, paddingNeeded, p;
    printf("Enter the message to send: ");
    fgets(message, 100, stdin);
    msgLen = strlen(message);
    if(message[msgLen-1] == '\n' || message[msgLen-1] == '\r') 
    {
        message[msgLen-1] = '\0';
    }
    fp = fopen("message.txt", "w");
    if(fp == NULL) { printf("Cannot write to message.txt\n"); return 0; }
    fputs(message, fp);
    fclose(fp);
    messageToBinary(message, binary);
    byteStuffing(binary, stuffed);
    stuffedLen = strlen(stuffed);
    remainder = stuffedLen % 16;
    if (remainder != 0) {
        paddingNeeded = 16 - remainder;
        for (p = 0; p < paddingNeeded; p++) {
            stuffed[stuffedLen + p] = '0';
    }
        stuffed[stuffedLen + paddingNeeded] = '\0';
    }

    printf("\nOriginal Message: %s\n", message);
    printf("Binary Data :\n%s\n", binary);
    printf("Stuffed Data:\n%s\n", stuffed);
    checksumVal = calculateChecksum(stuffed);
    checksumToBinaryString(checksumVal, checksumBinary);
    printf("Calculated Checksum: %s\n", checksumBinary);
    strcpy(frame, flag);
    strcat(frame, stuffed);
    strcat(frame, checksumBinary);
    strcat(frame, flag);
    printf("\nPPP Frame Layout:\n| %s | %s | %s | %s |\n", flag, stuffed, checksumBinary, flag);
    fp = fopen("frame.txt", "w");
    if(fp == NULL) { printf("Cannot create frame.txt\n"); return 0; }
    fputs(frame, fp);
    fclose(fp);
    fp = fopen("output.txt", "w");
    if(fp == NULL) { printf("Cannot create output.txt\n"); return 0; }
    fprintf(fp, "********** SENDER **********\n\n");
    fprintf(fp, "Original Message :\n%s\n", message);
    fprintf(fp, "Binary Data:\n%s\n\n", binary);
    fprintf(fp, "Stuffed Data:\n%s\n\n", stuffed);
    fprintf(fp, "Calculated Checksum : %s\n\n", checksumBinary);
    fprintf(fp, "PPP Frame:\n| %s | %s | %s | %s |\n", flag, stuffed, checksumBinary, flag);
    fclose(fp);
    printf("\nPPP Frame saved in frame.txt\n");
    printf("Sender output saved in output.txt\n");
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 1000
char flag[]="01111110";
char escape[]="01111101";
char frame[MAX];
char stuffed[MAX];
char receivedChecksum[17];
char destuffed[MAX];
char message[100];
void byteDestuffing(char input[], char output[])
{
    int i=0, j=0;
    char byte[9];
    while(input[i] != '\0')
    {
        int k;
        for(k=0; k<8; k++)
            byte[k]=input[i+k];
        byte[8]='\0';
        if(strcmp(byte, escape) == 0)
        {
            i = i + 8;
            for(k=0; k<8; k++)
                byte[k]=input[i+k];
            byte[8]='\0';
        }
        if (byte[0] == '\0') break;
        strcpy(&output[j], byte);
        j = j + 8;
        i = i + 8;
    }
    output[j]='\0';
}
void binaryToMessage(char binary[], char msg[])
{
    int i=0, j=0;
    while(binary[i] != '\0' && binary[i+7] != '\0')
    {
        int value=0;
        int k;
        for(k=0; k<8; k++)
        {
            value = value * 2 + (binary[i+k] - '0');
        }
        if (value == 0) break;
        msg[j] = (char)value;
        j++;
        i = i + 8;
    }
    msg[j]='\0';
}
unsigned short calculateActualChecksum(char stuffedData[], char checksumStr[])
{
    unsigned long sum = 0;
    char combined[MAX];
    int len, i, bit;
    strcpy(combined, stuffedData);
    strcat(combined, checksumStr);
    len = strlen(combined);
    for (i = 0; i < len; i += 16)
    {
        unsigned short word = 0;
        for (bit = 0; bit < 16; bit++)
        {
            word <<= 1;
            if (i + bit < len && combined[i + bit] == '1') {
                word |= 1;
            }
        }
        sum += word;
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return (unsigned short)(sum);
}
void intToBinary16(unsigned short value, char outputBits[])
{
    int i;
    for (i = 15; i >= 0; i--) {
        outputBits[i] = (value & 1) ? '1' : '0';
        value >>= 1;
    }
    outputBits[16] = '\0';
}
int main()
{
    FILE *fp;
    int i, j = 0, len, dataEndIndex;
    int simulateError, bitPos;
    int status;
    unsigned short actualChecksumValue;
    char checksumBinaryStr[17];
    fp = fopen("frame.txt", "r");
    if(fp == NULL) { printf("Cannot open frame.txt\n"); return 0; }
    fgets(frame, MAX, fp);
    fclose(fp);
    len = strlen(frame);
    while(len > 0 && (frame[len-1] == '\n' || frame[len-1] == '\r')) {
        frame[len-1] = '\0';
        len = strlen(frame);
    }
    printf("Received PPP Frame :\n%s\n", frame);
    dataEndIndex = len - 8 - 16;
    j = 0;
    for(i = 8; i < dataEndIndex; i++) {
        stuffed[j++] = frame[i];
    }
    stuffed[j] = '\0';
    j = 0;
    for(i = dataEndIndex; i < len - 8; i++) {
        receivedChecksum[j++] = frame[i];
    }
    receivedChecksum[j] = '\0';
    printf("\nStuffed Data :\n%s\n", stuffed);
    printf("Extracted Checksum:\n%s\n", receivedChecksum);

    printf("\nDo you want to modify a bit? (1 for Yes, 0 for No): ");
    scanf("%d", &simulateError);
    if (simulateError == 1)
    {
        printf("Enter bit index position to flip (0 to %d): ", (int)(strlen(stuffed) - 1));
        scanf("%d", &bitPos);

        if (bitPos >= 0 && bitPos < (int)strlen(stuffed))
        {
            stuffed[bitPos] = (stuffed[bitPos] == '0') ? '1' : '0';
            printf("\nData Stream after simulated error:\n%s\n", stuffed);
        }
    }
    actualChecksumValue = calculateActualChecksum(stuffed, receivedChecksum);
    intToBinary16(actualChecksumValue, checksumBinaryStr);
    status = (actualChecksumValue == 0xFFFF) ? 1 : 0;
    if (status == 1)
    {
        printf("\nRESULT: VALID.No error\n");
        printf("Calculated Checksum: %s\n", checksumBinaryStr);
        byteDestuffing(stuffed, destuffed);
        binaryToMessage(destuffed, message);
        printf("Decoded Message: %s\n", message);
    }
    else
    {
        printf("\nRESULT: INVALID.Error in data\n");
        printf("Calculated Checksum: %s\n", checksumBinaryStr);
        printf("Integrity check failed.\n");
    }
    fp = fopen("output.txt", "a");
    if(fp != NULL)
    {
        fprintf(fp, "\n\n********** RECEIVER **********\n\n");
        fprintf(fp, "PPP Frame :\n%s\n\n", frame);
        if (status == 1) {
            fprintf(fp, "Result: VALID\n");
            fprintf(fp, "Calculated Checksum Status: %d\n", status);
            fprintf(fp, "Calculated Checksum: %s\n", checksumBinaryStr);
            fprintf(fp, "Original Message: %s\n", message);
        } else {
            fprintf(fp, "Result: INVALID\n");
            fprintf(fp, "Calculated Checksum Status: %d\n", status);
            fprintf(fp, "Calculated Checksum: %s\n", checksumBinaryStr);
        }
        fclose(fp);
    }
    printf("\nReceiver result in output.txt\n");
    return 0;
}

