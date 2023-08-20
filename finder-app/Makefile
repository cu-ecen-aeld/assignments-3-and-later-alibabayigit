CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -Werror -g
	
all: writer

writer: writer.o
	$(CC) $(CFLAGS) writer.c -o writer
		
clean:
	rm -f writer *.o
