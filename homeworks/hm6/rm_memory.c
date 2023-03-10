#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

int main() {
    char pathname[] = "generate_file.txt";
    int *shm;
    int shmid;
    key_t key;
    
    if ((key = ftok(pathname, 1)) < 0) {
        printf("Error in frok\n");
        exit(-1);
    }
    
    if ((shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT)) < 0) {
    	printf("Error in shmget\n");
    	exit(-1);
    }
    
    int info;
    if ((info = shmctl(shmid, IPC_RMID, NULL)) < 0) {
        printf("Error in shmctl\n");
    }
}
