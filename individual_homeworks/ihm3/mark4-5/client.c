#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <string.h>   
#include <unistd.h>
#include <stdbool.h>

#define MAXCONNECTIONS 1
#define MSGSIZE 4

char prop[MSGSIZE];

void handlerClient(int clntSocket, char* argv[]) {
    char buffer[MSGSIZE];        
    int msgSize = recv(clntSocket, buffer, MSGSIZE, 0);
    printf("[CLIENT %d] recieved his proportion - %d\n", atoi(argv[4]), atoi(buffer));
    for (int i = 0; i < MSGSIZE; i++) {
        prop[i] = buffer[i];
    }
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


    int sock;                        
    struct sockaddr_in addr;
    char *servIP;   
    
    servIP = argv[2];
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&addr, 0, sizeof(addr));  
    addr.sin_family      = AF_INET;    
    addr.sin_addr.s_addr = inet_addr(servIP);
    addr.sin_port = htons(atoi(argv[3]));
    
    connect(sock, (struct sockaddr *) &addr, sizeof(addr));
    send(sock, argv[4], MSGSIZE, 0);
    close(sock);

    clntLen = sizeof(clntAddr);
    clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen);
    handlerClient(clntSock, argv);

    int sockToCountingServer;                        
    struct sockaddr_in addrToCountingServer;
    
    sockToCountingServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&addrToCountingServer, 0, sizeof(addr));  
    addrToCountingServer.sin_family      = AF_INET;    
    addrToCountingServer.sin_addr.s_addr = inet_addr(servIP);
    addrToCountingServer.sin_port = htons(atoi(argv[5]));
    

    connect(sockToCountingServer, (struct sockaddr *) &addrToCountingServer, sizeof(addrToCountingServer));
    send(sockToCountingServer, prop, MSGSIZE, 0);
    printf("[CLIENT %d] send proportion - %d to counting server\n", atoi(argv[4]), atoi(prop));
    close(sockToCountingServer);
}
