CXX = g++
CXXFLAGS = -I ../PokerHandEvaluator/cpp/include/ -I ../json/include/ -std=c++11 -O3 -flto -m64 -fopenmp
SRC = main.cc deck.cc game.cc shared.cc infosetnode.cc

# Output executable
TARGET = cfrpoker

# Libraries
LIBS = libpheval.a 

# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LIBS) -o $(TARGET)

# Clean the build
clean:
	rm -f $(TARGET) *.o

.PHONY: all clean