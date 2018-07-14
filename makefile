CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -g -pedantic -march=native -O2
FILES = src/pacdan.c
OUT_EXE = bin/pacdan
LIBS = -lX11
# LIBS = -lpthread -lmath
DEBUGFLAGS = -fsanitize=thread -fsanitize=undefined -fstack-protector-all
# DEBUGFLAGS = -fsanitize=address -fsanitize=undefined -fstack-protector-all

build:
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUT_EXE) $(FILES) $(LIBS)

clean:
	rm -f $(OUT_EXE)
