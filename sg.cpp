#include<bits/stdc++.h>
#define token std::pair<std::string, std::string> // token type, token content


// The sg compiler
// Pascal Sommer, November 2016

// requires GCC >= 4.4

class Scanner{
    std::stringstream buffered;
    std::ifstream infile;
    std::string file_path;
    int fallback_line_number;
    int fallback_character_number;

    bool read_to_buffer(){	
	std::string line = "";
	if( ! getline(infile, line)) return false;

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

    // get until newline, and consume input.
    std::string get_rest_of_line(){
	
    }
    
    Scanner(std::string file_path): file_path(file_path)
    {
	infile.open(file_path, std::ifstream::in);
	line_number = 0;
	line_character_number = 0;
	//buffered = "";
    }
};


class Lexer{
    Scanner* scanner;
    const std::vector<token> v_empty;
    const std::vector<token> v_error = {{"ERROR", "ERROR"}};

    // the lex_<type> functions return the lexed tokens if
    // the type matches, otherwise return an empty vector
    // because c++ is lacking a maybe monad.
    
    std::vector<token> lex_node(){
	if( ! (scanner->match_string("node") || scanner->match_string("n")) ){
	    return v_empty;
	}
	
	std::string name = scanner->get_word();
	
    }

    std::vector<token> lex_edge(){
	if( ! (scanner->match_string("edge") || scanner->match_string("e")) ){
	    return v_empty;
	}
	
    }
    
    std::vector<token> lex_group_header(){
	if( ! (scanner->match_string("group") || scanner->match_string("g")) ){
	    return v_empty;
	}

	std::string name = scanner->get_word();

	if( ! scanner->match_string("{")){
	    std::cerr<< "ERROR in " << scanner->get_position() << ": Expected '{'\n";
	    return v_error;
	}
    }

    std::vector<token> lex_group_close(){
	if( ! scanner->match_string("}")){
	    return v_empty;
	}
	
    }

    std::vector<token> lex_comment(){
	if ( ! scanner->match_string("#")){
	    return v_empty;
	}

	// inline populate vector, requires GCC >= 4.4
	std::vector<token> out = {{"COMMENT", scanner->get_rest_of_line()}};
	return out;
    }
    
public:
    std::string file_path;

    
    
    Lexer(std::string file_path): file_path(file_path){
	scanner = new Scanner(file_path);
    }
};
