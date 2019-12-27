CC1=mpic++
CC2=mpic++
SOURCE_DIR=src
BIN_DIR=bin
LIB_DIR=lib

OUTPUT1=$(BIN_DIR)/paralela
OUTPUT2=$(BIN_DIR)/sequencial

NETPBM_INC=-I./lib
NETPBM_LIB=-L./lib -lnetpbm


SOURCE1= $(SOURCE_DIR)/mainMPI.cpp
SOURCE2= $(SOURCE_DIR)/main.cpp

CFLAGS= -O3 -lm -g -flto-report -fopenmp

MKDIR= mkdir -p

all: $(OUTPUT1) $(OUTPUT2)


$(OUTPUT1):  $(SOURCE1) 
	    $(MKDIR) $(BIN_DIR)
	    $(CC1) $(SOURCE1) $(CFLAGS) $(NETPBM_INC) $(NETPBM_LIB) -o $(OUTPUT1)

$(OUTPUT2):  $(SOURCE2) 
	    $(CC2) $(SOURCE2) $(CFLAGS) $(NETPBM_INC) $(NETPBM_LIB) -o $(OUTPUT2)



.PHONY: clean
clean:
	rm -f $(OUTPUT1)
	rm -f $(OUTPUT2)
