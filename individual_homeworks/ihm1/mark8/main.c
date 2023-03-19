#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <semaphore.h>

const int buf_len = 8192;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Incorrect numbre of arguments\n");
        exit(-1);
    }

    int fifo;
    char fifo_name[] = "fifo";
    int n = atoi(argv[1]);

    if (n <= 0) {
        printf("Incorrect n value\n");
        exit(-1);
    }

    (void)umask(0);
    mknod(fifo_name, S_IFIFO | 0666, 0);

    const char* sem_read_name = "/read-semaphore";
    const char* sem_write_name = "/write-semaphore";
    sem_t *sem_read, *sem_write;
    
    if ((sem_read = sem_open(sem_read_name, O_CREAT, 0666, 0)) == 0) {
        perror("sem_read: can not create semaphore\n");
        exit(-1);
    }
    if ((sem_write = sem_open(sem_write_name, O_CREAT, 0666, 0)) == 0) {
        perror("sem_write: can not create semaphore\n");
        exit(-1);
    }

    if ((fifo = open(fifo_name, O_RDWR)) < 0) {
        printf("Can\'t open FIFO\n");
        exit(-1);
    }

    sem_wait(sem_read);
    char buffer[buf_len];
    size_t read_bytes = read(fifo, buffer, buf_len);
    if (read_bytes < 0) {
        printf("Can\'t read bytes from pipe\n");
        exit(-1);
    }
            
    int m = (int)read_bytes;
    bool found = false;
    for (int i = m - n; i >= 0; --i) {
        bool ok = true;
        for (int j = i + 1; j <= i + n - 1; ++j) {
            if (buffer[j] <= buffer[j - 1]) {
                ok = false;
                 break;
            }
        }

        if (ok) {
            char ans[n];
            for (int j = i; j <= i + n - 1; ++j) {
                ans[j - i] = buffer[j];
            }
            size_t write_bytes = write(fifo, ans, n);
            if (write_bytes != n) {
                printf("Can't write all bytes to pipe\n");
                exit(-1);
            }
            found = true;
            break;
        }
    }

    if (!found) {
        size_t write_bytes = write(fifo, "Substring not found", 19);
        if (write_bytes != 19) {
            printf("Can't write all bytes to pipe\n");
            exit(-1);
        }
    }
    
    sem_post(sem_write);

    if (close(fifo) < 0) {
        printf("Can't close fifo\n");
    }

    if (sem_close(sem_read) == -1) {
        perror("Incorrect close of read semaphore\n");
        exit(-1);
    }
    if (sem_close(sem_write) == -1) {
        perror("Incorrect close of write semaphore\n");
        exit(-1);
    }
}