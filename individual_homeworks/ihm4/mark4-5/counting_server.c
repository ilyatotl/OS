#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <string.h>   
#include <unistd.h>
#include <stdbool.h>

#define MAXCONNECTIONS 8
#define MSGSIZE 4

const int ChildCount = 8;

int currentIdx = 0;
bool honest = true;


int main(int argc, char* argv[]) {
    int proportions[ChildCount];
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

    int amount = atoi(argv[2]);
    int amountRecievedByChildren = 0;

    for (int i = 0; i < 8; ++i) {
        clntLen = sizeof(clntAddr);

        char buffer[MSGSIZE];
        int msgSize = recvfrom(servSock, buffer, MSGSIZE, 0, (struct sockaddr *) &clntAddr, &clntLen);


        printf("[SERVER COUNTING] recieved proportion - %d\n", atoi(buffer));
        int proportion = atoi(buffer);
        if (proportion < 0 || proportion > 100) {
            honest = false;
        }
        currentIdx++;
        proportions[i] = proportion;

        amountRecievedByChildren += amount * proportions[i] / 100;
    }

    if (amountRecievedByChildren == amount && honest) {
        printf("Everything is correct, lawyer is honest\n");
    } else if (amountRecievedByChildren == amount) {
        printf("The lawyer did not take anything for himself, but he counted incorrectly\n");
    } else if (amountRecievedByChildren > amount) {
        printf("Lawyer was wrong, great-grandchildren will receive more than was bequeathed\n");     
    } else {
        printf("Lawyer was not honest he took some of the money\n");
    }
}