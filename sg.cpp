#include<bits/stdc++.h>
#define token std::pair<std::string, std::string> // token type, token content

// The sg compiler
// Pascal Sommer, November 2016

// requires GCC >= 4.4

class Scanner{
    // The Scanner class doesn't do the lexing, but is the interface for the
    // actual lexer to the file. It keeps track of the line and character
    // number, and lets the lexer fetch words or lines, and provides a simple
    // form of backtracking.
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

    void skip_whitespace(){
	
    }

    // todo: figure out what kind of buffer functionality
    // I need exactly
    
public:
    int line_number;
    int line_character_number;
    
    bool match_string(std::string pattern);
    std::string get_alphanum();
    std::string get_rest_of_line();
    std::string get_position();
    
    Scanner(std::string file_path): file_path(file_path)
    {
	infile.open(file_path, std::ifstream::in);
	line_number = 0;
	line_character_number = 0;
	//buffered = "";
    }
};


// if string matches, consume input, otherwise don't
// consume anything. Skips any leading whitespace.
bool Scanner::match_string(std::string pattern){
	
}

// consumes word unless EOF. Returns without leading
// or trailing whitespace.
std::string Scanner::get_alphanum(){
	
}

// get until newline, and consume input.
std::string Scanner::get_rest_of_line(){
	
}

// return the file path, line number and character number
std::string Scanner::get_position(){
    return file_path + ", " +
	std::to_string(line_number) + ":" +
	std::to_string(line_character_number);
}




class Lexer{
    // The Lexer class uses the Scanner class as an interface to the file. It
    // has a single public method (besides the constructor) that returns a
    // vector of tokens.
    Scanner* scanner;
    const std::vector<token> v_empty;
    std::vector<token> v_error;

    void print_error(std::string message){
	std::cerr<<"ERROR in " << scanner->get_position() << ": " << message <<"\n";
    }

    // the lex_<type> functions return the lexed tokens if
    // the type matches, otherwise return an empty vector
    // because c++ is lacking a maybe monad.
    
    std::vector<token> lex_node(){
	if( ! (scanner->match_string("node") || scanner->match_string("n")) ){
	    return v_empty;
	}
	
	std::string name = scanner->get_alphanum();

	std::vector<token> out = {{"node", name}};
	
	if(scanner->match_string(":")){
	    // bash node
	    std::string command = scanner->get_rest_of_line();
	    if(command == ""){
		print_error("Expected bash command");
		return v_error;
	    }
	    
	    out.push_back({"bash_command", command});
	}else if(scanner->match_string("/")){
	    // io node
	    std::string io_type = scanner->get_alphanum();
	    if(io_type != "infile" && io_type != "outfile"){
		print_error("Expected type of io node ('infile' or 'outfile')");
		return v_error;
	    }
	    std::string number = scanner->get_alphanum();
	    if(number == ""){
		print_error("Expected number of file");
		return v_error;
	    }
	    out.push_back({io_type, number});
	}else if(scanner->match_string("-")){
	    // instance node
	    std::string name = scanner->get_alphanum();
	    if(name == ""){
		print_error("Expected group name");
		return v_error;
	    }

	    out.push_back({"instance", name});
	}else{
	    print_error("Expected ':', '/' or '-'");
	    return v_error;
	}
    }

    std::vector<token> lex_modifier(){
	std::vector<token> out;
	if(scanner->match_string(".")){
	    std::string mod = scanner->get_alphanum();
	    if(mod == ""){
		print_error("Expected node modifier");
		return v_error;
	    }
	    out.push_back({"node_modifier", mod});
	}
	return out;
    }

    std::vector<token> lex_mod_name(std::string token_name){
	std::vector<token> out;
	
	std::string name = scanner->get_alphanum();
	if(name == ""){
	    print_error("Expected name of node");
	    return v_error;
	}
	out.push_back({token_name, name});

	// modifier of the source
	std::vector<token> mod = lex_modifier();
	if(mod == v_error){
	    return v_error;
	}else if(! mod.empty()){
	    out.push_back(mod[0]);
	}

	return out;
    }

    std::vector<token> lex_edge(){
	if( ! (scanner->match_string("edge") || scanner->match_string("e")) ){
	    return v_empty;
	}

	std::vector<token> out = {{"edge", ""}};

	// source of the edge
	std::vector<token> name1 = lex_mod_name("edge_source");
	if(name1 == v_error){
	    return v_error;
	}
	for(auto p:name1){
	    out.push_back(p);
	}

	// destination of the edge
	std::vector<token> name2 = lex_mod_name("edge_destination");
	if(name2 == v_error){
	    return v_error;
	}
	for(auto p:name2){
	    out.push_back(p);
	}

	return out;
    }
    
    std::vector<token> lex_group_header(){
	if( ! (scanner->match_string("group") || scanner->match_string("g")) ){
	    return v_empty;
	}

	std::string name = scanner->get_alphanum();

	if( ! scanner->match_string("{")){
	    print_error("Expected '{'");
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
	std::vector<token> out = {{"comment", scanner->get_rest_of_line()}};
	return out;
    }
    
public:
    std::string file_path;

    
    
    Lexer(std::string file_path): file_path(file_path){
	scanner = new Scanner(file_path);
	v_error.push_back({"ERROR", "ERROR"});
    }
};
