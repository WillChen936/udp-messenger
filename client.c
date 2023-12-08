#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define serverPort 50500
#define serverIP "127.0.0.1"

int main() 
{
    // message buffer
    char buf[1024] = {0};
    char recvbuf[1024] = {0};
    
    // build socket fd, PF_INET = IPv4, SOCK_DGRAM = UDP
    int socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Fail to create a socket.");
        return -1;
    }

    // set server address
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(serverIP),
        .sin_port = htons(serverPort)
    };
    int len = sizeof(serverAddr);

    // Wait for user input and send the data to server.
    while (1) {
        // data input from user
        printf("Please input your message: ");
        scanf("%s", buf);

        // transport the data to server
        sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        // clear the message buffer.
        memset(buf, 0, sizeof(buf));

        // wait server's return data.
        if (recvfrom(socket_fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &len) < 0) {
            printf("recvfrom data from %s:%d, failed!\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
            break;
        }
        
        // Print the server's address and the return data.
        printf("get receive message from [%s:%d]: %s\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port), recvbuf);
        memset(recvbuf, 0, sizeof(recvbuf));
    }
    // close the socket
    if (close(socket_fd) < 0) {
        perror("close socket failed!");
    }

    return 0;
}