Server

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/select.h>
struct Book
{
    int id;
    char name[50];
    int available;
};
int main()
{
    int s,n,i,found,choice,id;
    char msg[100],log[100];
    struct Book b[5]={{1,"C",1},{2,"Java",1},{3,"Python",1},{4,"CN",1},{5,"DBMS",1}};
    struct sockaddr_in server,client;
    socklen_t len=sizeof(client);
    FILE *fp;
    s=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port=htons(8081);
    server.sin_addr.s_addr=INADDR_ANY;
    bind(s,(struct sockaddr*)&server,sizeof(server));
    printf("Library Server Started\n");
    while(1)
    {
        fd_set set;
        struct timeval tv;
        FD_ZERO(&set);
        FD_SET(s,&set);
        tv.tv_sec=5;
        tv.tv_usec=0;
        if(select(s+1,&set,NULL,NULL,&tv)<=0)
            continue;
        n=recvfrom(s,msg,sizeof(msg)-1,0,(struct sockaddr*)&client,&len);
        if(n<=0)
            continue;
        msg[n]='\0';
        sendto(s,"ACK",4,0,(struct sockaddr*)&client,len);
        sscanf(msg,"%d %d",&choice,&id);
        found=0;
        strcpy(log,"");
        if(choice==5)
        {
            strcpy(msg,"Available Books:\n");
            for(i=0;i<5;i++)
                if(b[i].available)
                {
                    sprintf(log,"%d %s\n",b[i].id,b[i].name);
                    strcat(msg,log);
                }
        }
        else
        {
            for(i=0;i<5;i++)
                if(b[i].id==id)
                {
                    found=1;
                    if(choice==1)
                        sprintf(msg,"Book Found: %s",b[i].name);
                    else if(choice==2 && b[i].available)
                    {
                        b[i].available=0;
                        sprintf(msg,"Book Issued: %s",b[i].name);
                    }
                    else if(choice==3 && !b[i].available)
                    {
                        b[i].available=1;
                        sprintf(msg,"Book Returned: %s",b[i].name);
                    }
                    else
                        strcpy(msg,"Book Not Available");
                    break;
                }
            if(!found)
                strcpy(msg,"Book Not Found");
        }
        fp=fopen("library.txt","w");
        for(i=0;i<5;i++)
            fprintf(fp,"%d %s %d\n",b[i].id,b[i].name,b[i].available);
        fclose(fp);
        fp=fopen("activity.log","a");
        fprintf(fp,"Request: %s Response: %s\n",log,msg);
        fclose(fp);
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
#include<sys/select.h>
int main()
{
    int s,choice,id,n,tries;
    char msg[100];
    struct sockaddr_in server;
    socklen_t len=sizeof(server);
    struct timeval tv;
    s=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port=htons(8081);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    tv.tv_sec=2;
    tv.tv_usec=0;
    setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
    while(1)
    {
        printf("\n1.Search Book\n2.Issue Book\n3.Return Book\n4.Exit\n5.Available Books\n");
        printf("Enter choice: ");
        scanf("%d",&choice);
        if(choice==4)
            break;
        id=0;
        if(choice>=1 && choice<=3)
        {
            printf("Enter Book ID: ");
            scanf("%d",&id);
        }
        sprintf(msg,"%d %d",choice,id);
        for(tries=0;tries<3;tries++)
        {
            sendto(s,msg,strlen(msg)+1,0,(struct sockaddr*)&server,len);
            n=recvfrom(s,msg,sizeof(msg)-1,0,(struct sockaddr*)&server,&len);
            if(n>0 && strcmp(msg,"ACK")==0)
                break;
            printf("Timeout! Retrying...\n");
        }
        if(tries==3)
        {
            printf("Server not responding\n");
            continue;
        }
        n=recvfrom(s,msg,sizeof(msg)-1,0,(struct sockaddr*)&server,&len);
        if(n>0)
        {
            msg[n]='\0';
            printf("%s\n",msg);
        }
    }
    close(s);
    return 0;
}