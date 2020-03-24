PROGRAM_NAME=ouji_board
SRC_DIR=src
O_DIR=obj
CXX=g++
cpp_dirs=math instance editor tty terminal buffer setting_editor animation
SRC=src

SRC_FILES= $(wildcard $(SRC)/*.cpp $(addsuffix /*.cpp,$(addprefix $(SRC)/,$(cpp_dirs))))
H_FILES=$(SRC_DIR) $(addprefix $(SRC_DIR)/,$(cpp_dirs))
OBJ_FILES=$(patsubst $(SRC)/%.o,$(O_DIR)/%.o, $(SRC_FILES:.cpp=.o))

VPATH=$(SRC_DIR) $(addprefix $(SRC)/,$(cppdirs))

CXX_FLAGS=$(addprefix -I, $(H_FILES)) `sdl2-config --libs --cflags` -O3 -Wall -lSDL2_image -lm -std=c++17 -lSDL2_ttf -lutil -pthread -ggdb

all: $(PROGRAM_NAME) | run

compile: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(CXX_FLAGS) $(external_files)

$(O_DIR)/%.o: %.cpp | $(addprefix $(O_DIR)/,$(cpp_dirs))
	$(CXX) -c -o $@ $< $(CXX_FLAGS)


$(addprefix $(O_DIR)/,$(cpp_dirs)):
	mkdir -p $@

.PHONY: clean run
clean:
	rm -rf $(O_DIR)

run: $(PROGRAM_NAME)
	./$<
