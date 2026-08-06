#include <stdio.h>
#include <string.h>
#define MAX 1000
char flag[]="01111110";
char escape[]="01111101";
char message[100];
char binary[MAX]="";
char stuffed[MAX];
char frame[MAX];
void charToBinary(char ch,char bin[])
{
    int i;
    for(i=7;i>=0;i--)
    {
        if((ch>>i)&1)
            bin[7-i]='1';
        else
            bin[7-i]='0';
    }
    bin[8]='\0';
}
void messageToBinary(char msg[],char binary[])
{
    int i;
    char temp[9];
    binary[0]='\0';
    for(i=0;msg[i]!='\0' && msg[i]!='\n';i++)
    {
        charToBinary(msg[i],temp);
        strcat(binary,temp);
    }
}
void byteStuffing(char input[],char output[])
{
    int i=0,j=0;
    char byte[9];
    while(input[i]!='\0')
    {
        int k;
        for(k=0;k<8;k++)
            byte[k]=input[i+k];
        byte[8]='\0';
        if(strcmp(byte,flag)==0 || strcmp(byte,escape)==0)
        {
            strcpy(&output[j],escape);
            j+=8;
        }
        strcpy(&output[j],byte);
        j+=8;
        i+=8;
    }
    output[j]='\0';
}
int main()
{
    FILE *fp;
    fp=fopen("message.txt","r");
    if(fp==NULL)
    {
        printf("Cannot open message.txt");
        return 0;
    }
    fgets(message,100,fp);
    fclose(fp);
    printf("Original Message : %s\n",message);
    messageToBinary(message,binary);
    printf("\nBinary Data :\n%s\n",binary);
    byteStuffing(binary,stuffed);
    printf("\nStuffed Data :\n%s\n",stuffed);
    strcpy(frame, flag);
    strcat(frame, stuffed);
    strcat(frame, flag);
    printf("\nPPP Frame :\n");
    printf("------------------------------------------\n");
    printf("| Flag | Stuffed Data | Flag |\n");
    printf("------------------------------------------\n");
    printf("|%s|%s|%s|\n", flag, stuffed, flag);
    printf("------------------------------------------\n");
    fp = fopen("frame.txt", "w");
    if(fp == NULL)
    {
        printf("Cannot create frame.txt");
        return 0;
    }
    fputs(frame, fp);
    fclose(fp);
    fp = fopen("output.txt", "w");
    if(fp == NULL)
    {
        printf("Cannot create output.txt");
        return 0;
    }
    fprintf(fp, "********** PPP SENDER **********\n\n");
    fprintf(fp, "Original Message :\n%s\n", message);
    fprintf(fp, "Binary Data :\n%s\n\n", binary);
    fprintf(fp, "Stuffed Data :\n%s\n\n", stuffed);
    fprintf(fp, "PPP Frame :\n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "| Flag | Stuffed Data | Flag |\n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "|%s|%s|%s|\n", flag, stuffed, flag);
    fprintf(fp, "------------------------------------------\n");
    fclose(fp);
    printf("\nPPP Frame saved in frame.txt");
    printf("\nSender output saved in output.txt");
    return 0;
}

#include <stdio.h>
#include <string.h>
#define MAX 1000
char flag[]="01111110";
char escape[]="01111101";
char frame[MAX];
char stuffed[MAX];
char destuffed[MAX];
char message[100];
void byteDestuffing(char input[],char output[])
{
    int i=0,j=0;
    char byte[9];
    while(input[i]!='\0')
    {
        int k;
        for(k=0;k<8;k++)
            byte[k]=input[i+k];
        byte[8]='\0';
        if(strcmp(byte,escape)==0)
        {
            i=i+8;
            for(k=0;k<8;k++)
                byte[k]=input[i+k];
            byte[8]='\0';
        }
        strcpy(&output[j],byte);
        j=j+8;
        i=i+8;
    }
    output[j]='\0';
}
void binaryToMessage(char binary[],char msg[])
{
    int i=0,j=0;
    while(binary[i]!='\0')
    {
        int value=0;
        int k;
        for(k=0;k<8;k++)
        {
            value=value*2+(binary[i+k]-'0');
        }
        msg[j]=(char)value;
        j++;
        i=i+8;
    }
    msg[j]='\0';
}
int main()
{
    FILE *fp;
    int i,j=0,len;
    fp=fopen("frame.txt","r");
    if(fp==NULL)
    {
        printf("Cannot open frame.txt");
        return 0;
    }
    fgets(frame,MAX,fp);
    fclose(fp);
    printf("Received PPP Frame :\n%s\n",frame);
    len=strlen(frame);
    if(frame[len-1]=='\n')
        len--;
    for(i=8;i<len-8;i++)
    {
        stuffed[j]=frame[i];
        j++;
    }
    stuffed[j]='\0';
    printf("\nStuffed Data :\n%s\n",stuffed);
    byteDestuffing(stuffed,destuffed);
    printf("\nDestuffed Binary :\n%s\n",destuffed);
    binaryToMessage(destuffed,message);
    printf("\nOriginal Message : %s\n",message);
    fp = fopen("output.txt", "a");
    if(fp == NULL)
    {
        printf("Cannot open output.txt");
        return 0;
    }
    fprintf(fp, "\n\n********** PPP RECEIVER **********\n\n");
    fprintf(fp, "Received PPP Frame :\n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "  Flag   Stuffed Data   Flag \n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, " %s %s %s \n", flag, stuffed, flag);
    fprintf(fp, "------------------------------------------\n\n");
    fprintf(fp, "After Removing Flags :\n%s\n\n", stuffed);
    fprintf(fp, "Destuffed Binary :\n%s\n\n", destuffed);
    fprintf(fp, "Original Message :\n%s\n\n", message);
    fprintf(fp, "PPP Frame After Destuffing :\n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "  Flag   Original Data   Flag  \n");
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, " %s %s %s \n", flag, destuffed, flag);
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "\nData Received Successfully.\n");
    fclose(fp);
    printf("\nReceiver output saved in output.txt\n");
    return 0;
}


