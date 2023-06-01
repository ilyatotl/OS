#include <unistd.h>

#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>

int main() {
    char* path = "new_file.txt";
    char link[] = "link00000000.txt";
    char prev_link[] = "link00000000.txt";

    int fd;
    if ((fd = creat(path, S_IRUSR | S_IWUSR)) < 0) {
        perror("error in creating");
        return -1;
    }
    close(fd);
    int pointer = 11;
    int depth = 1;
    remove(link);
    if (symlink(path, link) != 0) {
        perror("error in symlink");
    }
    link[pointer]++;
    depth++;

    while (true) {
        remove(link);
        if (symlink(prev_link, link) != 0) {
            perror("error in symlink");
        }
        if (access(link, F_OK) == -1) {
            break;
        }
        if (link[pointer] == '9') {
            pointer--;
            prev_link[pointer + 1]++;
            link[pointer]++;
        } else {
            prev_link[pointer]++;
            link[pointer]++;
        }

        depth++;
    }

    printf("%d\n", depth);
}