Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#define PORT 8083
#define MAX 100
#define SUBJECTS 5
struct Student
{
    int regno;
    char name[50];
    char dept[30];
    int semester;
    int marks[SUBJECTS];
    char grade[5];
    float cgpa;
};
struct Request
{
    int choice;
    struct Student s;
};
struct Response
{
    int status;
    int count;
    char message[100];
    struct Student s[MAX];
};
struct Student students[MAX];
int n = 0;
int sendall(int fd, void *data, int size)
{
    int total = 0, bytes;
    while (total < size)
    {
        bytes = send(fd, (char *)data + total,
                     size - total, 0);
        if (bytes <= 0)
           return -1;
        total += bytes;
    }
    return total;
}
int recvall(int fd, void *data, int size)
{
    int total = 0, bytes;
    while (total < size)
    {
        bytes = recv(fd, (char *)data + total,
                     size - total, 0);
        if (bytes <= 0)
            return -1;
        total += bytes;
    }
    return total;
}
void loadRecords()
{
    FILE *fp = fopen("students.dat", "rb");
    if (fp != NULL)
    {
        n = fread(students, sizeof(struct Student),
                  MAX, fp)
        fclose(fp);
    }
}
void saveRecords()
{
    FILE *fp = fopen("students.dat", "wb");
    if (fp != NULL)
    {
        fwrite(students, sizeof(struct Student),
               n, fp);
        fclose(fp);
    }
}
void logTransaction(char *activity)
{
    FILE *fp;
    time_t now;
    time(&now);
    fp = fopen("student_log.txt", "a");
    if (fp != NULL)
    {
        fprintf(fp, "%s - %s",
                activity, ctime(&now));
        fclose(fp);
    }
}
int findStudent(int regno)
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (students[i].regno == regno)
            return i;
    }
    return -1;
}
void calculateResult(struct Student *s)
{
    int i, total = 0;
    float avg;
    for (i = 0; i < SUBJECTS; i++)
        total += s->marks[i];
    avg = total / (float)SUBJECTS;
    s->cgpa = avg / 10.0;
    if (avg >= 90)
        strcpy(s->grade, "A+");
    else if (avg >= 80)
        strcpy(s->grade, "A");
    else if (avg >= 70)
        strcpy(s->grade, "B");
    else if (avg >= 60)
        strcpy(s->grade, "C");
    else if (avg >= 50)
        strcpy(s->grade, "D");
    else
        strcpy(s->grade, "F");
}
int validStudent(struct Student s)
{
    int i;
    if (s.regno <= 0 ||
        strlen(s.name) == 0 ||
        strlen(s.dept) == 0 ||
        s.semester < 1 || s.semester > 8)
        return 0;
    for (i = 0; i < SUBJECTS; i++)
    {
        if (s.marks[i] < 0 || s.marks[i] > 100)
            return 0;
    }
    return 1;
}
void processRequest(struct Request req,
                    struct Response *res)
{
    int pos, i;
    memset(res, 0, sizeof(struct Response));
    switch (req.choice)
    {
        case 1:
            if (!validStudent(req.s))
            {
                strcpy(res->message, "Invalid Student Details");
                break;
            }
            if (n >= MAX)
            {
                strcpy(res->message, "Database Full");
                break;
            }
            if (findStudent(req.s.regno) != -1)
            {
                strcpy(res->message, "Student Already Exists");
                break;
            }
            students[n] = req.s;
            calculateResult(&students[n]);
            n++;
            saveRecords();
            logTransaction("Student Added");
            strcpy(res->message,
                   "Student Added Successfully");
            break;
        case 2:
            pos = findStudent(req.s.regno);
            if (pos == -1)
            {
                strcpy(res->message, "Student Not Found");
                break;
            }
            res->count = 1;
            res->s[0] = students[pos];
            strcpy(res->message,
                   "Student Found Successfully");
            break;
        case 3:
            pos = findStudent(req.s.regno);
            if (pos == -1)
            {
                strcpy(res->message, "Student Not Found");
                break;
            }
            if (!validStudent(req.s))
            {
                strcpy(res->message, "Invalid Student Details");
                break;
            }
            students[pos] = req.s;
            calculateResult(&students[pos]);
            saveRecords();
            logTransaction("Student Updated");
            strcpy(res->message,
                   "Student Updated Successfully");
            break;
        case 4:
            pos = findStudent(req.s.regno);
            if (pos == -1)
            {
                strcpy(res->message, "Student Not Found");
                break;
            }
            for (i = pos; i < n - 1; i++)
                students[i] = students[i + 1];
            n--;
            saveRecords();
            logTransaction("Student Deleted");
            strcpy(res->message,
                   "Student Deleted Successfully");
            break;
        case 5:
            if (n == 0)
            {
                strcpy(res->message, "No Records Found");
                break;
            }
            res->count = n;
            for (i = 0; i < n; i++)
                res->s[i] = students[i];
            strcpy(res->message,
                   "All Student Records");
            break;
        case 6:
            if (n == 0)
            {
                strcpy(res->message, "No Records Found");
                break;
            }
            pos = 0;
            for (i = 1; i < n; i++)
            {
                if (students[i].cgpa >
                    students[pos].cgpa)
                    pos = i;
            }
            res->count = 1;
            res->s[0] = students[pos];
            strcpy(res->message,
                   "Class Topper Details");
            logTransaction("Topper Details Generated");
            break;
        case 7:
            strcpy(res->message,
                   "Session Terminated");
            break;
        default:
            strcpy(res->message, "Invalid Operation");
    }
}
int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    struct Request req;
    struct Response res;
    loadRecords();
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Socket Error");
        return 1;
    }
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET,
               SO_REUSEADDR, &opt, sizeof(opt));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&server,
         sizeof(server));
    listen(server_fd, 5);
    printf("Student Result Server Waiting...\n");
    while (1)
    {
        client_fd = accept(server_fd,
                    (struct sockaddr *)&client, &len);
        if (client_fd < 0)
            continue;
        printf("\nClient Connected\n");
        while (1)
        {
            if (recvall(client_fd, &req,
                        sizeof(req)) <= 0)
                break;
            processRequest(req, &res);
            if (sendall(client_fd, &res,
                        sizeof(res)) < 0)
                break;
            if (req.choice == 7)
                break;
        }
        close(client_fd);
        printf("Client Disconnected\n");
    }
    close(server_fd);
    return 0;
}


