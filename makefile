CXX = g++
CXXFLAGS = -O2 -std=c++17
TARGET = bruteforce
SRC = bruteforce.cpp

all: $(TARGET) count

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	
count: count.cpp
	$(CXX) $(CXXFLAGS) -o count count.cpp