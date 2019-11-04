CC=gcc
SOURCE_DIR=./src
BIN_DIR=./bin
OUTPUT=$(BIN_DIR)/executavel.o

NETPBM_INC=-I/usr/include
NETPBM_LIB=-L/usr/lib -lnetpbm

SOURCE= $(SOURCE_DIR)/*.c

CFLAGS=-std=c11

MKDIR= mkdir -p

all: $(OUTPUT)

$(OUTPUT):  $(SOURCE)
	    $(MKDIR) $(BIN_DIR)
	    $(CC) $(INC) $(SOURCE) $(CFLAGS) $(NETPBM_INC) $(NETPBM_LIB) -o $(OUTPUT)

.PHONY: clean
clean:
	rm -f $(OUTPUT)
