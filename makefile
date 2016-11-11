BOOST_DIR=/usr/local/boost_1_62_0

ARGPARSE_DIR=~/code/argparse

CFLAGS=-Wall -Wextra -D_GLIBCXX_DEBUG -std=c++0x -g

INCLUDE=-I $(ARGPARSE_DIR) -I $(BOOST_DIR)

BOOSTFLAGS=-DBOOST_SYSTEM_NO_DEPRECATED

SOURCES=sg.cpp sg-ast.cpp $(ARGPARSE_DIR)/argparse.cpp $(BOOST_DIR)/stage/lib/libboost_filesystem.a


sg: $(SOURCES)
	g++ $(CFLAGS) $(INCLUDE) $(BOOSTFLAGS) $(SOURCES) -o sg

