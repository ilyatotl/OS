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
    if (argc != 4) {
        printf("Incorrect numbre of files\n");
        exit(-1);
    }

    int result;
    int fd[2];

    int n = atoi(argv[3]);

    if (n <= 0) {
        printf("Incorrect n value\n");
        exit(-1);
    }

    if (pipe(fd) < 0) {
        printf("Can't open pipe\n");
        exit(-1);
    }

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


    result = fork();
    if (result < 0) {
        printf("Can't fork child\n");
        exit(-1);
    } else if (result > 0) {
        int inputFD;
        if ((inputFD = open(argv[1], O_RDONLY)) < 0) {
            printf("Can't open input file\n");
            exit(-1);
        }

        char read_buffer[buf_len];
        ssize_t read_bytes = read(inputFD, read_buffer, buf_len);
        if (read_bytes == -1) {
    	    printf("Can\'t read from input file\n");
    	    exit(-1);
        }

        size_t write_bytes = write(fd[1], read_buffer, read_bytes);
        if (write_bytes != read_bytes) {
            printf("Can't write all bytes to pipe\n");
            exit(-1);
        }

        sem_post(sem_read);
        sem_wait(sem_write);

        char buffer[buf_len];
        read_bytes = read(fd[0], buffer, buf_len);
        if (read_bytes < 0) {
            printf("Can\'t read bytes from pipe\n");
            exit(-1);
        }

        int outputFD;
        if ((outputFD = open(argv[2], O_WRONLY)) < 0) {
            printf("Can't open output file\n");
            exit(-1);
        }

        write_bytes = write(outputFD, buffer, read_bytes);
        if (write_bytes != read_bytes) {
            printf("Can't write all bytes\n");
            exit(-1);
        }

        if (close(inputFD) < 0 || close(outputFD) < 0 || close(fd[0]) < 0 || close(fd[1])) {
            printf("Can't close files\n");
        }

        if (sem_close(sem_read) == -1) {
            perror("Incorrect close of read semaphore\n");
            exit(-1);
        }
        if (sem_close(sem_write) == -1) {
            perror("Incorrect close of write semaphore\n");
            exit(-1);
        }

        if (sem_unlink(sem_read_name) == -1 || sem_unlink(sem_write_name) == -1) {
            perror("Incorrect unlink of semaphores\n");
            exit(-1);
        }

    } else {
        sem_wait(sem_read);
        char buffer[buf_len];
        size_t read_bytes = read(fd[0], buffer, buf_len);
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
                size_t write_bytes = write(fd[1], ans, n);
                if (write_bytes != n) {
                    printf("Can't write all bytes to pipe\n");
                    exit(-1);
                }
                found = true;
                break;
            }
        }

        if (!found) {
            size_t write_bytes = write(fd[1], "Substring not found", 19);
            if (write_bytes != 19) {
                printf("Can't write all bytes to pipe\n");
                exit(-1);
            }
        }
    
        sem_post(sem_write);

        if (close(fd[1]) < 0 || close(fd[0]) < 0) {
            printf("Can't close pipes\n");
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
}