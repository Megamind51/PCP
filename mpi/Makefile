CC=mpic++
SOURCE_DIR=src
BIN_DIR=bin
LIB_DIR=lib
OUTPUT=$(BIN_DIR)/executavel.o


SOURCE= $(SOURCE_DIR)/main.cpp

CFLAGS= -O3 -lm -g -flto-report

MKDIR= mkdir -p

all: $(OUTPUT)

$(OUTPUT):  $(SOURCE)
	    $(MKDIR) $(BIN_DIR)
	    $(CC) $(INC) $(SOURCE) $(CFLAGS) -o $(OUTPUT)

.PHONY: clean
clean:
	rm -f $(OUTPUT)
