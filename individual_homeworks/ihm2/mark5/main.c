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

sem_t sem;
sem_t write_sem;

void finish(int nsig) {
    printf("There will be no division of the will, Don Enrique is risen\n");
    sem_destroy(&sem);
    sem_destroy(&write_sem);
    exit(0);
}

void* check_proportion(void* share) {
    sleep(rand() % 3 + 1);
    int s = *(int*)share;
    
    if (s < 0 || s > 100) {
        sem_wait(&write_sem);
        printf("Lawyer is not honest - the proportion of great-grandchild can't be %d%%\n", s);
        honest = false;
        sem_post(&write_sem);
    }

    sem_wait(&sem);
    amountRecievedByChildren += amount * s / 100;
    sem_post(&sem);
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

    sem_init(&sem, 0, 1);
    sem_init(&write_sem, 0, 1);

    srand(time(NULL));

    pthread_t greatGrandson[greatGrandsonCount];

    for (int i = 0; i < greatGrandsonCount; ++i) {
        pthread_create(&greatGrandson[i], 0, check_proportion, (void*)(&share[i]));
    }

    for (int i = 0; i < greatGrandsonCount; ++i) {
        pthread_join(greatGrandson[i], NULL);
    }

    sem_destroy(&sem);
    sem_destroy(&write_sem);

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