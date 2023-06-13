#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <string.h>   
#include <unistd.h>
#include <stdbool.h>

#define MSGSIZE 128


int main(int argc, char* argv[]) {
    int servSock;                   
    int clntSock;                   
    struct sockaddr_in servAddr; 
    struct sockaddr_in clntAddr;
    unsigned short port;   
    unsigned int clntLen;
    int msgSize;

    port = atoi(argv[1]);

    servSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&servAddr, 0, sizeof(servAddr));  
    servAddr.sin_family = AF_INET;               
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr));

    while (true) {
        clntLen = sizeof(clntAddr);
        
        char buf[MSGSIZE];
        msgSize = recvfrom(servSock, buf, MSGSIZE, 0, (struct sockaddr *) &clntAddr, &clntLen);
        printf("[MONITORING] %s\n", buf);
    }
}