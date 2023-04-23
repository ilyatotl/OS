#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>

const int greatGrandsonCount = 8;

int amount;
int amountRecievedByChildren = 0;
bool honest = true;

const char* sem_name = "/access-semaphore";
const char* sem_write_name = "/write-semaphore";

int sem;
int write_sem;

void finish(int nsig) {
    printf("There will be no division of the will, Don Enrique is risen\n");
    semctl(sem, 0, IPC_RMID, 0);
    semctl(write_sem, 0, IPC_RMID, 0);
    exit(0);
}

void* check_proportion(void* share) {
    sleep(rand() % 3 + 1);
    int s = *(int*)share;

    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op  = -1;
    buf.sem_flg = 0;
    
    if (s < 0 || s > 100) {
        if (semop(write_sem, &buf, 1) < 0) {
            printf("Can\'t sub 1 from semaphore\n");
            exit(-1);
        }
        printf("Lawyer is not honest - the proportion of great-grandchild can't be %d%%\n", s);
        honest = false;
        buf.sem_op = 1;
        if (semop(write_sem, &buf, 1) < 0) {
            printf("Can\'t add 1 to semaphore\n");
            exit(-1);
        }
    }

    buf.sem_op = -1;
    if (semop(sem, &buf, 1) < 0) {
        printf("Can\'t sub 1 from semaphore\n");
        exit(-1);
    }
    amountRecievedByChildren += amount * s / 100;
    buf.sem_op = 1;
    if (semop(sem, &buf, 1) < 0) {
        printf("Can\'t add 1 to semaphore\n");
        exit(-1);
    }
}

int main(int argc, char* argv[]) {
    (void)signal(SIGINT, finish);

    if (argc != 10) {
        fprintf(stderr, "the amount of the will and 8 shares are required\n");
        exit(1);
    }

    key_t key = ftok(sem_name, 0);
    key_t write_key = ftok(sem_write_name, 0);

    if ((sem = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t create semaphore\n");
        exit(-1);
    }

    if ((write_sem = semget(write_key, 1, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t create semaphore\n");
        exit(-1);
    }

    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op  = 1;
    buf.sem_flg = 0;
    if (semop(sem, &buf, 1) < 0) {
        printf("Can\'t add 1 to semaphore\n");
        exit(-1);
    }
    if (semop(write_sem, &buf, 1) < 0) {
        printf("Can\'t add 1 to write semaphore\n");
        exit(-1);
    }
    

    amount = atoi(argv[1]);

    int share[greatGrandsonCount];
    for (int i = 0; i < greatGrandsonCount; ++i) {
        share[i] = atoi(argv[i + 2]);
    }

    srand(time(NULL));

    pthread_t greatGrandson[greatGrandsonCount];

    for (int i = 0; i < greatGrandsonCount; ++i) {
        pthread_create(&greatGrandson[i], 0, check_proportion, (void*)(&share[i]));
    }

    for (int i = 0; i < greatGrandsonCount; ++i) {
        pthread_join(greatGrandson[i], NULL);
    }

    semctl(sem, 0, IPC_RMID, 0);
    semctl(write_sem, 0, IPC_RMID, 0);

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