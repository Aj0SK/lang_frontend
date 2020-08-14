CFLAGS = -Wall -O2 -std=c++17
CC = clang++
BUILD = build
SRC = src

all: prepare demo

clean:
	rm -r -f $(BUILD) $(SRC)/generated

prepare:
	mkdir -p $(BUILD)
	mkdir -p $(SRC)/generated

demo: prepare sample calculator
	${CC} ${CFLAGS} -o $(BUILD)/calc++ $(BUILD)/calc++.o $(BUILD)/driver.o $(BUILD)/parser.o $(BUILD)/scanner.o

sample: grammar
	${CC} ${CFLAGS} -Wno-unused-result -DDG=1 -I$(SRC)/generated -c -o $(BUILD)/calc++.o $(SRC)/calc++.cc

calculator: prepare grammar scanner parser driver

parser: grammar
	${CC} ${CFLAGS} -Wno-unused-result -DDG=1 -I$(SRC) -I$(SRC)/generated -c -o $(BUILD)/parser.o $(SRC)/generated/parser.cc

driver: parser
	${CC} ${CFLAGS} -Wno-unused-result -DDG=1 -I$(SRC)/generated -c -o $(BUILD)/driver.o $(SRC)/driver.cc

scanner:
	flex  -o $(SRC)/generated/scanner.cc $(SRC)/scanner.ll
	${CC} ${CFLAGS} -Wno-unused-result -DDG=1 -I$(SRC) -I$(SRC)/generated -c -o $(BUILD)/scanner.o $(SRC)/generated/scanner.cc

grammar:
	bison  --xml --graph=$(SRC)/generated/parser.gv -o $(SRC)/generated/parser.cc $(SRC)/parser.yy
