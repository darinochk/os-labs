#ifndef LAB2_CACHE_H
#define LAB2_CACHE_H

#include <stddef.h>
#include <sys/types.h>

// Открытие файла по пути
int lab2_open(const char *path);

// Закрытие файла по inode
int lab2_close(int inode);

// Чтение данных из файла через кэш
ssize_t lab2_read(int inode, void *buf, size_t count);

// Запись данных в файл через кэш
ssize_t lab2_write(int inode, const void *buf, size_t count);

// Перестановка указателя позиции в файле
off_t lab2_lseek(int inode, off_t offset, int whence);

// Синхронизация кэша с диском
int lab2_fsync(int inode);

// Инициализация и завершение работы кэша
int lab2_cache_init(size_t cache_size, size_t block_size);
void lab2_cache_destroy();

#endif
