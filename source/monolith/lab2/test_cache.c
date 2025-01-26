#include <stdio.h>
#include <string.h>
#include "lab2_cache.h"

#define TEST_FILE "testfile.txt"

void create_test_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Failed to create test file '%s'\n", filename);
        return;
    }
    fprintf(file, "Initial test data");
    fclose(file);
    printf("Test file '%s' created successfully.\n", filename);
}

int safe_open(const char *filename) {
    int fd = lab2_open(filename);
    if (fd == -1) {
        printf("Error: Failed to open file '%s'\n", filename);
    }
    return fd;
}

ssize_t safe_write(int fd, const void *buffer, size_t size) {
    ssize_t result = lab2_write(fd, buffer, size);
    if (result == -1) {
        printf("Error: Failed to write to file descriptor %d\n", fd);
    }
    return result;
}

ssize_t safe_read(int fd, void *buffer, size_t size) {
    ssize_t result = lab2_read(fd, buffer, size);
    if (result == -1) {
        printf("Error: Failed to read from file descriptor %d\n", fd);
    }
    return result;
}

void test_cache_write_read() {
    printf("Running test_cache_write_read...\n");

    int fd = safe_open(TEST_FILE);
    if (fd == -1) return;

    const char *data = "Testing cache write";
    if (safe_write(fd, data, strlen(data)) == -1) {
        lab2_close(fd);
        return;
    }
    printf("Data written: %s\n", data);

    if (lab2_lseek(fd, 0, SEEK_SET) == -1) {
        printf("Error: Failed to seek to the beginning of the file\n");
        lab2_close(fd);
        return;
    }

    char buffer[256] = {0};
    ssize_t read_bytes = safe_read(fd, buffer, sizeof(buffer) - 1);
    if (read_bytes == -1) {
        lab2_close(fd);
        return;
    }

    buffer[read_bytes] = '\0';
    printf("Read data: %s\n", buffer);

    lab2_close(fd);
}

void test_cache_seek() {
    printf("Running test_cache_seek...\n");

    int fd = safe_open(TEST_FILE);
    if (fd == -1) return;

    if (lab2_lseek(fd, 0, SEEK_SET) == -1) {
        printf("Error: Failed to seek to the beginning of the file\n");
        lab2_close(fd);
        return;
    }

    char buffer[256] = {0};
    ssize_t read_bytes = safe_read(fd, buffer, sizeof(buffer) - 1);
    if (read_bytes == -1) {
        lab2_close(fd);
        return;
    }

    buffer[read_bytes] = '\0';
    printf("Read after seek: %s\n", buffer);

    lab2_close(fd);
}

int main() {
    printf("Initializing cache...\n");

    if (lab2_cache_init(1024 * 1024, 4096) < 0) {
        printf("Error: Failed to initialize cache\n");
        return 1;
    }

    create_test_file(TEST_FILE);

    test_cache_write_read();
    test_cache_seek();

    printf("Destroying cache...\n");
    lab2_cache_destroy();

    printf("All tests completed successfully.\n");
    return 0;
}
