#include "lab2_cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_FILES 128
#define CACHE_BLOCK_SIZE 4096

typedef struct {
    int fd;
    char *path;
    off_t offset;
    int valid;
} FileDescriptor;

typedef struct {
    void *data;
    off_t block_start;
    int valid;
    int referenced;
    int dirty;
    int inode;
} CacheBlock;

static CacheBlock *cache = NULL;
static size_t cache_size;
static size_t block_size;
static size_t num_blocks;
static int clock_hand;
static FileDescriptor files[MAX_FILES];

int lab2_cache_init(size_t c_size, size_t b_size) {
    cache_size = c_size;
    block_size = b_size;
    num_blocks = cache_size / block_size;

    cache = (CacheBlock*)malloc(num_blocks * sizeof(CacheBlock));
    if (!cache) {
        perror("malloc");
        return -1;
    }

    for (size_t i = 0; i < num_blocks; ++i) {
        cache[i].data = malloc(block_size);
        if (!cache[i].data) {
            perror("malloc");
            return -1;
        }
        cache[i].valid = 0;
        cache[i].referenced = 0;
        cache[i].dirty = 0;
        cache[i].inode = -1;
    }

    clock_hand = 0;
    memset(files, 0, sizeof(files));

    return 0;
}

void lab2_cache_destroy() {
    for (size_t i = 0; i < num_blocks; ++i) {
        if (cache[i].valid && cache[i].dirty) {
            lseek(files[cache[i].inode].fd, cache[i].block_start, SEEK_SET);
            ssize_t result = write(files[cache[i].inode].fd, cache[i].data, block_size);
            if (result < 0) {
                perror("write error");
            }
        }
        free(cache[i].data);
    }
    free(cache);
}

static int find_free_inode() {
    for (int i = 0; i < MAX_FILES; ++i) {
        if (!files[i].valid) {
            return i;
        }
    }
    return -1;
}

int lab2_open(const char *path) {
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    int index = find_free_inode();
    if (index < 0) {
        fprintf(stderr, "Too many open files\n");
        close(fd);
        return -1;
    }

    files[index].fd = fd;
    files[index].path = strdup(path);
    if (!files[index].path) {
        perror("strdup");
        close(fd);
        return -1;
    }
    files[index].offset = 0;
    files[index].valid = 1;

    return index;
}

int lab2_close(int inode) {
    if (inode < 0 || inode >= MAX_FILES || !files[inode].valid) {
        fprintf(stderr, "Invalid inode\n");
        return -1;
    }

    for (size_t i = 0; i < num_blocks; ++i) {
        if (cache[i].valid && cache[i].dirty && cache[i].inode == inode) {
            lseek(files[inode].fd, cache[i].block_start, SEEK_SET);
            ssize_t result = write(files[inode].fd, cache[i].data, block_size);
            if (result < 0) {
                perror("write error");
            }
        }
    }

    free(files[inode].path);
    files[inode].valid = 0;
    close(files[inode].fd);

    return 0;
}

ssize_t lab2_read(int inode, void *buf, size_t count) {
    if (inode < 0 || inode >= MAX_FILES || !files[inode].valid) {
        return -1;
    }

    off_t offset = files[inode].offset;
    size_t total_read = 0;

    while (count > 0) {
        size_t block_idx = offset / block_size;
        size_t block_offset = offset % block_size;

        int cache_idx = -1;
        for (size_t i = 0; i < num_blocks; ++i) {
            if (cache[i].valid && cache[i].block_start == (off_t)block_idx && cache[i].inode == inode) {
                cache_idx = i;
                break;
            }
        }

        if (cache_idx == -1) {
            while (cache[clock_hand].referenced) {
                cache[clock_hand].referenced = 0;
                clock_hand = (clock_hand + 1) % num_blocks;
            }

            if (cache[clock_hand].valid && cache[clock_hand].dirty) {
                lseek(files[cache[clock_hand].inode].fd, cache[clock_hand].block_start, SEEK_SET);
                ssize_t result = write(files[cache[clock_hand].inode].fd, cache[clock_hand].data, block_size);
                if (result < 0) {
                    return -1;
                }
            }

            lseek(files[inode].fd, block_idx * block_size, SEEK_SET);
            ssize_t bytes_read = read(files[inode].fd, cache[clock_hand].data, block_size);
            if (bytes_read < 0) {
                return -1;
            }

            cache[clock_hand].valid = 1;
            cache[clock_hand].referenced = 1;
            cache[clock_hand].dirty = 0;
            cache[clock_hand].block_start = (off_t)block_idx;
            cache[clock_hand].inode = inode;
            cache_idx = clock_hand;
            clock_hand = (clock_hand + 1) % num_blocks;
        }

        size_t bytes_to_copy = (count < block_size - block_offset) ? count : block_size - block_offset;
        memcpy((char *)buf + total_read, (char *)cache[cache_idx].data + block_offset, bytes_to_copy);

        offset += bytes_to_copy;
        count -= bytes_to_copy;
        total_read += bytes_to_copy;
    }

    files[inode].offset = offset;
    return total_read;
}

