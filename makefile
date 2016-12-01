CC = g++
CFLAGS = -Wall -Wextra -g -D_GLIBCXX_DEBUG -std=c++0x
INCLUDED_FILES = sg-lexer sg-scanner sg-parser sg-ast sg-linker sg-graph sg-bash-generator
SOURCES = $(INCLUDED_FILES:=.cpp) sg.cpp
HEADERS = $(INCLUDED_FILES:=.hpp)
OBJECTS = $(INCLUDED_FILES:=.o) sg.o


all: $(SOURCES) $(HEADERS) sg

sg-lexer.o: sg-lexer.hpp sg-lexer.cpp
	$(CC) $(CFLAGS) -c sg-lexer.cpp

sg-graph.o: sg-graph.hpp sg-graph.cpp
	$(CC) $(CFLAGS) -c sg-graph.cpp

sg-linker.o: sg-linker.hpp sg-linker.cpp
	$(CC) $(CFLAGS) -c sg-linker.cpp

sg-scanner.o: sg-scanner.hpp sg-scanner.cpp
	$(CC) $(CFLAGS) -c sg-scanner.cpp

sg-parser.o: sg-parser.hpp sg-parser.cpp
	$(CC) $(CFLAGS) -c sg-parser.cpp

sg-ast.o: sg-ast.hpp sg-ast.cpp
	$(CC) $(CFLAGS) -c sg-ast.cpp

sg-bash-generator.o: sg-bash-generator.hpp sg-bash-generator.cpp
	$(CC) $(CFLAGS) -c sg-bash-generator.cpp

sg.o: sg.cpp
	$(CC) $(CFLAGS) -c sg.cpp

# this creates a cpp file with the sg core as string literal
core-script.cpp: core.sh
	./load-core-to-cpp.sh core.sh > core-script.cpp

sg: $(OBJECTS) core-script.cpp
	$(CC) $(CLFAGS) -o sg $(OBJECTS)
