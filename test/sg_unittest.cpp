#include "sg-helpers.hpp"
#include "gtest/gtest.h"

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
