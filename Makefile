# Global declerations ----------------
CC := gcc
SRC := ./src
BIN := ./bin
CSRC := $(wildcard $(SRC)/*.c)
OBJS := ${CSRC:%.c=%.o}
FLAGS = -Wall

# For debug builds -------------------
DEBUGTARGET = dnote

# For release builds -----------------
BUILD = note

# Make rules -------------------------
# --- Release Build ------------------
$(BUILD): $(OBJS)
	$(CC) $^ -o $@
	mv $(SRC)/*.o $(BIN)
	sudo mv ./$(BUILD) /usr/local/bin/

# --- Debug Build --------------------
Test: FLAGS := $(FLAGS) -g3 -DDEBUG
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
