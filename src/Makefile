CC=gcc
CFLAGS=-Wall -Werror
TARGET=stackvm
OBJS=main.o machine.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY: clean
clean:
	rm -rf $(OBJS) $(TARGET)

