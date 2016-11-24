#include<bits/stdc++.h>
#define token std::pair<std::string, std::string> // {token type, token content}


// Scanner class for the sg compiler

// The Scanner class doesn't do the lexing, but is the interface for the
// actual lexer to the file. It keeps track of the line and character
// number, and lets the lexer fetch words or lines, and provides a simple
// form of backtracking.

// Pascal Sommer, November 2016


#ifndef SG_SCANNER_HPP
#define SG_SCANNER_HPP


class Scanner{
    std::string buffer;
    std::ifstream infile;
    std::string file_path;
    // has filestream reached eof? if this is true, buffer could still be non-
    // empty.
    bool eof;
    
    int line_number;
    int line_character_number;

    bool fill_buffer(int len);

    std::string clear_from_buffer(int len);

    std::string get_while(bool (*cond)(char));

    std::string peek_str(int len);

    std::string get_str(int len);

public:
    bool reached_eof();
    bool match_string(std::string pattern);
    void skip_whitespace();
    std::string get_alphanum();
    std::string get_number();
    std::string get_rest_of_line();
    std::string get_position();
    
    Scanner(std::string file_path);
};


#endif
