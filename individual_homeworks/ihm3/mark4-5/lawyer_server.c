#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <string.h>   
#include <unistd.h>
#include <stdbool.h>

#define MAXCONNECTIONS 16
#define MSGSIZE 4

void sendResponse(char* msg, char* argv[]) {
    int sock;                        
    struct sockaddr_in servAddr;
    char* servIP;   
    
    servIP = argv[2];
    int child_id = atoi(msg);

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    memset(&servAddr, 0, sizeof(servAddr));  
    servAddr.sin_family      = AF_INET;    
    servAddr.sin_addr.s_addr = inet_addr(servIP);
    servAddr.sin_port = htons(atoi(argv[child_id + 2]));
    
    connect(sock, (struct sockaddr*) &servAddr, sizeof(servAddr));
    send(sock, argv[child_id + 11], MSGSIZE, 0);
    printf("[SERVER LAWYER] Send response %s to child with id=%s\n", argv[child_id + 11],  msg);  
    close(sock);
}

void handlerClient(int clntSocket, char* argv[]) { 
    char buf[MSGSIZE];      
    int msgSize = recv(clntSocket, buf, MSGSIZE, 0);
 
    printf("[SERVER LAWYER] Got request from child with id=%s\n", buf);  
    sendResponse(buf, argv);

    close(clntSocket);
}

int main(int argc, char* argv[]) {
    int servSock;                   
    int clntSock;                   
    struct sockaddr_in servAddr; 
    struct sockaddr_in clntAddr;
    unsigned short port;   
    unsigned int clntLen;   

    port = atoi(argv[1]);

    servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    memset(&servAddr, 0, sizeof(servAddr));  
    servAddr.sin_family = AF_INET;               
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr));
    listen(servSock, MAXCONNECTIONS);

    while (true) {
        clntLen = sizeof(clntAddr);

        clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen);
        handlerClient(clntSock, argv);
    }
}