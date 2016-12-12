CC = g++
CFLAGS = -Wall -Wextra -g -D_GLIBCXX_DEBUG -std=c++0x -O3

all: obj obj/argparse.o obj/bashminify.min.sh

obj:
	if [ ! -d obj ]; then mkdir obj; fi

obj/argparse.o: pull_argparse
	$(MAKE) -C argparse
	cp argparse/argparse.o obj/argparse.o

obj/bashminify.min.sh: pull_bashminify
	$(MAKE) -C bashminify
	cp bashminify/bashminify.min.sh obj/bashminify.min.sh

pull_argparse: argparse
	cd argparse; git pull;

pull_bashminify: bashminify
	cd bashminify; git pull;

argparse:
	if [ ! -d argparse -o ! -d argparse/.git ]; then \
		rm -rf argparse; \
		git clone https://github.com/Pascal-So/argparse.git; fi

bashminify:
	if [ ! -d bashminify -o ! -d bashminify/.git ]; then \
		rm -rf bashminify; \
		git clone https://github.com/Pascal-So/Bash-minify.git bashminify; fi
