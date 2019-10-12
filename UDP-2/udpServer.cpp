#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
using namespace std;

#define PORT_NO 8080
#define BUFFER_SIZE 10048
#define nofile "File Not Found!"

long int find_size(const char *file_name)
{
    struct stat st;

    if(stat(file_name,&st)==0)
        return (st.st_size);
    else
        return -1;
}

int send_file(FILE* fp, char* buffer, int size)
{
    int i, len;
    if (fp == NULL) {
        strcpy(buffer, nofile);
        return 1;
    }

    char ch;
    for (i = 0; i < size; i++) {
        ch = fgetc(fp);
        buffer[i] = ch;
        if (feof(fp)){
            return 1;
        }
    }
    return 0;
}

int main()
{
    int sock_fd, number_of_bytes;
    struct sockaddr_in server_address;
    socklen_t address_length = sizeof(server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NO);
    server_address.sin_addr.s_addr = INADDR_ANY;
    char buffer[BUFFER_SIZE];
    FILE* fp;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock_fd < 0)
        cout << "\nSocket not created!!\n";
    else
        cout << "\nSocket ID " << sock_fd << " received\n";


    if (bind(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == 0)
        cout << "\nSuccessfully binded!\n";
    else
        cout << "\nBinding Failed!\n";

    cout << "\nWaiting for file name...\n";

    bzero(&buffer,sizeof(buffer));

    number_of_bytes = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_address, &address_length);

    fp = fopen(buffer, "r");
    cout << "\nFile Name Received: " << buffer << "\n";

    long file_size;

    if (fp == NULL) {
        cout << "File open failed!\n";
        file_size = 10048;
    }
    else {
        cout << "File Successfully opened!\n";
        file_size = find_size(buffer);
    }

    cout << "File size:" << file_size << endl;

    number_of_bytes += sendto(sock_fd, (char *) &file_size, sizeof(file_size), 0, (struct sockaddr*)&server_address, address_length);

    number_of_bytes = 0;
    int flag;
    while (1) {
        flag = send_file(fp, buffer, BUFFER_SIZE);
        if (flag) {
            number_of_bytes += sendto(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_address, address_length);
            break;
        }
        number_of_bytes += sendto(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_address, address_length);
        bzero(&buffer,sizeof(buffer));
    }

    if (fp != NULL)
        fclose(fp);

    cout << "\n" << number_of_bytes << " bytes sent to " << inet_ntoa(server_address.sin_addr) << "\n";

    return 0;
}
