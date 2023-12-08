#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main()
{
    // Init the configurations.
    char buf[1024] = {0};
    char ip[10] = {0};
    int port = 0;
    printf("Please enter the IP address you want to listen to : ");
    scanf("%s", ip);
    printf("Please enter the Port you want to listen to : ");
    scanf("%d", &port);

    // build socket fd, PF_INET = IPv4, SOCK_DGRAM = UDP
    int socket_fd = socket(PF_INET , SOCK_DGRAM , 0);
    if (socket_fd < 0) {
        perror("Fail to create a socket.");
        return -1;
    }
    
    // set server address
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(ip),
        .sin_port = htons(port)
    };
    
    // bind the socket to specific port
    if (bind(socket_fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind socket failed!");
        close(socket_fd);
        exit(0);
    }
    
    printf("Server ready!\n");

    struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);
    // Listen to client
    while (1) {
        // recvfrom() will be invoked if there's someone transport the data via port 50500 with UDP.
        // Then write the data to clientAddr.
        if (recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &len) < 0) {
            perror("Recieve data error.");
            break;
        }

        // Print the source & data
        printf("get message from [%s:%d]: ", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        printf("%s\n", buf);

        // return the data by the info of clientAddr.
        sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));

        // clear the message buffer.
        memset(buf, 0, sizeof(buf));
    }

    // close the socket
    if (close(socket_fd) < 0) {
        perror("close socket failed!");
    }
    
    return 0;
}