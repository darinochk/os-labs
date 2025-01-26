#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab2_cache.h"

int main() {
  
    if (lab2_cache_init(1024 * 1024, 4096) < 0) {
        fprintf(stderr, "Failed to initialize cache\n");
        return 1;
    }


    int fd = lab2_open("testfile.txt");
    if (fd < 0) {
        fprintf(stderr, "Failed to open file\n");
        lab2_cache_destroy();
        return 1;
    }


    char buffer[4096];
    ssize_t bytes_read = lab2_read(fd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        fprintf(stderr, "Failed to read from file\n");
    } else {
        printf("Read %zd bytes\n", bytes_read);
    }


    if (lab2_close(fd) < 0) {
        fprintf(stderr, "Failed to close file\n");
    }

    
    lab2_cache_destroy();
    return 0;
}
