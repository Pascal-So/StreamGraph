CC = g++
CFLAGS = -Wall -Wextra -g -D_GLIBCXX_DEBUG -std=c++0x -O3

all: obj/argparse.o

obj/argparse.o: pull_argparse
	$(MAKE) -C argparse
	cp argparse/argparse.o obj/argparse.o

pull_argparse: argparse
	cd argparse; git pull;

argparse:
	git clone https://github.com/Pascal-So/argparse.git
