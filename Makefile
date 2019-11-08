CC=gcc
SOURCE_DIR=./src
BIN_DIR=./bin
LIB_DIR=./lib
OUTPUT=$(BIN_DIR)/executavel.o

NETPBM_INC=-I./lib
NETPBM_LIB=-L./lib -lnetpbm

SOURCE= $(SOURCE_DIR)/*.c

CFLAGS= -fopenmp -O3 -std=c99  -lm -lpapi

MKDIR= mkdir -p

all: $(OUTPUT)

$(OUTPUT):  $(SOURCE)
	    $(MKDIR) $(BIN_DIR)
	    $(CC) $(INC) $(SOURCE) $(CFLAGS) $(NETPBM_INC) $(NETPBM_LIB) -o $(OUTPUT)

.PHONY: clean
clean:
	rm -f $(OUTPUT)
