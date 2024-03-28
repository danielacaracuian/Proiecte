CC=gcc
CFLAGS=-Wall #-fsanitize=undefined -fsanitize=address 

build: tema3

tema3: main.c src/*.c
	$(CC) $(CFLAGS) -o $@ $^

manual: build
	cp input/cerinta2/test8.in tema3.in

clean:
	rm tema3*
