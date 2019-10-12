#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT_NO 10000
#define BUFFER_SIZE 10048

int receive_file(char* buffer, int size, FILE *fp) {
    int i;
    char ch;
    if (strcmp(buffer, "File Not Found!") == 0) {
        return 1;
    }
    for (i = 0; i < size; i++) {
        ch = buffer[i];
        fputc(ch,fp);
    }
    return 0;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: %s <IP address of server>\n", argv[0]);
        return -1;
    }

    // creating client socket
	int client_fd = socket(AF_INET,SOCK_STREAM,0);
    printf("\nSocket created successfully\n");
    char choice[2];

    // defining the server
    struct sockaddr_in server;
    server.sin_family = AF_INET;
	server.sin_port   = htons(PORT_NO);
	server.sin_addr.s_addr = inet_addr(argv[1]);

    // connecting to server
    if (connect(client_fd, (struct sockaddr *)&server, sizeof(server)) == 0)
        printf("\nConnection established\n");
    else {
        printf("\nConnection failed!\n");
        return -1;
    }

    printf("\n------------  Menu  -------------\n");
    printf("1. Chat App\n");
    printf("2. File Transfer App\n");
    printf("3. Calculator App\n");
    printf("4. Exit\n");
    printf("Enter your choice > ");
    scanf("%s", choice);

    send(client_fd, choice, 2, 0);

    char client_msg[100], server_msg[100], file_size_char[100];
    char buffer[BUFFER_SIZE]; char destination[100];
    char expression[20];
    int number_of_bytes;
    long file_size;
    double result;
    char answer[20];
    FILE* fp;

    switch(choice[0]) {
        case '1':
            while(1) {
                printf("\nClient > ");
                scanf(" %[^\n]%*c", client_msg);
                send(client_fd, client_msg, 100, 0);
                if(client_msg[0]=='b' && client_msg[1]=='y' && client_msg[2]=='e')
                    break;
                recv(client_fd, server_msg, 100, 0);
                printf("Server > %s", server_msg);
            }
            break;
        case '2':
            printf("Enter the name of the file: ");
            scanf("%s", buffer);
            printf("Enter the destination to store the file: ");
            scanf("%s", destination);
            send(client_fd, buffer, 20, 0);
            fp = fopen(strcat(destination, buffer), "w");
            recv(client_fd, (char *)&file_size, sizeof(file_size), 0);
            // file_size = atoi(file_size_char);
            // printf("%d", file_size);
            number_of_bytes = 0;
            if (fp > 0) {
                while (number_of_bytes <= file_size + 1) {
                    bzero(buffer,sizeof(buffer));
                    number_of_bytes += recv(client_fd, buffer, BUFFER_SIZE, 0);
                    if (receive_file(buffer, BUFFER_SIZE, fp)) {
                        if (strcmp(buffer, "File Not Found!") == 0) {
                            printf("\nFile not found at server side!\n");
                            return -1;
                        }
                    }
                }
                fclose(fp);
                printf("Data received successfully!\n");
            } else {
                printf("\nFolder specified does not exist!\n");
            }
            break;
        case '3':
            printf("\nEnter the expression: ");
            scanf(" %[^\n]%*c", expression);
            send(client_fd, expression, 20, 0);
            bzero(answer,sizeof(answer));
			recv(client_fd, answer, 20, 0);
            result = atof(answer);
            if (result == -1) {
                printf("\n%s is Invalid!\n", expression);
            } else {
                printf("\nValue of %s is %lf\n", expression, result);
            }
            break;
        case '4':
            close(client_fd);
            break;
        default:
            printf("\nWrong option!\n");
    }

    if (client_fd)
        close(client_fd);
    return 0;
}
