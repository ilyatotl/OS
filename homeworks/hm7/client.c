// gcc client.c -o ./client -lrt
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

int main () {
    char memn[] = "shared-memory";
    char *addr;
    int shm;
    int mem_size = 1;

    if ((shm = shm_open(memn, O_RDWR | O_CREAT, 0666)) == -1 ) {
        perror("Opening error\n");
        return -1;
    }
  
    if (ftruncate(shm, 1) == -1) {
        perror("Error in truncate\n");
        return -1;
    }

    addr = mmap(0, mem_size, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);
    if (addr == (char*)-1 ) {
        perror("Error getting pointer to shared memory\n");
        return -1;
    }

    srand(time(NULL));
    int val = rand() % 10;
    addr[0] = val;
    printf("Client generate %d\n", val);

    return 0;
}
