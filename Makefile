# Global declerations ----------------
CC := gcc
SRC := ./src
BIN := ./bin
CSRC := $(wildcard $(SRC)/*.c)
OBJS := ${CSRC:c=o}
FLAGS = -Wall

# For debug builds -------------------
DEBUGTARGET = dnote

# For release builds -----------------
BUILD = note

# Make rules -------------------------
	# Release Build ------------------
all: $(BUILD)

$(BUILD): $(OBJS)
	mv $(SRC)/*.o $(BIN)
	mv $(BUILD) /usr/bin/
	./install


	# Debug Build --------------------
Test: Flags += -g3
Test: $(DEBUGTARGET)
	mv $(SRC)/*.o $(DEBUGTARGET) $(BIN)

$(DEBUGTARGET): $(OBJS)
	$(CC) -o $@ $^


	# Global rules -------------------
%.o: %.c
	$(CC) $(FLAGS) -o $@ -c $<

# Clean
clean:
	rm $(BIN)/*

