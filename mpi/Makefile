CC=mpic++
SOURCE_DIR=src
BIN_DIR=bin
LIB_DIR=lib
OUTPUT=$(BIN_DIR)/executavel.o

NETPBM_INC=-I./lib
NETPBM_LIB=-L./lib -lnetpbm
#PAPI_INC=-I/share/apps/papi/5.5.0/include
#PAPI_LIB=-L/share/apps/papi/5.5.0/lib

SOURCE= $(SOURCE_DIR)/mainMPI.cpp

CFLAGS= -O3 -lm -g -flto-report

MKDIR= mkdir -p

all: $(OUTPUT)

$(OUTPUT):  $(SOURCE)
	    $(MKDIR) $(BIN_DIR)
	    $(CC) $(INC) $(SOURCE) $(CFLAGS) $(NETPBM_INC) $(NETPBM_LIB) -o $(OUTPUT)
			#$(PAPI_LIB) $(PAPI_INC)

.PHONY: clean
clean:
	rm -f $(OUTPUT)
