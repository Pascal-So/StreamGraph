#include<bits/stdc++.h>

class Scanner{
    std::stringstream buffered;
    std::ifstream infile;
    std::string file_path;
    int fallback_line_number;
    int fallback_character_number;

    bool read_to_buffer(){	
	std::string line = "";
	if( ! getline(infile, line)) break;

	buffered << line;

	if(infile.bad()){
	    std::cerr<<"IO Error\n";
	    return false;
        }

	return ! buffered.str().empty(); 
    }

    // todo: figure out what kind of buffer functionality
    // I need exactly
    
public:
    int line_number;
    int line_character_number;

    // if string matches, consume input, otherwise don't
    // consume anything.
    bool match_string(std::string pattern){
	
    }

    // consumes word unless EOF. Returns without leading
    // or trailing whitespace. Word character is everything
    // except for whitespace.
    std::string get_word(){
	
    }

    
    
    Scanner(std::string file_path): file_path(file_path)
    {
	infile.open(file_path, std::ifstream::in);
	line_number = 0;
	line_character_number = 0;
	buffered = "";
    }
};


class Lexer{
    Scanner* scanner;
public:
    std::string file_path;

    
    
    Lexer(std::string file_path): file_path(file_path){
	scanner = new Scanner(file_path);
    }
};