Client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8083
#define MAX 100
#define SUBJECTS 5
struct Student
{
    int regno;
    char name[50];
    char dept[30];
    int semester;
    int marks[SUBJECTS];
    char grade[5];
    float cgpa;
};
struct Request
{
    int choice;
    struct Student s;
};
struct Response
{
    int status;
    int count;
    char message[100];
    struct Student s[MAX];
};
int sendall(int fd, void *data, int size)
{
    int total = 0, bytes;
    while (total < size)
    {
        bytes = send(fd, (char *)data + total,
                     size - total, 0);
        if (bytes <= 0)
            return -1;
        total += bytes;
    }
    return total;
}
int recvall(int fd, void *data, int size)
{
    int total = 0, bytes;
    while (total < size)
    {
        bytes = recv(fd, (char *)data + total,
                     size - total, 0);
        if (bytes <= 0)
            return -1;
        total += bytes;
    }
    return total;
}
void inputStudent(struct Student *s)
{
    int i;
    printf("Enter Register Number: ");
    scanf("%d", &s->regno);
    printf("Enter Name: ");
    scanf(" %49[^\n]", s->name);
    printf("Enter Department: ");
    scanf(" %29[^\n]", s->dept);
    printf("Enter Semester: ");
    scanf("%d", &s->semester);
    printf("Enter Marks of 5 Subjects:\n");
    for (i = 0; i < SUBJECTS; i++)
    {
        printf("Subject %d: ", i + 1);
        scanf("%d", &s->marks[i]);
    }
}
void displayStudent(struct Student s)
{
    int i;
    printf("\n-----------------------------\n");
    printf("Register Number : %d\n", s.regno);
    printf("Name            : %s\n", s.name);
    printf("Department      : %s\n", s.dept);
    printf("Semester        : %d\n", s.semester);
    printf("Subject Marks   : ");
    for (i = 0; i < SUBJECTS; i++)
        printf("%d ", s.marks[i]);
    printf("\nGrade           : %s\n", s.grade);
    printf("CGPA            : %.2f\n", s.cgpa);
    printf("-----------------------------\n");
}
int main()
{
    int sock, choice, i;
    struct sockaddr_in server;
    struct Request req;
    struct Response res;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket Error");
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr =
        inet_addr("127.0.0.1");
    if (connect(sock, (struct sockaddr *)&server,
                sizeof(server)) < 0)
    {
        perror("Connection Failed");
        close(sock);
        return 1;
    }
    printf("Connected to Student Result Server\n");
    while (1)
    {
        printf("\n===== STUDENT RESULT MANAGEMENT =====\n");
        printf("1. Add Student Record\n");
        printf("2. Search Student Record\n");
        printf("3. Update Student Record\n");
        printf("4. Delete Student Record\n");
        printf("5. Display All Records\n");
        printf("6. Generate Class Topper\n");
        printf("7. Exit\n");
        printf("Enter Your Choice: ");
        scanf("%d", &choice);
        memset(&req, 0, sizeof(req));
        req.choice = choice;
        if (choice == 1 || choice == 3)
        {
            inputStudent(&req.s);
        }
        else if (choice == 2 || choice == 4)
        {
            printf("Enter Register Number: ");
            scanf("%d", &req.s.regno);
        }
        else if (choice < 1 || choice > 7)
        {
            printf("Invalid Choice!\n");
            continue;
        }
        if (sendall(sock, &req, sizeof(req)) < 0)
            break;
        if (recvall(sock, &res, sizeof(res)) <= 0)
            break;
        printf("\nServer Response: %s\n",
               res.message);
        for (i = 0; i < res.count; i++)
        {
            displayStudent(res.s[i]);
        }
        if (choice == 7)
            break;
    }
    close(sock);
    return 0;
}