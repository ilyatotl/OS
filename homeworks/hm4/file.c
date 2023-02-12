#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

// gcc file.c -o ./file
// ./file file.c new_file

const int buf_len = 8192;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Need 2 files\n");
        exit(-1);
    }
    
    int inputFD, outputFD;
    
    if ((inputFD = open(argv[1], O_RDONLY)) < 0) {
        printf("Can\'t open input file\n");
        exit(-1);
    }
    
    if ((outputFD = open(argv[2], O_WRONLY | O_CREAT, 0666)) < 0 ) {
        printf("Can\'t open output file\n");
        exit(-1);
    }
    
    char buffer[buf_len];
    
    ssize_t read_bytes = read(inputFD, buffer, buf_len);
    if (read_bytes == -1) {
    	printf("Can\'t read from this file\n");
    	exit(-1);
    }
    
    size_t write_bytes = write(outputFD, buffer, read_bytes);
    
    if (write_bytes != read_bytes) {
        printf("can\'t write all string\n");
        exit(-1);
    }
    
    if (close(inputFD) < 0 || close(outputFD) < 0) {
        printf("Can\'t close files\n");
    }
    return 0;
}
