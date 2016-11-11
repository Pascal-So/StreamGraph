#include<bits/stdc++.h>
#include<boost/filesystem.hpp>
#include "argparse.h"
#include "sg-ast.h"

using namespace std;

/*
 * Compiler for streamgraph
 *
 * Pascal Sommer
 * November 2016
 *
 * Writing a small, not very flexible parser for now, as
 * this should be enough for this language. Maybe use 
 * bison or yacc or boost.spirit later on, or even rewrite
 * everything in haskell and use parsec...
 *
 * Requires [pascal-so/argparse](https://github.com/Pascal-So/argparse)
 *
 */






void info_message(){
    std::cerr<<"sg - compiler for the sg language\n";
    std::cerr<<"Pascal Sommer, November 2016\n\n";
}

// print the help message
void help_message(){
    std::cerr<<"Usage: sg inputfile.sg [-o outputfile.sh]\n";
    std::cerr<<"    -o, --output   The .sh file where the resulting\n";
    std::cerr<<"                   script should be saved. Defaults\n";
    std::cerr<<"                   to <input file name>.sh.\n";
    std::cerr<<"    -h, --help     Print this help message.\n";
    std::cerr<<"\n";
}

template <typename T, typename U>
bool key_exists(std::unordered_map<T, U> m, T key){
    return m.find(key) != m.end();
}

int main(int argc, char * argv[]){
    
    vector<option> options;
    options.push_back({0, "", "", false}); // input file
    options.push_back({1, "o", "output", true}); //output file
    options.push_back({9, "h", "help", false}); //help message
    

    auto parsed_options = argparse(options, argc, argv);

    // exit if argparse returned an error
    if(key_exists(parsed_options, -1)){
	help_message();
	return 1;
    }

    string input_file = "";
    string output_file = "";
    
    // detect options that were actually passed

    // input file
    if(key_exists(parsed_options, 0)){
	input_file = parsed_options[0];
    }else{ //no input file was given
	std::cerr<<"ERROR: No input file given\n";
	help_message();
	return 1;
    }

    // output file
    if(key_exists(parsed_options, 1)){
	output_file = parsed_options[0];
    }

    // display help message
    if(key_exists(parsed_options, 9)){
	info_message();
	help_message();
	return 0;
    }


    
    
    // setup defaults

    if(output_file == ""){
	// get base name of the input file (without extension)
	string base_name = boost::filesystem::path(input_file).stem().string();

	output_file = base_name + ".sh";
    }



    // open input file
    ifstream infile (input_file);
    if( ! infile.is_open()){
	std::cerr<<"ERROR: Can't open input file";
	help_message();
	return 1;
    }

    // open output file
    ofstream outfile (output_file);
    if( ! outfile.is_open()){
	std::cerr<<"ERROR: Can't open output file";
	help_message();
	return 1;
    }

    // parse input file
    Ast* tree = parse_file(infile);


    
}
