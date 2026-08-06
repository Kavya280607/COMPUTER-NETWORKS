#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct url
{
    char url[50];
    char ip[50];
} urltab[]={
    {"google.com","172.217.160.78"},
    {"mepcoeng.ac.in","172.16.16.1"},
    {"gmail.com","172.217.179.98"},
    {"youtube.com","172.15.178.10"},
    {"facebook.com","172.254.16.56"},
    {"instagram.com","172.145.166.176"}
};
struct mac
{
    char ip[50];
    char mac[50];
} mactab[]={
    {"172.217.160.78","AA:1B:34:6C:33:1A"},
    {"172.217.179.98","AA:1B:34:6C:33:02"},
    {"172.15.178.10","AA:1B:34:5C:32:34"},
    {"172.254.16.56","AA:1B:34:4C:33:04"},
    {"172.145.166.176","A2:1B:34:6C:34:06"},
    {"172.16.16.1","A2:34:1A:03:3C:12"}
};
int urlcount=6;
int maccount=6;
void dectobin(int n,char bin[])
{
    int i;
    for(i=7;i>=0;i--)
    {
        bin[i]=(n%2)+'0';
        n=n/2;
    }
    bin[8]='\0';
}

void mestobin(char msg[],char binary[])
{
    int i,j,k=0;
    char bin[9];
    for(i=0;msg[i]!='\0';i++)
    {
        dectobin(msg[i],bin);
        for(j=0;j<8;j++)
            binary[k++]=bin[j];
    }
    binary[k]='\0';
}

void searchip(char url[],char ip[])
{
    int i,j;
    for(i=0;i<urlcount;i++)
    {
        if(strcmp(url,urltab[i].url)==0)
        {
            for(j=0;urltab[i].ip[j]!='\0';j++)
                ip[j]=urltab[i].ip[j];
            ip[j]='\0';
            return;
        }
    }
}

void searchmac(char ip[],char mac[])
{
    int i,j;
    for(i=0;i<maccount;i++)
    {
        if(strcmp(ip,mactab[i].ip)==0)
        {
            for(j=0;mactab[i].mac[j]!='\0';j++)
                mac[j]=mactab[i].mac[j];
            mac[j]='\0';
            return;
        }
    }
}

void iptobin(char ip[],char ipbin[])
{
    int x[4];
    int i,j=0,k;
    char bin[9];
    sscanf(ip,"%d.%d.%d.%d",&x[0],&x[1],&x[2],&x[3]);
    for(i=0;i<4;i++)
    {
        dectobin(x[i],bin);
        for(k=0;k<8;k++)
            ipbin[j++]=bin[k];
        if(i!=3)
            ipbin[j++]='.';
    }
    ipbin[j]='\0';
}

void mactobin(char mac[],char macbin[])
{
    int i,j=0,n;
    char bin[9];
    for(i=0;mac[i]!='\0';i++)
    {
        if(mac[i]==':')
        {
            macbin[j++]=':';
        }
        else
        {
            if(mac[i]>='0' && mac[i]<='9')
                n=mac[i]-'0';
            else
                n=mac[i]-'A'+10;
            dectobin(n,bin);
            macbin[j++]=bin[4];
            macbin[j++]=bin[5];
            macbin[j++]=bin[6];
            macbin[j++]=bin[7];
        }
    }
    macbin[j]='\0';
}
void porttobin(int n,char bin[])
{
    int i;
    for(i=15;i>=0;i--)
    {
        bin[i]=(n%2)+'0';
        n=n/2;
    }
    bin[16]='\0';
}
int main()
{
    char mes[100],bin[1000];
    char srcurl[50],desurl[50];
    char srcip[50],desip[50];
    char srcmac[50],desmac[50];
    char srcipbin[100],desipbin[100];
    char srcmacbin[100],desmacbin[100];
    char packet[17];
    char fragment[11]; 
    int sport,dport;
    char sportbin[50],dportbin[50];
    int i,j=0,k,x;
    int packetcount,fragmentcount=0;
    
    printf("Enter Message : ");
    scanf("%s",mes);
    printf("Enter Source URL : ");
    scanf("%s",srcurl);
    printf("Enter Destination URL : ");
    scanf("%s",desurl);
    
    mestobin(mes,bin);
    searchip(srcurl,srcip);
    searchip(desurl,desip);
    searchmac(srcip,srcmac);
    searchmac(desip,desmac);
    iptobin(srcip,srcipbin);
    iptobin(desip,desipbin);
    mactobin(srcmac,srcmacbin);
    mactobin(desmac,desmacbin);

    sport=1000+rand()%9000;
    dport=1000+rand()%9000;

    packetcount=(strlen(bin)+15)/16;
    porttobin(sport,sportbin);
    porttobin(dport,dportbin);
    
    printf("\n=========== TRANSPORT LAYER ===========\n");
    printf("Message \t Source portno \t Destination portno\n");
    printf("%s %s %s\n",bin,sportbin,dportbin);
    
    printf("\n=========== NETWORK LAYER ============\n");
    j=0;
    for(i=0;i<packetcount;i++)
    {
        for(k=0;k<16;k++)
        {
            if(bin[j] != '\0')
                packet[k]=bin[j++];
            else
                packet[k]='0';
        }
        packet[16]='\0';
        printf("\nPacket %d\n",i+1);
        printf("Packet data \t Source portno \t Destination portno \t Source IP \t Destination IP\n");
        printf("%s %s %s %s %s\n",packet,sportbin,dportbin,srcipbin,desipbin);
    }
    
    printf("\n=========== DATA LINK LAYER ===========\n");
    fragmentcount=0;
    j=0; 
    
    for(i=0;i<packetcount;i++)
    {
        for(k=0;k<16;k+=10)
        {
            for(x=0;x<10;x++)
            {
                if((k+x)<16 && (i*16 + (k+x) < packetcount*16))
                {
                    if((i*16 + (k+x)) < (int)strlen(bin))
                        fragment[x] = bin[i*16 + (k+x)];
                    else
                        fragment[x] = '0'; 
                }
                else
                {
                    fragment[x]='0'; 
                }
            }
            fragment[10]='\0';
            fragmentcount++;
            
            printf("\nFragment %d (From Packet %d)\n",fragmentcount, i+1);
            printf("Fragment data \t Source portno \t Destination portno \t SourceIP \t DestinationIP \t SourceMAC \t Destination MAC \t Error code\n");
            printf("%s %s %s %s %s %s %s 00000000\n",fragment,sportbin,dportbin,srcipbin,desipbin,srcmacbin,desmacbin);
        }
    }
    
    printf("\nTotal Bits : %d\n",(int)strlen(bin));
    printf("Total Packets : %d\n",packetcount);
    printf("Total Fragments : %d\n",fragmentcount);
    return 0;
}
