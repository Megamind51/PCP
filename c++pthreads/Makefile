CC=g++
SOURCE_DIR=./src
BIN_DIR=./bin
LIB_DIR=./lib
OUTPUT=$(BIN_DIR)/executavel.o

SOURCE= $(SOURCE_DIR)/*.cpp

CFLAGS= -lpthread -O0 -lm -g -flto-report -std=c++11

MKDIR= mkdir -p

all: $(OUTPUT)

$(OUTPUT):  $(SOURCE)
	    $(MKDIR) $(BIN_DIR)
	    $(CC) $(INC) $(SOURCE) $(CFLAGS) -o $(OUTPUT)

.PHONY: clean
clean:
	rm -f $(OUTPUT)
