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
    if (argc != 3) {
        printf("Incorrect numbre of files\n");
        exit(-1);
    }

    int fifo;
    char fifo_name[] = "fifo";

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

    size_t write_bytes = write(fifo, read_buffer, read_bytes);
    if (write_bytes != read_bytes) {
        printf("Can't write all bytes to pipe\n");
        exit(-1);
    }

    sem_post(sem_read);
    sem_wait(sem_write);

    char buffer[buf_len];
    read_bytes = read(fifo, buffer, buf_len);
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

    if (close(inputFD) < 0 || close(outputFD) < 0 || close(fifo) < 0) {
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
}