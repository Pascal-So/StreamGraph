#include<bits/stdc++.h>
#include "sg-lexer.hpp"
#define token std::pair<std::string, std::string> // {token type, token content}

// Lexer class for the sg compiler

// The Lexer class uses the Scanner class as an interface to the file. It
// has a single public method (besides the constructor) that returns a
// vector of tokens.

// Pascal Sommer, November 2016


template <typename T>
void vector_append(std::vector<T> &v1, const std::vector<T> &v2){
    v1.insert(v1.end(), v2.begin(), v2.end());
}



// ****************** Public functions *****************************

Lexer::Lexer(std::string file_path): file_path(file_path){
    scanner = new Scanner(file_path);
    v_error.push_back({"ERROR", "ERROR"});
}

std::vector<token> Lexer::lex(){
    std::vector<token> entire_program;

    scanner->skip_whitespace();
	
    while( ! scanner->reached_eof() ){
	std::vector<token> output;

	output = lex_comment();     // try matching comment
	if(output == v_empty){
	    output = lex_node();    // try matching node
	}
	if(output == v_empty){
	    output = lex_edge();    // try matching edge
	}
	if(output == v_empty){
	    output = lex_group_header(); // try matching group header
	}
	if(output == v_empty){
	    output = lex_group_close();  // try matching group end
	}

	if(output == v_empty){
	    // nothing could be matched, print error.
	    print_error("Expected group, edge, node or comment");
	    output = v_error;
	}

	if(output == v_error){
	    // an error has occurred while lexing.
	    // Stop lexing any further.
	    break;
	}
	
	vector_append(entire_program, output);
	
	scanner->skip_whitespace();
    }

    return entire_program;
}


// *********************  Private functions ************************

void Lexer::print_error(std::string message){
    std::cerr<<"ERROR in " << scanner->get_position() << ": " << message <<"\n";
}

// the lex_<type> functions return the lexed tokens if
// the type matches, otherwise return an empty vector
// because c++ is lacking a maybe monad.
    
std::vector<token> Lexer::lex_node(){
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

    return out;
}

std::vector<token> Lexer::lex_modifier(){
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

std::vector<token> Lexer::lex_mod_name(std::string token_name){
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

std::vector<token> Lexer::lex_edge(){
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
    
std::vector<token> Lexer::lex_group_header(){
    if( ! (scanner->match_string("group") || scanner->match_string("g")) ){
	return v_empty;
    }

    std::string name = scanner->get_alphanum();

    if(name == ""){
	print_error("Expected group name");
	return v_error;
    }
	
    if( ! scanner->match_string("{")){
	print_error("Expected '{'");
	return v_error;
    }

    std::vector<token> out = {{"group", name}};
    return out;
}

std::vector<token> Lexer::lex_group_close(){
    if( ! scanner->match_string("}")){
	return v_empty;
    }

    std::vector<token> out = {{"group_close", "}"}};
    return out;
}

std::vector<token> Lexer::lex_comment(){
    if ( ! scanner->match_string("#")){
	return v_empty;
    }

    // inline populate vector, requires GCC >= 4.4
    std::vector<token> out = {{"comment", scanner->get_rest_of_line()}};
    return out;
}
