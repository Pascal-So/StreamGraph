#include "sg-helpers.hpp"
#include "sg-scanner.hpp"
#include "gtest/gtest.h"
#include<bits/stdc++.h>

TEST(HelpersTest, join){
    std::vector<std::string> words;
    std::string joined;

    words = {"Hello", "there", "world"};
    joined = join(words, ", ");
    EXPECT_EQ(joined, "Hello, there, world");

    words = {"a", "b"};
    joined = join(words, "");
    EXPECT_EQ(joined, "ab");

    words = {};
    joined = join(words, "lkasdf");
    EXPECT_EQ(joined, "");

    words = {""};
    joined = join(words, "fff");
    EXPECT_EQ(joined, "");


    words = {"a"};
    joined = join(words, "eagtetaw");
    EXPECT_EQ(joined, "a");
}

// split string at whitespace
TEST(HelpersTest, str_split){
    std::vector<std::string> result, expected;
    
    result = str_split("a b c");
    expected = {"a", "b", "c"};
    EXPECT_EQ(result, expected);

    result = str_split("a");
    expected = {"a"};
    EXPECT_EQ(result, expected);

    result = str_split("");
    expected = {};
    EXPECT_EQ(result, expected);

    result = str_split("a\ta\n\n\n");
    expected = {"a", "a"};
    EXPECT_EQ(result, expected);

    result = str_split("    a \t     \n");
    expected = {"a"};
    EXPECT_EQ(result, expected);
}


TEST(ScannerTest, get_position){
    std::stringstream ss;
    ss<< " abc\nabc";
    Scanner *s = new Scanner("path", ss);

    EXPECT_EQ(s->get_position(), "path, 1:0");

    s->get_rest_of_line();

    EXPECT_EQ(s->get_position(), "path, 1:4");

    s->skip_whitespace();

    EXPECT_EQ(s->get_position(), "path, 2:0");
}


TEST(ScannerTest, get_number){
    std::stringstream ss;
    ss<< "492abc\nabc";
    Scanner *s = new Scanner("path", ss);

    EXPECT_EQ(s->get_number(), "492");

    EXPECT_EQ(s->get_number(), "");
}

TEST(ScannerTest, get_alphanumscore){
    std::stringstream ss;
    ss<< "492_abc\nlk.a";
    Scanner *s = new Scanner("path", ss);

    EXPECT_EQ(s->get_alphanumscore(), "492_abc");

    s->skip_whitespace();

    EXPECT_EQ(s->get_alphanumscore(), "lk");
}

TEST(ScannerTest, get_rest_of_line){
    std::stringstream ss;
    ss<< "492_a bc\nlk.a";
    Scanner *s = new Scanner("path", ss);

    EXPECT_EQ(s->get_rest_of_line(), "492_a bc");

    EXPECT_EQ(s->get_rest_of_line(), "");

    s->skip_whitespace();
    
    EXPECT_EQ(s->get_rest_of_line(), "lk.a");
}

TEST(ScannerTest, reached_eof){
    std::stringstream ss;
    ss<< " ";
    Scanner *s = new Scanner("path", ss);

    EXPECT_FALSE(s->reached_eof());

    s->skip_whitespace();

    EXPECT_TRUE(s->reached_eof());
}

TEST(ScannerTest, match_string){
    std::stringstream ss;
    ss<< "492_a bc\nlk.a";
    Scanner *s = new Scanner("path", ss);

    EXPECT_TRUE(s->match_string("492_"));

    EXPECT_EQ(s->get_position(), "path, 1:4");

    EXPECT_FALSE(s->match_string("a bb"));

    EXPECT_EQ(s->get_position(), "path, 1:4");

    EXPECT_TRUE(s->match_string("a bc\nl"));

    EXPECT_EQ(s->get_position(), "path, 2:1");
}
