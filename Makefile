# 
CFLAGS = -Wall -Wextra -std=c++17
LLVMFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs --libfiles core`
CC = clang++
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

demo: prepare  ast.o example calculator
	${CC} ${CFLAGS} -o $(BUILD)/example $(BUILD)/example.o $(BUILD)/ast.o $(BUILD)/driver.o $(BUILD)/parser.o $(BUILD)/scanner.o $(LLVMFLAGS)

ast.o:
	${CC} ${CFLAGS} -Wno-unused-result -DDG=1 -I$(SRC) -c -o $(BUILD)/ast.o $(SRC)/ast.cpp $(LLVMFLAGS)

example: grammar
	${CC} ${CFLAGS} -Wno-unused-result -DDG=1 -I$(SRC)/generated -c -o $(BUILD)/example.o $(SRC)/example.cc $(LLVMFLAGS)

calculator: prepare grammar scanner parser driver

parser: grammar
	${CC} ${CFLAGS} -Wno-unused-result -DDG=1 -I$(SRC) -I$(SRC)/generated -c -o $(BUILD)/parser.o $(SRC)/generated/parser.cc $(LLVMFLAGS)

driver: parser
	${CC} ${CFLAGS} $(LLVMFLAGS) -Wno-unused-result -DDG=1 -I$(SRC)/generated -c -o $(BUILD)/driver.o $(SRC)/driver.cc

scanner:
	flex  -o $(SRC)/generated/scanner.cc $(SRC)/scanner.ll
	${CC} ${CFLAGS} $(LLVMFLAGS) -Wno-unused-result -DDG=1 -I$(SRC) -I$(SRC)/generated -c -o $(BUILD)/scanner.o $(SRC)/generated/scanner.cc

grammar:
	bison  --xml --graph=$(SRC)/generated/parser.gv -o $(SRC)/generated/parser.cc $(SRC)/parser.yy
