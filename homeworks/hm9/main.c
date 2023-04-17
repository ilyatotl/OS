#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>

const int buf_len = 64;

int main() {
    int result;
    int fd[2];

    if (pipe(fd) < 0) {
        printf("Can't open pipe\n");
        exit(-1);
    }

    int parent_semid, child_semid;
    char  parent_pathname[] = "main.c";
    char  child_pathname[] = "test.c";
    key_t parent_key, child_key;
    
    parent_key = ftok(parent_pathname, 0);
    child_key = ftok(child_pathname, 0);

    if ((parent_semid = semget(parent_key, 1, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t create semaphore\n");
        exit(-1);
    }
    if ((child_semid = semget(child_key, 1, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t create semaphore\n");
        exit(-1);
    }

    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op  = 1;
    buf.sem_flg = 0;
    if (semop(parent_semid, &buf, 1) < 0) {
        printf("Can\'t add 1 to semaphore\n");
        exit(-1);
    }

    result = fork();

    if (result < 0) {
        printf("Can't fork child\n");
        exit(-1);
    } else if (result > 0) {
        struct sembuf mybuf;
        mybuf.sem_num = 0;
        mybuf.sem_op  = -1;
        mybuf.sem_flg = 0;

        if (semop(parent_semid, &mybuf, 1) < 0) {
            printf("Can\'t sub 1 from semaphore\n");
            exit(-1);
        }

        size_t write_bytes = write(fd[1], "message", 7);
        printf("Parent process write message\n");

        mybuf.sem_op = 1;
        if (semop(child_semid, &mybuf, 1) < 0) {
            printf("Can\'t add 1 to semaphore\n");
            exit(-1);
        }

        for (int i = 0; i < 9; ++i) {
            mybuf.sem_op = -1;
            if (semop(parent_semid, &mybuf, 1) < 0) {
                printf("Can\'t sub 1 from semaphore\n");
                exit(-1);
            }
            
            char buffer[buf_len];
            ssize_t read_bytes = read(fd[0], buffer, buf_len);
            if (read_bytes < 0) {
                printf("Can\'t read bytes from pipe\n");
                exit(-1);
            }
            printf("Parent process read message\n");

            size_t write_bytes = write(fd[1], "message", 7);
            printf("Parent process write message\n");

            mybuf.sem_op = 1;
            if (semop(child_semid, &mybuf, 1) < 0) {
                printf("Can\'t add 1 to semaphore\n");
                exit(-1);
            }
        }

        if (close(fd[1]) < 0 || close(fd[0]) < 0) {
            printf("Can't close pipes\n");
        }
    } else {
        struct sembuf mybuf;
        mybuf.sem_num = 0;
        mybuf.sem_flg = 0;

        for (int i = 0; i < 9; ++i) {
            mybuf.sem_op = -1;
            if (semop(child_semid, &mybuf, 1) < 0) {
                printf("Can\'t sub 1 from semaphore\n");
                exit(-1);
            }
            
            char buffer[buf_len];
            ssize_t read_bytes = read(fd[0], buffer, buf_len);
            if (read_bytes < 0) {
                printf("Can\'t read bytes from pipe\n");
                exit(-1);
            }
            printf("Child process read message\n");

            size_t write_bytes = write(fd[1], "message", 7);
            printf("Child process write message\n");

            mybuf.sem_op = 1;
            if (semop(parent_semid, &mybuf, 1) < 0) {
                printf("Can\'t add 1 to semaphore\n");
                exit(-1);
            }
        }

        mybuf.sem_op = -1;
        if (semop(child_semid, &mybuf, 1) < 0) {
            printf("Can\'t sub 1 from semaphore\n");
            exit(-1);
        }
            
        char buffer[buf_len];
        ssize_t read_bytes = read(fd[0], buffer, buf_len);
        if (read_bytes < 0) {
            printf("Can\'t read bytes from pipe\n");
            exit(-1);
        }
        printf("Child process read message\n");

        mybuf.sem_op = 1;
        if (semop(parent_semid, &mybuf, 1) < 0) {
            printf("Can\'t add 1 to semaphore\n");
            exit(-1);
        }

        if (close(fd[1]) < 0 || close(fd[0]) < 0) {
            printf("Can't close pipes\n");
        }

        semctl(parent_semid, 0, IPC_RMID, 0);
        semctl(child_semid, 0, IPC_RMID, 0);
    }
}
