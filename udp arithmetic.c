Server

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
int main()
{
    int s,n;
    float a,b,result;
    char op,msg[100];
    struct sockaddr_in server,client;
    socklen_t len=sizeof(client);
    s=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port=htons(8082);
    server.sin_addr.s_addr=INADDR_ANY;
    bind(s,(struct sockaddr*)&server,sizeof(server));
    printf("UDP Arithmetic Server Started\n");
    while(1)
    {
        n=recvfrom(s,msg,sizeof(msg)-1,0,(struct sockaddr*)&client,&len);
        if(n<=0)
            continue;
        msg[n]='\0';
        sscanf(msg,"%f %f %c",&a,&b,&op);
        if(op=='+')
            result=a+b;
        else if(op=='-')
            result=a-b;
        else if(op=='*')
            result=a*b;
        else if(op=='/' && b!=0)
            result=a/b;
        else if(op=='/' && b==0)
        {
            strcpy(msg,"Error: Division by zero");
            sendto(s,msg,strlen(msg)+1,0,(struct sockaddr*)&client,len);
            continue;
        }
        else if(op=='x')
        {
            strcpy(msg,"Exit");
            sendto(s,msg,strlen(msg)+1,0,(struct sockaddr*)&client,len);
            break;
        }
        else
        {
            strcpy(msg,"Invalid Operation");
            sendto(s,msg,strlen(msg)+1,0,(struct sockaddr*)&client,len);
            continue;
        }
        sprintf(msg,"Result = %.2f",result);
        sendto(s,msg,strlen(msg)+1,0,(struct sockaddr*)&client,len);
    }
    close(s);
    return 0;
}


Client

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
int main()
{
    int s,n;
    float a,b;
    char op,msg[100];
    struct sockaddr_in server;
    socklen_t len=sizeof(server);
    s=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port=htons(8082);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    while(1)
    {
        printf("\nEnter operation (+,-,*,/ or x to exit): ");
        scanf(" %c",&op);
        a=0;
        b=0;
        if(op!='x')
        {
            printf("Enter two numbers: ");
            scanf("%f %f",&a,&b);
        }
        sprintf(msg,"%.2f %.2f %c",a,b,op);
        sendto(s,msg,strlen(msg)+1,0,(struct sockaddr*)&server,len);
        n=recvfrom(s,msg,sizeof(msg)-1,0,(struct sockaddr*)&server,&len);
        if(n>0)
        {
            msg[n]='\0';
            printf("%s\n",msg);
        }
        if(op=='x')
            break;
    }
    close(s);
    return 0;
}