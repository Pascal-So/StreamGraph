#include<bits/stdc++.h>
#include "sg-lexer.hpp"
#include "sg-parser.hpp"
#include "sg-ast.hpp"
#include "sg-linker.hpp"
#include "sg-graph.hpp"
#include "sg-bash-generator.hpp"
#include "sg-helpers.hpp"
#include "core-script.cpp" // contains the sg core as string literal
#include "argparse.hpp"
#define token std::pair<std::string, std::string> // token type, token content
#define Group_namespace std::unordered_map<std::string, Group*>

// The sg compiler
// Pascal Sommer, 2016

std::string version_number = "0.5";

void info_message(){
    std::cerr<<"StreamGraph compiler " << version_number << "\n";
    std::cerr<<"Pascal Sommer, 2016\n";
}

void help_message(){
    std::cerr<<"Usage: sg infile [-o outfile]\n";
}

std::string get_base_name(std::string name){
    return name.substr(name.find_last_of("/") + 1);
}

std::string get_date(){
    time_t now = time(0);
    // convert `now` to string form
    std::string str_datetime = ctime(&now);
    return str_datetime;
}

void compilation_failed(){
    std::cerr<<"\nCompilation \033[0;31mfailed\033[0m at " << get_date() << "\n";
}

int main(int argc, char* argv[]){

    // parse options with argparse
    std::vector<option> argparse_options;

    // the input filename
    argparse_options.push_back({0, "", "", true}); 

    // the destination of the compiled bash script
    argparse_options.push_back({1, "o", "output-file", true});

    // display the help message
    argparse_options.push_back({99, "h", "help", false});
    
    auto parsed_options = argparse(argparse_options, argc, argv);

    // exit if argparse returned an error
    if(parsed_options.find(-1) != parsed_options.end()){
	std::cerr<<"\n";
        help_message();
	compilation_failed();
        return 1;
    }

    // display help message
    if(parsed_options.find(99) != parsed_options.end()){
	info_message();
	std::cerr<<"\n";
        help_message();
        return 0;
    }

    if (parsed_options.find(0)==parsed_options.end()){
	print_error("No input file given.");
	std::cerr<<"\n";
	help_message();
	compilation_failed();
	return 1;
    }

    std::string program_path= parsed_options[0];
    std::string output_file;
    if (parsed_options.find(1)!=parsed_options.end()){
	// output path was given
	output_file = parsed_options[1];
    }else{
	if(program_path.substr(program_path.length() -3) == ".sg"){
	    output_file = program_path.substr(0, program_path.length() - 3);
	}else{
	    output_file = program_path;
	}
	output_file += ".sh";
    }

    std::string program_name = get_base_name(program_path);

    std::ifstream infile (program_path);
    if( ! infile.is_open()){
	print_error("Couldn't open file " + program_path + ".");
	compilation_failed();
	return 1;
    }
    
    Lexer l (program_path, infile);

    // lex the source code
    auto result = l.lex();

    if(result.back().first=="ERROR"){
	compilation_failed();
	return 1;
    }

    // parse the token stream
    Group* ast = parse(result);
    if (ast == 0){
	compilation_failed();
	return 1;
    }
    
    ast->name = program_name;
    
    // create an empty namespace to start the linker method
    Group_namespace gn;
    // link the DAG pointers in the AST
    bool link_ok = link(ast, gn);
    if ( ! link_ok){
	compilation_failed();
	return 1;
    }

    // create an empty group stack to start the graph checker
    std::stack<Group*> empty_group_stack;
    // check the graph for errors and remove unneccessary parts
    bool graph_ok = groups_check(ast, empty_group_stack);
    if ( ! graph_ok){
	compilation_failed();
	return 1;
    }

    // generate the bash script
    std::string bash_script = generate_bash_script(ast, sg_core, version_number);

    std::ofstream out_file_stream(output_file);
    if( ! out_file_stream.is_open() ){
	std::cerr << "Can't write to output file \"" << output_file << "\"\n";
	help_message();
	return 1;
    }
    out_file_stream<<bash_script<<"\n";
    out_file_stream.close();
    
    return 0;
}
