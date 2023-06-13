#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <string.h>   
#include <unistd.h>
#include <stdbool.h>

#define MAXCONNECTIONS 1
#define MSGSIZE 4
#define MONITORINGMSGSIZE 128

char prop[MSGSIZE];

void sendToMonitoring(char* msg, char* argv[]) {
    int sock;                        
    struct sockaddr_in servAddr;
    char* servIP;   
    
    servIP = argv[2];
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&servAddr, 0, sizeof(servAddr));  
    servAddr.sin_family      = AF_INET;    
    servAddr.sin_addr.s_addr = inet_addr(servIP);
    servAddr.sin_port = htons(atoi(argv[6]));

    sendto(sock, msg, MONITORINGMSGSIZE, 0, (struct sockaddr*) &servAddr, sizeof(servAddr));
    close(sock);
}

int main(int argc, char* argv[]) {
    int servSock;                   
    int clntSock;                   
    struct sockaddr_in servAddr; 
    struct sockaddr_in clntAddr;
    unsigned short port;   
    unsigned int clntLen;   

    port = atoi(argv[1]);

    servSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&servAddr, 0, sizeof(servAddr));  
    servAddr.sin_family = AF_INET;               
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr));


    int sock;                        
    struct sockaddr_in addr;
    char *servIP;   
    
    servIP = argv[2];
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&addr, 0, sizeof(addr));  
    addr.sin_family      = AF_INET;    
    addr.sin_addr.s_addr = inet_addr(servIP);
    addr.sin_port = htons(atoi(argv[3]));
    
    sendto(sock, argv[4], MSGSIZE, 0, (struct sockaddr *) &addr, sizeof(addr));
    close(sock);

    char buffer[MSGSIZE];
    int msgSize = recvfrom(servSock, buffer, MSGSIZE, 0, (struct sockaddr *) &clntAddr, &clntLen);
    char* info = "[CLIENT %d] recieved his proportion - %d";
    printf("[CLIENT %d] recieved his proportion - %d\n", atoi(argv[4]), atoi(buffer));
    sendToMonitoring(info, argv);
    for (int i = 0; i < MSGSIZE; i++) {
        prop[i] = buffer[i];
    }

    int sockToCountingServer;                        
    struct sockaddr_in addrToCountingServer;
    
    sockToCountingServer = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&addrToCountingServer, 0, sizeof(addr));  
    addrToCountingServer.sin_family      = AF_INET;    
    addrToCountingServer.sin_addr.s_addr = inet_addr(servIP);
    addrToCountingServer.sin_port = htons(atoi(argv[5]));
    
    sendto(sockToCountingServer, prop, MSGSIZE, 0, (struct sockaddr *) &addrToCountingServer, sizeof(addrToCountingServer));
    info = "[CLIENT %d] send proportion - %d to counting server";
    printf("[CLIENT %d] send proportion - %d to counting server\n", atoi(argv[4]), atoi(prop));
    sendToMonitoring(info, argv);
    close(sockToCountingServer);
}
