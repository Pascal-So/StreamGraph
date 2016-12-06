CC = g++
CFLAGS = -Wall -Wextra -g -D_GLIBCXX_DEBUG -std=c++0x -O3
INCLUDED_FILES = sg-lexer sg-scanner sg-parser sg-ast sg-linker sg-graph sg-bash-generator argparse
OBJECTS = $(INCLUDED_FILES:=.o) sg.o

all: sg

argparse.o: argparse.cpp argparse.hpp
	$(CC) $(CFLAGS) -c argparse.cpp

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

sg.o: sg.cpp core-script.cpp
	$(CC) $(CFLAGS) -c sg.cpp

core.min.sh: core.sh minify-script.sh
	./minify-script.sh core.sh > core.min.sh

# this creates a cpp file with the sg core as string literal
core-script.cpp: core.min.sh load-core-to-cpp.sh
	./load-core-to-cpp.sh core.min.sh > core-script.cpp

sg: $(OBJECTS)
	$(CC) $(CLFAGS) -o sg $(OBJECTS)


clear:
	rm -f sg
	rm -f *.o
	rm -f core.min.sh
	rm -f core-script.cpp
