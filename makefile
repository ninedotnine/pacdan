CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -g -pedantic -march=native -pipe -ggdb -Os -Werror -Wundef -Wcast-align -Wwrite-strings -Wunreachable-code -Wformat=2 -fdelete-null-pointer-checks -Winit-self -Wunused-result -Wstrict-aliasing -Wimplicit-fallthrough
FILES = src/main.c
OUT_EXE = bin/pacdan
LIBS = -l X11 -l pthread
# LIBS = -lpthread -lmath
# DEBUGFLAGS = -fsanitize=thread -fsanitize=undefined -fstack-protector-all -Wsuggest-attribute=pure
# DEBUGFLAGS = -fsanitize=thread -fsanitize=undefined -fstack-protector-all
# DEBUGFLAGS = -fsanitize=address -fsanitize=undefined -fstack-protector-all

build:
	mkdir -p bin
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -o $(OUT_EXE) $(FILES) $(LIBS)

clean:
	rm -f $(OUT_EXE)
