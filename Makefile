CFLAGS = -Wall -Wextra -O2 -std=c++17
LLVMFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs core`
CC = g++
BUILD = build
SRC = src

all: prepare demo

reformat:
	clang-format -i -style=file $(SRC)/*.cc $(SRC)/*.h

clean:
	rm -r -f $(BUILD) $(SRC)/generated

prepare:
	mkdir -p $(BUILD)
	mkdir -p $(SRC)/generated

demo: prepare example calculator
	${CC} ${CFLAGS} $(LLVMFLAGS) -o $(BUILD)/example $(BUILD)/example.o $(BUILD)/driver.o $(BUILD)/parser.o $(BUILD)/scanner.o

example: grammar
	${CC} ${CFLAGS} $(LLVMFLAGS) -Wno-unused-result -DDG=1 -I$(SRC)/generated -c -o $(BUILD)/example.o $(SRC)/example.cc

calculator: prepare grammar scanner parser driver

parser: grammar
	${CC} ${CFLAGS} $(LLVMFLAGS) -Wno-unused-result -DDG=1 -I$(SRC) -I$(SRC)/generated -c -o $(BUILD)/parser.o $(SRC)/generated/parser.cc

driver: parser
	${CC} ${CFLAGS} $(LLVMFLAGS) -Wno-unused-result -DDG=1 -I$(SRC)/generated -c -o $(BUILD)/driver.o $(SRC)/driver.cc

scanner:
	flex  -o $(SRC)/generated/scanner.cc $(SRC)/scanner.ll
	${CC} ${CFLAGS} $(LLVMFLAGS) -Wno-unused-result -DDG=1 -I$(SRC) -I$(SRC)/generated -c -o $(BUILD)/scanner.o $(SRC)/generated/scanner.cc

grammar:
	bison  --xml --graph=$(SRC)/generated/parser.gv -o $(SRC)/generated/parser.cc $(SRC)/parser.yy
