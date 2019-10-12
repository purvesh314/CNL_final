#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

#define PORT_NO 8080
#define BUFFER_SIZE 10048

int receive_file(char* buffer, int size, FILE *fp)
{
    int i;
    char ch;
    if (strcmp(buffer, "File Not Found!") == 0) {
        return 1;
    }
    for (i = 0; i < size; i++) {
        ch = buffer[i];
        fputc(ch, fp);
    }
    return 0;
}

int main(int argc, char**argv)
{
	int sock_fd, number_of_bytes;
   	struct sockaddr_in server_address;
	socklen_t address_length = sizeof(server_address);

	if (argc != 3)
	{
	     cout << "Usage: " << argv[0] << " <IP address of server> <Destination folder>\n";
	     return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NO);
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    char buffer[BUFFER_SIZE];
    FILE* fp;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock_fd < 0)
    {
        cout << "\nSocket Not Created\n";
    }
    else
        cout << "\nSocket Id is " << sock_fd << "\n";

    cout << "\nPlease enter file name to receive: ";
    cin >> buffer;
    sendto(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_address, address_length);

    fp = fopen(strcat(argv[2], buffer), "w");

    long file_size;

    number_of_bytes = recvfrom(sock_fd, (char *)&file_size, sizeof(file_size), 0, (struct sockaddr*)&server_address, &address_length);

    cout << "File size: " << file_size << endl;

    number_of_bytes = 0;

    if (fp > 0) {
        while (number_of_bytes <= file_size + 1) {
            bzero(buffer,sizeof(buffer));
            // cout << "\n" << number_of_bytes << " of bytes received from IP " << inet_ntoa(server_address.sin_addr) << "\n";

            number_of_bytes += recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_address, &address_length);

            if (receive_file(buffer, BUFFER_SIZE, fp)) {
                if (strcmp(buffer, "File Not Found!") == 0) {
                    cout << "\nFile not found at server side!\n";
                    return -1;
                }
            }
        }
        fclose(fp);
        cout << "Data received successfully!\n";
        cout << "\n" << number_of_bytes << " of bytes received from IP " << inet_ntoa(server_address.sin_addr) << "\n";
    } else {
        cout << "\nFolder specified does not exist!\n";
    }

}
