#include<bits/stdc++.h>
#include "sg-scanner.hpp"

// Lexer class for the sg compiler

// The Lexer class uses the Scanner class as an interface to the file. It
// has a single public method (besides the constructor) that returns a
// vector of tokens.

// Pascal Sommer, November 2016

#ifndef SG_LEXER_HPP
#define SG_LEXER_HPP

template <typename T>
void vector_append(std::vector<T> &v1, const std::vector<T> &v2);


class Lexer{
    
    Scanner* scanner;
    const std::vector<token> v_empty;
    std::vector<token> v_error;

    void print_lexer_error(std::string message);

    
    // the lex_<type> functions return the lexed tokens if
    // the type matches, otherwise return an empty vector
    // because c++ is lacking a maybe monad.
    
    std::vector<token> lex_node();

    std::vector<token> lex_modifier();
    
    std::vector<token> lex_mod_name(std::string token_name);

    std::vector<token> lex_edge();
    
    std::vector<token> lex_group_header();

    std::vector<token> lex_group_close();

    std::vector<token> lex_comment();
    
public:
    std::string file_path;

    std::vector<token> lex();
    
    Lexer(std::string file_path, std::istream & infile); 
};

#endif
