Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#define PORT 8084
#define MAX 100
struct Patient
{
    int id;
    char name[50];
    int age;
};
struct Appointment
{
    int patient_id;
    int doctor_id;
    int slot;
    int active;
};
struct Request
{
    int choice;
    struct Patient p;
    int doctor_id;
    int slot;
};
struct Response
{
    char message[200];
    int count;
    struct Appointment a[MAX];
};
struct Patient patients[MAX];
struct Appointment appointments[MAX];
int np=0,na=0;
void loadData()
{
    FILE *fp=fopen("patients.dat","rb");
    if(fp!=NULL)
    {
        np=fread(patients,sizeof(struct Patient),MAX,fp);
        fclose(fp);
    }
    fp=fopen("appointments.dat","rb");
    if(fp!=NULL)
    {
        na=fread(appointments,sizeof(struct Appointment),MAX,fp);
        fclose(fp);
    }
}
void saveData()
{
    FILE *fp=fopen("patients.dat","wb");
    if(fp!=NULL)
    {
        fwrite(patients,sizeof(struct Patient),np,fp);
        fclose(fp);
    }
    fp=fopen("appointments.dat","wb");
    if(fp!=NULL)
    {
        fwrite(appointments,sizeof(struct Appointment),na,fp);
        fclose(fp);
    }
}
void logActivity(char *msg)
{
    FILE *fp=fopen("hospital_log.txt","a");
    time_t t=time(NULL);
    if(fp!=NULL)
    {
        fprintf(fp,"%s - %s",msg,ctime(&t));
        fclose(fp);
    }
}
int findPatient(int id)
{
    int i;
    for(i=0;i<np;i++)
        if(patients[i].id==id)
            return i;
    return -1;
}
void processRequest(struct Request req,struct Response *res)
{
    int i,found;
    memset(res,0,sizeof(struct Response));
    switch(req.choice)
    {
        case 1:
            if(req.p.id<=0 || req.p.age<=0 || strlen(req.p.name)==0 || findPatient(req.p.id)!=-1 || np>=MAX)
                strcpy(res->message,"Invalid or Duplicate Patient Details");
            else
            {
                patients[np++]=req.p;
                saveData();
                logActivity("Patient Registered");
                strcpy(res->message,"Patient Registered Successfully");
            }
            break;
        case 2:
            if(findPatient(req.p.id)==-1)
            {
                strcpy(res->message,"Register Patient First");
                break;
            }
            if(req.doctor_id<1 || req.doctor_id>3 || req.slot<1 || req.slot>3 || na>=MAX)
            {
                strcpy(res->message,"Invalid Doctor or Slot");
                break;
            }
            found=0;
            for(i=0;i<na;i++)
                if(appointments[i].active && appointments[i].doctor_id==req.doctor_id && appointments[i].slot==req.slot)
                    found=1;
            if(found)
                strcpy(res->message,"Slot Not Available");
            else
            {
                appointments[na].patient_id=req.p.id;
                appointments[na].doctor_id=req.doctor_id;
                appointments[na].slot=req.slot;
                appointments[na].active=1;
                na++;
                saveData();
                logActivity("Appointment Booked");
                strcpy(res->message,"Appointment Booked Successfully");
            }
            break;
        case 3:
            found=0;
            for(i=0;i<na;i++)
                if(appointments[i].active && appointments[i].patient_id==req.p.id && appointments[i].doctor_id==req.doctor_id && appointments[i].slot==req.slot)
                {
                    appointments[i].active=0;
                    found=1;
                    break;
                }
            if(found)
            {
                saveData();
                logActivity("Appointment Cancelled");
                strcpy(res->message,"Appointment Cancelled Successfully");
            }
            else
                strcpy(res->message,"Appointment Not Found");
            break;
        case 4:
            if(req.doctor_id<1 || req.doctor_id>3)
            {
                strcpy(res->message,"Invalid Doctor ID");
                break;
            }
            sprintf(res->message,"Available Slots for Doctor %d: ",req.doctor_id);
            for(int slot=1;slot<=3;slot++)
            {
                found=0;
                for(i=0;i<na;i++)
                    if(appointments[i].active && appointments[i].doctor_id==req.doctor_id && appointments[i].slot==slot)
                        found=1;
                if(!found)
                {
                    char temp[10];
                    sprintf(temp,"%d ",slot);
                    strcat(res->message,temp);
                }
            }
            logActivity("Doctor Availability Searched");
            break;
        case 5:
            for(i=0;i<na;i++)
                if(appointments[i].active && appointments[i].patient_id==req.p.id)
                    res->a[res->count++]=appointments[i];
            strcpy(res->message,"Appointment History");
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
    loadData();
    s=socket(AF_INET,SOCK_STREAM,0);
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(PORT);
    bind(s,(struct sockaddr *)&server,sizeof(server));
    listen(s,5);
    printf("Hospital Server Waiting...\n");
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
#define PORT 8084
#define MAX 100
struct Patient
{
    int id;
    char name[50];
    int age;
};
struct Appointment
{
    int patient_id;
    int doctor_id;
    int slot;
    int active;
};
struct Request
{
    int choice;
    struct Patient p;
    int doctor_id;
    int slot;
};
struct Response
{
    char message[200];
    int count;
    struct Appointment a[MAX];
};
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
    printf("Connected to Hospital Server\n");
    while(1)
    {
        memset(&req,0,sizeof(req));
        printf("\n1.Register Patient\n2.Book Appointment\n3.Cancel Appointment\n4.Search Doctor Availability\n5.View Appointment History\n6.Exit\n");
        printf("Enter Choice: ");
        scanf("%d",&choice);
        req.choice=choice;
        if(choice==1)
        {
            printf("Enter Patient ID: ");
            scanf("%d",&req.p.id);
            printf("Enter Name: ");
            scanf(" %49[^\n]",req.p.name);
            printf("Enter Age: ");
            scanf("%d",&req.p.age);
        }
        else if(choice==2)
        {
            printf("Enter Patient ID: ");
            scanf("%d",&req.p.id);
            printf("Enter Doctor ID (1-3): ");
            scanf("%d",&req.doctor_id);
            printf("Enter Slot (1-3): ");
            scanf("%d",&req.slot);
        }
        else if(choice==3)
        {
            printf("Enter Patient ID: ");
            scanf("%d",&req.p.id);
            printf("Enter Doctor ID: ");
            scanf("%d",&req.doctor_id);
            printf("Enter Slot: ");
            scanf("%d",&req.slot);
        }
        else if(choice==4)
        {
            printf("Enter Doctor ID (1-3): ");
            scanf("%d",&req.doctor_id);
        }
        else if(choice==5)
        {
            printf("Enter Patient ID: ");
            scanf("%d",&req.p.id);
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
        if(choice==5)
        {
            for(i=0;i<res.count;i++)
            {
                printf("Patient ID: %d\n",res.a[i].patient_id);
                printf("Doctor ID: %d\n",res.a[i].doctor_id);
                printf("Slot: %d\n",res.a[i].slot);
            }
            if(res.count==0)
                printf("No Appointments Found\n");
        }
        if(choice==6)
            break;
    }
    close(s);
    return 0;
}