#include<bits/stdc++.h>
#define token std::pair<std::string, std::string> // token type, token content

// The sg compiler
// Pascal Sommer, November 2016

// requires GCC >= 4.4




// some utility functions

bool is_whitespace(char c){
    return isspace(c);
}

bool is_not_newline(char c){
    return (c!='\n' && c!='\r');
}

bool is_alphanumeric(char c){
    return isalnum(c);
}

template <typename T>
void vector_append(std::vector<T> &v1, const std::vector<T> &v2){
    v1.insert(v1.end(), v2.begin(), v2.end());
}


class Scanner{
    // The Scanner class doesn't do the lexing, but is the interface for the
    // actual lexer to the file. It keeps track of the line and character
    // number, and lets the lexer fetch words or lines, and provides a simple
    // form of backtracking.
    std::string buffer;
    std::ifstream infile;
    std::string file_path;
    // has filestream reached eof? if this is true, buffer could still be non-
    // empty.
    bool eof;
    
    int line_number;
    int line_character_number;

    bool fill_buffer(int len){	
        while (buffer.size() < len){
	    if(infile.eof()){
		return false;
	    }

	    char next;
	    if( !infile.get(next)){
		eof = true;
		return false;
	    }else{
		buffer+=next;
	    }
	}
	return true;
    }


    // return substring from buffer and then remove it. If len is
    // greater than the buffer length, the entire buffer will be
    // returned quietly. len -1 will return the entire buffer as
    // well.
    // this function updates the line and character number, thus
    // every function that clears something from the buffer must
    // do such through this function.
    std::string clear_from_buffer(int len){
	if(len == -1){
	    len = buffer.size();
	}
	len = std::min(len, (int)buffer.size());	
	std::string out = buffer.substr(0, len);
	for(auto c:out){
	    if(is_not_newline(c)){
		++line_character_number;
	    }else{
		line_character_number = 0;
		++line_number;
	    }
	}
	buffer.erase(buffer.begin(), buffer.begin()+len);
	return out;
    }

    // consumes the input while the next character fulfills (*cond).
    // usually leaves a single character in the buffer, unless eof
    // was reached.
    std::string get_while(bool (*cond)(char)){
	
	for(int i = 0; i < buffer.size(); ++i){
	    if( ! (*cond)(buffer[i])){
		return clear_from_buffer(i);
	    }
	}

	std::string out = clear_from_buffer(-1);

	for(;;){
	    if( ! fill_buffer(1)) break;
	    if( ! (*cond)(buffer[0])) break;
	    out += clear_from_buffer(1);
	}

	return out;
    }


    std::string peek_str(int len){
	if( ! fill_buffer(len)){
	    // return as much as possible if eof is reached
	    len = buffer.size();
	}

	std::string out = buffer.substr(0, len);
	return out;
    }

    std::string get_str(int len){
	std::string out = peek_str(len);
	// this might be shorter than the original len
	len = out.size();
	clear_from_buffer(len);
	return out;
    }

    

public:
    bool reached_eof();
    bool match_string(std::string pattern);
    void skip_whitespace();
    std::string get_alphanum();
    std::string get_rest_of_line();
    std::string get_position();
    
    Scanner(std::string file_path): file_path(file_path)
    {
	infile.open(file_path, std::ifstream::in);
	line_number = 1;
	line_character_number = 0;
	buffer = "";
	eof = false;
    }
};

void Scanner::skip_whitespace(){
    get_while(&is_whitespace);
}

// is eof reached?
bool Scanner::reached_eof(){
    return buffer.empty() && eof;
}

// if string matches, consume input, otherwise don't
// consume anything. Skips any leading whitespace.
bool Scanner::match_string(std::string pattern){
    skip_whitespace();
    std::string in = peek_str(pattern.size());

    if(in.size() < pattern.size()){
	return false;
    }

    if( in == pattern ){
	// if string matches, consume it.
	get_str(pattern.size());
	return true;
    }else{
	return false;
    }
}

// consumes word unless EOF. Returns without leading
// or trailing whitespace.
std::string Scanner::get_alphanum(){
    skip_whitespace();
    return get_while(&is_alphanumeric);
}

// get until newline, and consume input.
std::string Scanner::get_rest_of_line(){
    return get_while(&is_not_newline);
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

	return out;
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

    std::vector<token> lex_group_close(){
	if( ! scanner->match_string("}")){
	    return v_empty;
	}

	std::vector<token> out = {{"group_close", "}"}};
	return out;
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

    std::vector<token> lex(){
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
    
    Lexer(std::string file_path): file_path(file_path){
	scanner = new Scanner(file_path);
	v_error.push_back({"ERROR", "ERROR"});
    }
};


int main(){

    /*
    std::ifstream f ("ipLookup.sg");

    char c;
    while(true){
	std::cout<<f.get(c);
	usleep(10000);
	if (c == EOF){
	    std::cout<<"EOF " << EOF <<"\n";
	}else{
	    std::cout<<c<<std::flush;
	}
    }
    */
    
    
    Lexer l ("ipLookup.sg");

    auto result = l.lex();
    
    
    for(auto t:result){
	std::cout<< t.first << "\t" << t.second<<"\n";
    }
}
