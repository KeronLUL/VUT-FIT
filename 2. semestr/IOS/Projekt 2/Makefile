CC=gcc

CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

TARGET = proj2

all: $(TARGET)

$(TARGET): $(TARGET).c
			$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	$(RM) $(TARGET)

zip:
	zip proj2.zip *.c Makefile