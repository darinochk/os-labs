#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lab2_cache.h"

#define TEST_FILE "testfile.txt"

void create_test_file(const char *filename, size_t size) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to create test file");
        exit(1);
    }

    char buffer[4096];
    memset(buffer, 'A', sizeof(buffer));
    size_t full_blocks = size / sizeof(buffer);
    size_t remaining_bytes = size % sizeof(buffer);

    for (size_t i = 0; i < full_blocks; i++) {
        if (write(fd, buffer, sizeof(buffer)) != sizeof(buffer)) {
            perror("Failed to write to test file");
            close(fd);
            exit(1);
        }
    }

    if (remaining_bytes > 0) {
        if (write(fd, buffer, remaining_bytes) != (ssize_t)remaining_bytes) {
            perror("Failed to write remaining bytes");
            close(fd);
            exit(1);
        }
    }

    close(fd);
}


int main() {
    if (lab2_cache_init(1024 * 1024, 4096) < 0) {
        fprintf(stderr, "Failed to initialize cache\n");
        return 1;
    }

    create_test_file(TEST_FILE, 16 * 1024);

    int fd = lab2_open(TEST_FILE);
    if (fd < 0) {
        perror("Failed to open file");
        lab2_cache_destroy();
        return 1;
    }

    char read_buffer[4096];
    printf("Reading block 0...\n");
    ssize_t read_bytes = lab2_read(fd, read_buffer, sizeof(read_buffer));
    if (read_bytes < 0) {
        perror("Failed to read");
        lab2_close(fd);
        lab2_cache_destroy();
        return 1;
    }
    printf("Read %ld bytes\n", read_bytes);

    printf("Reading block 0 again (should hit cache)...\n");
    read_bytes = lab2_read(fd, read_buffer, sizeof(read_buffer));
    if (read_bytes < 0) {
        perror("Failed to read");
        lab2_close(fd);
        lab2_cache_destroy();
        return 1;
    }
    printf("Read %ld bytes\n", read_bytes);

    char write_buffer[4096];
    memset(write_buffer, 'B', sizeof(write_buffer));
    printf("Writing block 0...\n");
    ssize_t written_bytes = lab2_write(fd, write_buffer, sizeof(write_buffer));
    if (written_bytes < 0) {
        perror("Failed to write");
        lab2_close(fd);
        lab2_cache_destroy();
        return 1;
    }
    printf("Written %ld bytes\n", written_bytes);

    printf("Synchronizing data with disk...\n");
    if (lab2_fsync(fd) < 0) {
        perror("Failed to sync");
        lab2_close(fd);
        lab2_cache_destroy();
        return 1;
    }
    printf("Data synchronized.\n");

    if (lab2_close(fd) < 0) {
        perror("Failed to close file");
        lab2_cache_destroy();
        return 1;
    }
    printf("Test completed successfully.\n");

    lab2_cache_destroy();
    return 0;
}
