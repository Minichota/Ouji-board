PROGRAM_NAME=ouji_board
SRC_DIR=src
CC=g++
CXX_FLAGS=-I$(SRC_DIR) `sdl2-config --libs --cflags` -ggdb3 -O0 -Wall -lSDL2_image -lm
FILES=main
H_FILES=$(FILES).h
SRC_FILES=$(FILES).cpp
OBJ_FILES=main.o
O_DIR=obj

DEPS = $(patsubst %,$(SRC_DIR)/%,$(H_FILES))
SRCS = $(patsubst %,$(SRC_DIR)/%,$(SRC_FILES))
OBJECTS = $(patsubst %,$(O_DIR)/%,$(OBJ_FILES))

all: $(O_DIR)/$(PROGRAM_NAME) | run

compile: $(O_DIR)/$(PROGRAM_NAME)

$(O_DIR):
	mkdir -p $@
$(O_DIR)/%.o: $(SRCS) $(DEPS) | $(O_DIR)
	$(CC) -c -o $@ $< $(CXX_FLAGS)

$(O_DIR)/$(PROGRAM_NAME): $(OBJECTS)
	$(CC) -o $@ $^ $(CXX_FLAGS)


	
.PHONY: clean run
clean:
	rm -rf $(O_DIR)

run: $(O_DIR)/$(PROGRAM_NAME)
	$<
