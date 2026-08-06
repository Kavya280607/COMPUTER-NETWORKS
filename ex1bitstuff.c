#include <stdio.h>
#include <string.h>
#define MAX 1000
char message[100];
char binary[MAX] = "";
char stuffed[MAX];
char frame[MAX];
void charToBinary(char ch)
{
    int i;
    for(i = 7; i >= 0; i--)
    {
        if((ch >> i) & 1)
            strcat(binary, "1");
        else
            strcat(binary, "0");
    }
}
int main()
{
    FILE *fp;
    int i, j = 0, count = 0;
    fp = fopen("message.txt", "r");
    if(fp == NULL)
    {
        printf("Cannot open message.txt");
        return 0;
    }
    fgets(message, 100, fp);
    fclose(fp);
    printf("Original Message : %s\n", message);
    for(i = 0; message[i] != '\0' && message[i] != '\n'; i++)
    {
        charToBinary(message[i]);
    }
    printf("Binary Data :\n%s\n", binary);
    for(i = 0; binary[i] != '\0'; i++)
    {
        stuffed[j++] = binary[i];
        if(binary[i] == '1')
            count++;
        else
            count = 0;
        if(count == 5)
        {
            stuffed[j++] = '0';
            count = 0;
        }
    }
    stuffed[j] = '\0';
    printf("Stuffed Data :\n%s\n", stuffed);
    strcpy(frame, "01111110");
    strcat(frame, stuffed);
    strcat(frame, "01111110");
    printf("\nHDLC Frame :\n%s\n", frame);
    fp = fopen("frame.txt", "w");
    if(fp == NULL)
    {
        printf("Cannot create frame.txt");
        return 0;
    }
    fputs(frame, fp);
    fclose(fp);
    fp = fopen("output.txt", "w");
    fprintf(fp, "********** HDLC SENDER **********\n\n");
    fprintf(fp, "Original Message :\n%s\n", message);
    fprintf(fp, "Binary Data :\n%s\n\n", binary);
    fprintf(fp, "Stuffed Data :\n%s\n\n", stuffed);
    fprintf(fp, "HDLC Frame :\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "  Flag   Stuffed Data   Flag \n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, " 01111110 %s 01111110 \n", stuffed);
    fprintf(fp, "----------------------------------------\n");
    fclose(fp);
    printf("\nHDLC Frame saved in frame.txt");
    printf("\nSender output saved in output.txt");
    return 0;
}
#include <stdio.h>
#include <string.h>
#define MAX 1000
char frame[MAX];
char stuffed[MAX];
char destuffed[MAX];
int main()
{
    FILE *fp;
    int i, j = 0, count = 0;
    int len;
    fp = fopen("frame.txt", "r");
    if(fp == NULL)
    {
        printf("Cannot open frame.txt");
        return 0;
    }
    fgets(frame, MAX, fp);
    fclose(fp);
    printf("Received HDLC Frame :\n%s\n", frame);
    len = strlen(frame);
    if(frame[len - 1] == '\n')
        len--;
    j = 0;
    for(i = 8; i < len - 8; i++)
    {
        stuffed[j++] = frame[i];
    }
    stuffed[j] = '\0';
    printf("\nStuffed Data :\n%s\n", stuffed);
    j = 0;
    count = 0;
    for(i = 0; stuffed[i] != '\0'; i++)
    {
        destuffed[j++] = stuffed[i];
        if(stuffed[i] == '1')
            count++;
        else
            count = 0;
        if(count == 5)
        {
            if(stuffed[i + 1] == '0')
                i++;
            count = 0;
        }
    }
    destuffed[j] = '\0';
    printf("\nDestuffed Data :\n%s\n", destuffed);
    fp = fopen("output.txt", "a");
    fprintf(fp, "\n\n********** HDLC RECEIVER **********\n\n");
    fprintf(fp, "Received HDLC Frame :\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "  Flag   Stuffed Data   Flag  \n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, " 01111110 %s 01111110 \n", stuffed);
    fprintf(fp, "----------------------------------------\n\n");
    fprintf(fp, "After Removing Flags :\n");
    fprintf(fp, "%s\n\n", stuffed);
    fprintf(fp, "Destuffed Data :\n");
    fprintf(fp, "%s\n\n", destuffed);
    fprintf(fp, "HDLC Frame After Destuffing :\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "  Flag   Original Data   Flag \n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, " 01111110 %s 01111110 \n", destuffed);
    fprintf(fp, "----------------------------------------\n\n");
    fprintf(fp, "Data Received Successfully.\n");
    fclose(fp);
    printf("\nReceiver output saved in output.txt\n");
    return 0;
}