ssize_t lab2_write(int inode, const void *buf, size_t count) {
    if (inode < 0 || inode >= MAX_FILES || !files[inode].valid) {
        return -1;
    }

    off_t offset = files[inode].offset;
    size_t total_written = 0;

    while (count > 0) {
        size_t block_idx = offset / block_size;
        size_t block_offset = offset % block_size;

        int cache_idx = -1;
        for (size_t i = 0; i < num_blocks; ++i) {
            if (cache[i].valid && cache[i].block_start == (off_t)block_idx && cache[i].inode == inode) {
                cache_idx = i;
                break;
            }
        }

        if (cache_idx == -1) {
            while (cache[clock_hand].referenced) {
                cache[clock_hand].referenced = 0;
                clock_hand = (clock_hand + 1) % num_blocks;
            }

            if (cache[clock_hand].valid && cache[clock_hand].dirty) {
                lseek(files[cache[clock_hand].inode].fd, cache[clock_hand].block_start, SEEK_SET);
                ssize_t result = write(files[cache[clock_hand].inode].fd, cache[clock_hand].data, block_size);
                if (result < 0) {
                    return -1;
                }
            }

            lseek(files[inode].fd, block_idx * block_size, SEEK_SET);
            ssize_t bytes_read = read(files[inode].fd, cache[clock_hand].data, block_size);
            if (bytes_read < 0) {
                return -1;
            }

            cache[clock_hand].valid = 1;
            cache[clock_hand].referenced = 1;
            cache[clock_hand].dirty = 1;
            cache[clock_hand].block_start = (off_t)block_idx;
            cache[clock_hand].inode = inode;
            cache_idx = clock_hand;
            clock_hand = (clock_hand + 1) % num_blocks;
        }

        size_t bytes_to_copy = (count < block_size - block_offset) ? count : block_size - block_offset;
        memcpy((char *)cache[cache_idx].data + block_offset, (char *)buf + total_written, bytes_to_copy);

        offset += bytes_to_copy;
        count -= bytes_to_copy;
        total_written += bytes_to_copy;
    }

    files[inode].offset = offset;
    return total_written;
}

off_t lab2_lseek(int inode, off_t offset, int whence) {
    if (inode < 0 || inode >= MAX_FILES || !files[inode].valid) {
        return -1;
    }

    off_t new_offset;
    switch (whence) {
        case SEEK_SET:
            new_offset = offset;
            break;
        case SEEK_CUR:
            new_offset = files[inode].offset + offset;
            break;
        case SEEK_END:
            new_offset = lseek(files[inode].fd, 0, SEEK_END) + offset;
            break;
        default:
            return -1;
    }

    files[inode].offset = new_offset;
    return new_offset;
}

int lab2_fsync(int inode) {
    if (inode < 0 || inode >= MAX_FILES || !files[inode].valid) {
        return -1;
    }

    for (size_t i = 0; i < num_blocks; ++i) {
        if (cache[i].valid && cache[i].dirty && cache[i].inode == inode) {
            lseek(files[inode].fd, cache[i].block_start, SEEK_SET);
            ssize_t result = write(files[inode].fd, cache[i].data, block_size);
            if (result < 0) {
                return -1;
            }
        }
    }

    return 0;
}
