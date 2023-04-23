#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
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

sem_t *sem;
sem_t *write_sem;

const char* sem_name = "/access-semaphore";
const char* sem_write_name = "/write-semaphore";

void finish(int nsig) {
    printf("There will be no division of the will, Don Enrique is risen\n");
    sem_unlink(sem_name);
    sem_unlink(sem_write_name);
    exit(0);
}

void* check_proportion(void* share) {
    sleep(rand() % 3 + 1);
    int s = *(int*)share;
    
    if (s < 0 || s > 100) {
        sem_wait(write_sem);
        printf("Lawyer is not honest - the proportion of great-grandchild can't be %d%%\n", s);
        honest = false;
        sem_post(write_sem);
    }

    sem_wait(sem);
    amountRecievedByChildren += amount * s / 100;
    sem_post(sem);
}

int main(int argc, char* argv[]) {
    (void)signal(SIGINT, finish);

    if (argc != 10) {
        fprintf(stderr, "the amount of the will and 8 shares are required\n");
        exit(1);
    }

    amount = atoi(argv[1]);

    int share[greatGrandsonCount];
    for (int i = 0; i < greatGrandsonCount; ++i) {
        share[i] = atoi(argv[i + 2]);
    }

    if ((sem = sem_open(sem_name, O_CREAT, 0666, 0)) == 0) {
        perror("sem: can not create semaphore\n");
        exit(-1);
    }
    if ((write_sem = sem_open(sem_write_name, O_CREAT, 0666, 0)) == 0) {
        perror("sem: can not create write semaphore\n");
        exit(-1);
    }
    sem_post(sem);
    sem_post(write_sem);

    srand(time(NULL));

    pthread_t greatGrandson[greatGrandsonCount];

    for (int i = 0; i < greatGrandsonCount; ++i) {
        pthread_create(&greatGrandson[i], 0, check_proportion, (void*)(&share[i]));
    }

    for (int i = 0; i < greatGrandsonCount; ++i) {
        pthread_join(greatGrandson[i], NULL);
    }

    sem_close(sem);
    sem_close(write_sem);
    sem_unlink(sem_name);
    sem_unlink(sem_write_name);

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