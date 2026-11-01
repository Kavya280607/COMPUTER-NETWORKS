Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#define PORT 8085
#define MAX 100
struct Book
{
    int id;
    char title[50];
    char author[50];
    char category[30];
    int issued_to;
};
struct Request
{
    int choice;
    int book_id;
    int student_id;
    char search[50];
};
struct Response
{
    char message[100];
    int count;
    struct Book b[MAX];
};
struct Book books[MAX];
int n=0;
void loadBooks()
{
    FILE *fp=fopen("books.dat","rb");
    if(fp!=NULL)
    {
        n=fread(books,sizeof(struct Book),MAX,fp);
        fclose(fp);
    }
    else
    {
        books[0].id=101;
        strcpy(books[0].title,"C Programming");
        strcpy(books[0].author,"Dennis Ritchie");
        strcpy(books[0].category,"Programming");
        books[0].issued_to=0;
        books[1].id=102;
        strcpy(books[1].title,"Computer Networks");
        strcpy(books[1].author,"Tanenbaum");
        strcpy(books[1].category,"Networking");
        books[1].issued_to=0;
        books[2].id=103;
        strcpy(books[2].title,"Database Systems");
        strcpy(books[2].author,"Elmasri");
        strcpy(books[2].category,"DBMS");
        books[2].issued_to=0;
        n=3;
    }
}
void saveBooks()
{
    FILE *fp=fopen("books.dat","wb");
    if(fp!=NULL)
    {
        fwrite(books,sizeof(struct Book),n,fp);
        fclose(fp);
    }
}
void logActivity(char *msg)
{
    FILE *fp=fopen("library_log.txt","a");
    time_t t=time(NULL);
    if(fp!=NULL)
    {
        fprintf(fp,"%s - %s",msg,ctime(&t));
        fclose(fp);
    }
}
int findBook(int id)
{
    int i;
    for(i=0;i<n;i++)
        if(books[i].id==id)
            return i;
    return -1;
}
void processRequest(struct Request req,struct Response *res)
{
    int i,pos;
    memset(res,0,sizeof(struct Response));
    switch(req.choice)
    {
        case 1:
            for(i=0;i<n;i++)
                if(strstr(books[i].title,req.search)!=NULL || strstr(books[i].author,req.search)!=NULL)
                    res->b[res->count++]=books[i];
            strcpy(res->message,"Search Results");
            break;
        case 2:
            for(i=0;i<n;i++)
                res->b[res->count++]=books[i];
            strcpy(res->message,"Book Availability");
            break;
        case 3:
            pos=findBook(req.book_id);
            if(pos==-1)
                strcpy(res->message,"Book Not Found");
            else if(req.student_id<=0)
                strcpy(res->message,"Invalid Student ID");
            else if(books[pos].issued_to!=0)
                strcpy(res->message,"Book Already Issued");
            else
            {
                books[pos].issued_to=req.student_id;
                saveBooks();
                logActivity("Book Issued");
                strcpy(res->message,"Book Issued Successfully");
            }
            break;
        case 4:
            pos=findBook(req.book_id);
            if(pos==-1)
                strcpy(res->message,"Book Not Found");
            else if(req.student_id<=0 || books[pos].issued_to!=req.student_id)
                strcpy(res->message,"Invalid Return Request");
            else
            {
                books[pos].issued_to=0;
                saveBooks();
                logActivity("Book Returned");
                strcpy(res->message,"Book Returned Successfully");
            }
            break;
        case 5:
            for(i=0;i<n;i++)
                if(req.student_id>0 && books[i].issued_to==req.student_id)
                    res->b[res->count++]=books[i];
            strcpy(res->message,"Issued Book Details");
            break;
        case 6:
            strcpy(res->message,"Session Terminated");
            break;
        default:
            strcpy(res->message,"Invalid Choice");
    }
}
int main()
{
    int s,c;
    struct sockaddr_in server,client;
    socklen_t len=sizeof(client);
    struct Request req;
    struct Response res;
    loadBooks();
    s=socket(AF_INET,SOCK_STREAM,0);
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(PORT);
    bind(s,(struct sockaddr *)&server,sizeof(server));
    listen(s,5);
    printf("Library Server Waiting...\n");
    while(1)
    {
        c=accept(s,(struct sockaddr *)&client,&len);
        if(c<0)
            continue;
        printf("Client Connected\n");
        while(1)
        {
            if(recv(c,&req,sizeof(req),0)<=0)
                break;
            processRequest(req,&res);
            if(send(c,&res,sizeof(res),0)<=0)
                break;
            if(req.choice==6)
                break;
        }
        close(c);
        printf("Client Disconnected\n");
    }
    close(s);
    return 0;
}
Client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8085
#define MAX 100
struct Book
{
    int id;
    char title[50];
    char author[50];
    char category[30];
    int issued_to;
};
struct Request
{
    int choice;
    int book_id;
    int student_id;
    char search[50];
};
struct Response
{
    char message[100];
    int count;
    struct Book b[MAX];
};
void displayBook(struct Book b)
{
    printf("\nBook ID: %d\n",b.id);
    printf("Title: %s\n",b.title);
    printf("Author: %s\n",b.author);
    printf("Category: %s\n",b.category);
    if(b.issued_to==0)
        printf("Status: Available\n");
    else
        printf("Status: Issued\n");
}
int main()
{
    int s,choice,i;
    struct sockaddr_in server;
    struct Request req;
    struct Response res;
    s=socket(AF_INET,SOCK_STREAM,0);
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(connect(s,(struct sockaddr *)&server,sizeof(server))<0)
    {
        perror("Connection Failed");
        return 1;
    }
    printf("Connected to Library Server\n");
    while(1)
    {
        memset(&req,0,sizeof(req));
        printf("\n1.Search Book\n2.Check Book Availability\n3.Issue Book\n4.Return Book\n5.View Issued Books\n6.Exit\n");
        printf("Enter Choice: ");
        scanf("%d",&choice);
        req.choice=choice;
        if(choice==1)
        {
            printf("Enter Book Title or Author: ");
            scanf(" %49[^\n]",req.search);
        }
        else if(choice==3 || choice==4)
        {
            printf("Enter Book ID: ");
            scanf("%d",&req.book_id);
            printf("Enter Student ID: ");
            scanf("%d",&req.student_id);
        }
        else if(choice==5)
        {
            printf("Enter Student ID: ");
            scanf("%d",&req.student_id);
        }
        else if(choice<1 || choice>6)
        {
            printf("Invalid Choice\n");
            continue;
        }
        if(send(s,&req,sizeof(req),0)<=0)
            break;
        if(recv(s,&res,sizeof(res),0)<=0)
            break;
        printf("\nServer: %s\n",res.message);
        for(i=0;i<res.count;i++)
            displayBook(res.b[i]);
        if(res.count==0 && (choice==1 || choice==5))
            printf("No Records Found\n");
        if(choice==6)
            break;
    }
    close(s);
    return 0;
}