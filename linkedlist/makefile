CFLAGS =
LDFLAGS =

main: main.c mylinkedlist.h
	gcc $(CFLAGS) -o main main.c $(LDFLAGS)

.PHONY: run clean

run: main
	clear && ./main

clean:
	rm -f main
