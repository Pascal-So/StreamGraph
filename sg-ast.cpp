#include<bits/stdc++.h>
#include "sg-ast.h"
/*
 * StreamGraph language
 * 
 * The code related to the abstract syntax tree such
 * as parsing.
 * 
 * Pascal Sommer, November 2016
 *
 */


std::string trimWhitespace(std::string line){
    // strip leading whitespace
    auto strBegin = line.find_first_not_of(" \t");
    if (strBegin != std::string::npos){
	// trim trailing whitespace as well
	auto strEnd = line.find_last_not_of(" \t");
	auto strRange = strEnd - strBegin + 1;

	return line.substr(strBegin, strRange);
    }else{
	// line is empty
	return "";
    }
}

std::string stringToLower(std::string & in){
    std::string out = in;
    for(auto & c: out){
	c = tolower(c);
    }
    return out;
}

Node* parse_node(std::istringstream & line){
    
}

Edge* parse_edge(std::istringstream & line){

}


// this function calls itself recursively whenever
// a new group is created inside the main scope
// or inside another group
Ast* ast_parse(std::ifstream & infile, std::stack<Ast*> & current_scope){
    Ast* next_subtree = NULL;

    std::string line = "";

    while(getline(infile, line)){
	
	line = trimWhitespace(line);

	// is line a comment?
	if(line[0]=='#'){
	    continue;
	}

	// check for '}'
	
	
	// also, check for '{' if the next subtree
	// hasn't opened yet.

	// convert line to stringstream to make it
	// easier to read word by word
	std::istringstream iss(line);
	std::string type;
	iss >> type;

	type = stringToLower(type);
	
	if(type=="node" || type == "n"){
	    
	}else if (type == "edge" || type == "e"){

	}else if (type == "group" || type == "g"){

	}else{
	    //error
	}
	
    }
}


// main parser function
// assumes that ifstream is ok
//
// due to the simple nature of the
// language, this should be in O(n)
Ast* parse_file(std::ifstream &infile){

    Ast* root = new Ast();
    std::stack<Ast*> current_scope;

    current_scope.push(root);
    
    root = ast_parse(infile, current_scope);
}
