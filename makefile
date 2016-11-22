CC = g++
CFLAGS = -Wall -Wextra -g -D_GLIBCXX_DEBUG -std=c++0x
OBJECTS = sg-lexer.o sg-scanner.o sg-parser.o sg.o
SOURCES = sg-lexer.cpp sg-lexer.hpp sg-scanner.cpp sg-scanner.hpp sg-parser.cpp sg-parser.hpp sg.cpp

all: $(SOURCES) sg

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
