CC = g++
CFLAGS = -Wall -Wextra -g -D_GLIBCXX_DEBUG -std=c++0x
INCLUDED_FILES = sg-lexer sg-scanner sg-parser
SOURCES = $(INCLUDED_FILES:=.cpp) sg.cpp
HEADERS = $(INCLUDED_FILES:=.hpp)
OBJECTS = $(INCLUDED_FILES:=.o) sg.o


all: $(SOURCES) $(HEADERS) sg

sg-lexer.o: sg-lexer.hpp sg-lexer.cpp
	$(CC) $(CFLAGS) -c sg-lexer.cpp

sg-scanner.o: sg-scanner.hpp sg-scanner.cpp
	$(CC) $(CFLAGS) -c sg-scanner.cpp

sg-parser.o: sg-parser.hpp sg-parser.cpp
	$(CC) $(CFLAGS) -c sg-parser.cpp

sg.o: sg.cpp
	$(CC) $(CFLAGS) -c sg.cpp

sg: $(OBJECTS)
	$(CC) $(CLFAGS) -o sg $(OBJECTS)
