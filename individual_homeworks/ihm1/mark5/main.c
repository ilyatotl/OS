#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const int buf_len = 8192;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Incorrect numbre of files\n");
        exit(-1);
    }

    int result;
    int fifo_to_write, fifo_to_read;

    int n = atoi(argv[3]);

    if (n <= 0) {
        printf("Incorrect n value\n");
        exit(-1);
    }

    char name_to_read[] = "fifo_to_read";
    char name_to_write[] = "fifo_to_write";

    (void)umask(0);
    mknod(name_to_read, S_IFIFO | 0666, 0);
    mknod(name_to_write, S_IFIFO | 0666, 0);

    result = fork();
    if (result < 0) {
        printf("Can't fork child\n");
        exit(-1);
    } else if (result > 0) {
        if ((fifo_to_write = open(name_to_write, O_RDONLY)) < 0) {
            printf("Can\'t open FIFO for writting\n");
            exit(-1);
        }

        char buffer[buf_len];
        size_t read_bytes = read(fifo_to_write, buffer, buf_len);
        if (read_bytes < 0) {
            printf("Can\'t read bytes from pipe\n");
            exit(-1);
        }

        int outputFD;
        if ((outputFD = open(argv[2], O_WRONLY)) < 0) {
            printf("Can't open output file\n");
            exit(-1);
        }

        ssize_t write_bytes = write(outputFD, buffer, read_bytes);
        if (write_bytes != read_bytes) {
            printf("Can't write all bytes\n");
            exit(-1);
        }

        if (close(outputFD) < 0 || close(fifo_to_write) < 0) {
            printf("Can't close files\n");
        }

    } else {
        result = fork();
        if (result < 0) {
            printf("Can't fork child\n");
            exit(-1);
        } else if (result > 0) {
            if ((fifo_to_write = open(name_to_write, O_WRONLY)) < 0) {
                printf("Can\'t open FIFO for writting\n");
                exit(-1);
            }

            if ((fifo_to_read = open(name_to_read, O_RDONLY)) < 0) {
                printf("Can\'t open FIFO for writting\n");
                exit(-1);
            }

            char buffer[buf_len];
            size_t read_bytes = read(fifo_to_read, buffer, buf_len);
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
                    size_t write_bytes = write(fifo_to_write, ans, n);
                    if (write_bytes != n) {
                        printf("Can't write all bytes to pipe\n");
                        exit(-1);
                    }
                    found = true;
                    break;
                }
            }

            if (!found) {
                size_t write_bytes = write(fifo_to_write, "Substring not found", 19);
                if (write_bytes != 19) {
                    printf("Can't write all bytes to pipe\n");
                    exit(-1);
                }
            }

            if (close(fifo_to_write) < 0 || close(fifo_to_read) < 0) {
                printf("Can't close pipes\n");
            }

        } else {
            if ((fifo_to_read = open(name_to_read, O_WRONLY)) < 0) {
                printf("Can\'t open FIFO for writting\n");
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

            size_t write_bytes = write(fifo_to_read, read_buffer, read_bytes);
            if (write_bytes != read_bytes) {
                printf("Can't write all bytes to pipe\n");
                exit(-1);
            }

            if (close(inputFD) < 0 || close(fifo_to_read) < 0) {
                printf("Can't close pipes\n");
            }
        }
    }
}