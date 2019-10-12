#include <stdio.h>
#include <math.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>

#define PORT_NO 10000
#define BUFFER_SIZE 10048
#define nofile "File Not Found!"

long find_size(const char *file_name) {
    struct stat st;
    if(stat(file_name,&st)==0)
        return (st.st_size);
    else
        return -1;
}

int send_file(FILE *fp, char* buffer, int size) {
    int i;
    if (fp==NULL) {
        strcpy(buffer, nofile);
        return 1;
    }

    char ch;
    for (i = 0; i < size; i++) {
        ch=fgetc(fp);
        buffer[i] = ch;
        if (feof(fp)) {
            return 1;
        }
    }
    return 0;
}

int isOperand(char c) { return (c >= '0' && c <= '9'); }

// utility function to find value of and operand
float value(char c) {  return (c - '0'); }

// This function evaluates simple expressions. It returns -1 if the
// given expression is invalid.
float evaluate(char *exp)
{
    // Base Case: Given expression is empty
    if (*exp == '\0')  return -1;

    // The first character must be an operand, find its value
    float res = value(exp[0]);

    // Traverse the remaining characters in pairs
    for (int i = 1; exp[i]; i += 2)
    {
        // The next character must be an operator, and
        // next to next an operand
        char opr = exp[i], opd = exp[i+1];

        // If next to next character is not an operand
        if (isOperand(opd) == 0)  return -1;

        // Update result according to the operator
        if (opr == '+')       res += value(opd);
        else if (opr == '-')  res -= value(opd);
        else if (opr == '*')  res *= value(opd);
        else if (opr == '/') {
            if(opd != 0) res /= value(opd);
            else return -1;
        }

        // If not a valid operator
        else return -1;
    }
    return res;
}

int main() {
    int server_fd, client_fd, number_of_bytes;
    struct sockaddr_in server, client;
    char choice[2];

    // setting up server
    server.sin_family = AF_INET;
	server.sin_port   = htons(PORT_NO);
	server.sin_addr.s_addr = INADDR_ANY;

    // creating server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // check socket successfully created or not
    if (server_fd < 0)
        printf("\nSocket not created!!\n");
    else
        printf("\nSocket ID %d received\n",server_fd);

    // binding the socket, and checking the bind
    if (bind(server_fd, (struct sockaddr*)&server, sizeof(server)) == 0)
        printf("\nBind successful!\n");
    else
        printf("\nBinding Failed!\n");

    // listening to maximum 5 client's request
    if (listen(server_fd, 5) < 0)
        printf("\nError in listening !\n");
    else
	   printf("\nWaiting for client operation...\n");

    socklen_t client_addr_len = sizeof(client);

    // accepting one client from queue
    client_fd = accept(server_fd, (struct sockaddr *)&client, &client_addr_len);
    printf("\nClient connected successfully\n");

    // getting the choice from user
    recv(client_fd, choice, 2, 0);
    printf("\nChoice received: %s\n", choice);

    char server_msg[100], client_msg[100];
    char buffer[BUFFER_SIZE];
    FILE* fp;
    char expression[20];
    char answer[20];

    switch(choice[0]) {
        case '1':
            printf("\n");
            while(1)
            {
                recv(client_fd, client_msg, 100, 0);
                printf("Client > %s", client_msg);
                if(client_msg[0]=='b' && client_msg[1]=='y' && client_msg[2]=='e')
                    break;
                printf("\nServer > ");
                scanf("%[^\n]%*c", server_msg);
                send(client_fd, server_msg, 100, 0);
            }
            break;
        case '2':
            bzero(&buffer,sizeof(buffer));
            recv(client_fd, buffer, BUFFER_SIZE, 0);
            fp = fopen(buffer, "r");
            long file_size;
            if (fp == NULL) {
                printf("File open failed!\n");
                file_size = 10048;
            }
            else {
                printf("File Successfully opened!\n");
                file_size = find_size(buffer);
            }
            printf("%d", file_size);
            send(client_fd, (char *)&file_size, sizeof(file_size), 0);
            int flag;
            while (1) {
                flag = send_file(fp, buffer, BUFFER_SIZE);
                if (flag) {
                    send(client_fd, buffer, BUFFER_SIZE, 0);
                    break;
                }
                send(client_fd, buffer,BUFFER_SIZE, 0);
                bzero(&buffer,sizeof(buffer));
            }
            if (fp != NULL)
                fclose(fp);
            break;
        case '3':
            recv(client_fd, expression, 20, 0);
            // printf("\nExpression received: %s\n", expression);
            float result = evaluate(expression);
            bzero(answer,sizeof(answer));
            // gcvt(result, 6, answer);
            sprintf(answer, "%.3f", result);
            send(client_fd, answer, 20, 0);
            break;
        case '4':
            close(server_fd);
            break;
    }

    if (server_fd)
        close(server_fd);
    return 0;
}
