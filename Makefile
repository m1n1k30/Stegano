CC = gcc
CFLAGS = -Wall -Wextra -g -I include -I C:/vcpkg/installed/x64-windows/include
LDFLAGS = -L C:/vcpkg/installed/x64-windows/lib -lpng16 -lzlib
SRC = src/main.c src/image.c src/steg.c src/utils.c
OUT = stegano

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)