#include <stdio.h>
#include <string.h>
char str[100];
unsigned char data[100];
int n;
FILE *fp;
void getinput()
{
    printf("Enter string: ");
    fgets(str, 100, stdin);
    n = strlen(str);
    if(str[n-1]=='\n')
    {
        str[n-1]='\0';
        n--;
    }
}
void makeparity()
{
    int i, j, count, bit;
    unsigned char ch;
    for(i=0;i<n;i++)
    {
        ch = str[i] & 0x7F;
        count = 0;
        for(j=0;j<7;j++)
        {
            bit = (ch >> j) & 1;
            if(bit==1)
                count++;
        }
        if(count%2==0)
            ch = ch & 0x7F;
        else
            ch = ch | 0x80;
        data[i] = ch;
    }
}
void display()
{
    int i, j, bit;
    printf("\nSent data (8 bits, MSB = parity):\n");
    for(i=0;i<n;i++)
    {
        printf("%c : ", str[i]);
        for(j=7;j>=0;j--)
        {
            bit = (data[i]>>j) & 1;
            printf("%d",bit);
        }
        printf("\n");
    }
}
void writefile()
{
    fp = fopen("channel.dat","wb");
    fwrite(&n, sizeof(int), 1, fp);
    fwrite(data, sizeof(unsigned char), n, fp);
    fclose(fp);
}
int main()
{
    getinput();
    makeparity();
    display();
    writefile();
    printf("\nData sent to receiver.\n");
    return 0;
}
#include <stdio.h>
unsigned char data[100];
int n;
FILE *fp;
void readfile()
{
    fp = fopen("channel.dat","rb");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fread(&n, sizeof(int), 1, fp);
    fread(data, sizeof(unsigned char), n, fp);
    fclose(fp);
}
void display()
{
    int i, j, bit;
    printf("\nReceived data:\n");
    for(i=0;i<n;i++)
    {
        printf("Byte %d : ",i);
        for(j=7;j>=0;j--)
        {
            bit = (data[i]>>j) & 1;
            printf("%d",bit);
        }
        printf("\n");
    }
}
void flipbits()
{
    int num_flips, pos, bitpos;
    printf("\nEnter number of bits you want to modify: ");
    scanf("%d", &num_flips);
    int k;
    for(k = 0; k < num_flips; k++)
    {
        printf("\n--- Flip %d ---\n", k + 1);
        printf("Enter byte position to modify(0 to %d): ", n-1);
        scanf("%d",&pos);
        if(pos < 0 || pos >= n) {
            printf("Invalid byte position! Try again.\n");
            k--; 
            continue;
        }
        printf("Enter bit position to modify(0 to 7): ");
        scanf("%d",&bitpos);
        if(bitpos < 0 || bitpos > 7) {
            printf("Invalid bit position! Try again.\n");
            k--; 
            continue;
        }
        data[pos] = data[pos] ^ (1<<bitpos);
    }
}
void checkparity()
{
    int i, j, count, bit;
    printf("\nParity check after flipping:\n");
    for(i=0;i<n;i++)
    {
        count = 0;
        for(j=0;j<8;j++)
        {
            bit = (data[i]>>j) & 1;
            if(bit==1)
                count++;
        }
        printf("Byte %d : ",i);
        if(count%2==0)
            printf("Even parity OK, no error detected\n");
        else
            printf("Parity error detected\n");
    }
}
int main()
{
    readfile();
    display();
    flipbits();    
    checkparity(); 
    return 0;
}

