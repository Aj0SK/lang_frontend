BUILD = build
SRC = src
CXX = clang++
CXXFLAGS = -g -O3

all: prepare language

prepare:
	mkdir -p $(BUILD)

clean:
	rm -r -f $(BUILD)

language: prepare
	$(CXX) $(CXXFLAGS) $(SRC)/example.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o $(BUILD)/example
