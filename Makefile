CC=g++
SRC=main.cpp mac.cpp
OUTPUT=build/ping


compile:
		mkdir -p build
		$(CC) $(SRC) -o $(OUTPUT)

start:
		./$(OUTPUT)

all: compile start
