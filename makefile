CXX = g++
CXXFLAGS = -O2 -std=c++17
BIN_DIR = bin
SRC_DIR = src

SRC = $(wildcard src/*.cpp)
TARGETS = $(patsubst src/%.cpp, bin/%, $(SRC))

.PHONY: all clean run count board

all: $(TARGETS)

$(BIN_DIR)/%: $(SRC_DIR)/%.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

run: $(BIN_DIR)/bruteforce
	./$(BIN_DIR)/bruteforce

count: $(BIN_DIR)/count
	./$(BIN_DIR)/count

board:
	python bin/image_to_board.py

clean:
	rm -f $(TARGETS)