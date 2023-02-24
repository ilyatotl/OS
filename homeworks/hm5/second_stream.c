#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const int buf_size = 64;
    char str[buf_size];

    char msg[3][64] = {"Hello, first!", "Good bye second"};
    int msgCnt = 2;


    char* pipeToRead = "first_write";
    char* pipeToWrite = "second_write";
    if (argc == 3) {
        pipeToRead = argv[1];
        pipeToWrite = argv[2];
    }
    
    (void) umask(0);
    mknod(pipeToWrite, S_IFIFO | 0666, 0);

    int fd;
    size_t size;
    for (int i = 0; i < msgCnt; ++i) {
        if ((fd = open(pipeToRead, O_RDONLY)) < 0) {
            printf("Can\'t open FIFO for reading\n");
            exit(-1);
        }
        size = read(fd, str, buf_size);
        if (size < 0) {
            printf("Can\'t read string from FIFO\n");
            exit(-1);
        }
        printf("Second received: %s\n", str);
        if (close(fd) < 0) {
            printf("Second: Can\'t close FIFO\n");
            exit(-1);
        }

        printf("Second writing: %s\n", msg[i]);
        if ((fd = open(pipeToWrite, O_WRONLY)) < 0) {
            printf("Can\'t open FIFO for writting\n");
            exit(-1);
        }
        int msg_size = sizeof(msg[i]);
        size = write(fd, msg[i], msg_size);
        if (size != msg_size) {
            printf("Can\'t write all string to FIFO\n");
            exit(-1);
        }
        if (close(fd) < 0) {
            printf("Second: Can\'t close FIFO\n");
            exit(-1);
        }
    }
    printf("Second pipe exit\n");
}
