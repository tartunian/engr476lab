CC=gcc
CFLAGS= -g -lreadline -I.

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

Assignment1: Assignment1.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm Assignment1 *.o