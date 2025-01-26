CC = gcc
CXX = g++  
CFLAGS = -Wall -Wextra -fPIC -O2
CXXFLAGS = -Wall -Wextra -fPIC -O2 
LDFLAGS = -shared
TARGET_LIB = libcache.so
TEST_PROGRAM = test_main
TEST_CACHE_PROGRAM = test_cache
GEMA_SORT_INT = gema-sort-int

all: $(TARGET_LIB) $(TEST_PROGRAM) $(TEST_CACHE_PROGRAM) $(GEMA_SORT_INT)

$(TARGET_LIB): lab2_cache.c lab2_cache.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET_LIB) lab2_cache.c

$(TEST_PROGRAM): test_main.c lab2_cache.h
	$(CC) $(CFLAGS) -o $(TEST_PROGRAM) test_main.c -L. -lcache

$(TEST_CACHE_PROGRAM): test_cache.c lab2_cache.h
	$(CC) $(CFLAGS) -o $(TEST_CACHE_PROGRAM) test_cache.c -L. -lcache

$(GEMA_SORT_INT): gema-sort-int.cpp lab2_cache.c lab2_cache.h
	$(CXX) $(CXXFLAGS) -o $(GEMA_SORT_INT) gema-sort-int.cpp lab2_cache.c -L. -lcache

clean:
	rm -f $(TARGET_LIB) $(TEST_PROGRAM) $(TEST_CACHE_PROGRAM) $(GEMA_SORT_INT)
