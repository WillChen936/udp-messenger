#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BASE 500 // ms
#define MULTIPILER 2
#define MAX_WAIT_INTERVAL 8000 // ms
#define BUFFER_SIZE 1024

int main() 
{
    // Init the configurations.
    char sendbuf[BUFFER_SIZE] = {0};
    char recvbuf[BUFFER_SIZE] = {0};
    char ip[10] = {0};
    int port = 0;
    int max_retry = 10;
    printf("Please enter the IP address you want to listen to : ");
    scanf("%s", ip);
    printf("Please enter the Port you want to listen to : ");
    scanf("%d", &port);
    printf("How many times do you want to retry(if the wait interval < 8 secs) : ");
    scanf("%d", &max_retry);
    // Skip the "\n" symbol in the stdin.
    getchar();
    
    // build socket fd, PF_INET = IPv4, SOCK_DGRAM = UDP
    int socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
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
    int len = sizeof(serverAddr);

    // Wait for user input and send the data to server.
    // data input from user
    printf("Please input your message: ");
    fgets(sendbuf, BUFFER_SIZE, stdin);

    // transport the data to server
    sendto(socket_fd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // clear the message buffer.
    memset(sendbuf, 0, sizeof(sendbuf));


    struct timeval tv;
    int failures = 0;
    int wait_interval = BASE;

    // Send & RECV with retry function
    while(failures < max_retry && wait_interval <= MAX_WAIT_INTERVAL) {
        tv.tv_sec = wait_interval / 1000;
        tv.tv_usec = (wait_interval % 1000) * 1000;
        setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        // wait server's return data.
        if (recvfrom(socket_fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &len) < 0) {
            failures++;
            printf("recvfrom data from %s:%d, failed!, failures = %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port), failures);
        }
        else {
            // Print the server's address and the return data.
            printf("get receive message from [%s:%d]: %s\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port), recvbuf);
            memset(recvbuf, 0, sizeof(recvbuf));
            // close the socket
            if (close(socket_fd) < 0) {
                perror("close socket failed!");
            }
            exit(0);
        }
        wait_interval = BASE * pow(MULTIPILER, failures);
    }
    

    printf("Communicating with server failed, Close.\n");
    // close the socket
    if (close(socket_fd) < 0) {
        perror("close socket failed!");
    }

    exit(1);
}