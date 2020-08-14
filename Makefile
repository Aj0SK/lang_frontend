BUILD = build
SRC = src
CXX = clang++
CXXFLAGS = -g -O3

all: prepare language

reformat:
	clang-format -i -style=file $(SRC)/*.cpp

prepare: reformat
	mkdir -p $(BUILD)

clean:
	rm -r -f $(BUILD)

language: prepare
	$(CXX) $(CXXFLAGS) -I$(SRC) $(SRC)/example.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o $(BUILD)/example
