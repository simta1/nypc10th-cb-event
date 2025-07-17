CXX = g++
CXXFLAGS = -O2 -std=c++17
TARGET = bruteforce
SRC = bruteforce.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
