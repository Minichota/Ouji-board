PROGRAM_NAME=ouji_board
SRC_DIR=src
O_DIR=obj
CXX=g++
cpp_dirs=errors math terminal
external_files=$(SRC_DIR)/terminal/SDL_FontCache.o
SRC=src

SRC_FILES= $(wildcard $(SRC)/*.cpp $(addsuffix /*.cpp,$(addprefix $(SRC)/,$(cpp_dirs))))
H_FILES=$(SRC_DIR) $(addprefix $(SRC_DIR)/,$(cpp_dirs))
OBJ_FILES=$(patsubst $(SRC)/%.o,$(O_DIR)/%.o, $(SRC_FILES:.cpp=.o))

VPATH=$(SRC_DIR) $(addprefix $(SRC)/,$(cppdirs))

CXX_FLAGS=$(addprefix -I, $(H_FILES)) `sdl2-config --libs --cflags` -O3 -Wall -lSDL2_image -lm -std=c++17 -lSDL2_ttf -lutil

all: $(O_DIR)/$(PROGRAM_NAME) | run

compile: $(O_DIR)/$(PROGRAM_NAME)

$(O_DIR)/$(PROGRAM_NAME): $(OBJ_FILES) | copy_objects
	$(CXX) -o $@ $^ $(CXX_FLAGS) $(external_files)

$(O_DIR)/%.o: %.cpp | $(addprefix $(O_DIR)/,$(cpp_dirs))
	$(CXX) -c -o $@ $< $(CXX_FLAGS)


$(addprefix $(O_DIR)/,$(cpp_dirs)):
	mkdir -p $@

.PHONY: clean run
copy_objects:
	cp -u $(external_files) $(patsubst $(SRC_DIR)/%.o, $(O_DIR)/%.o, $(external_files))

clean:
	rm -rf $(O_DIR)

run: $(O_DIR)/$(PROGRAM_NAME)
	$<
