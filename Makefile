CC = gcc
CFLAGS = -Wall -g3 
SRC = ./src
BIN = ./bin
TARGET = note

CSRC := $(wildcard $(SRC)/*.c)
HSRC := $(wildcard $(SRC)/*.h)
OBJS := ${CSRC:c=o}

all: $(TARGET)
	mv $(SRC)/*.o $(TARGET) $(BIN)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm $(BIN)/*

debug: CFLAGS += -DDEBUG
debug: all
