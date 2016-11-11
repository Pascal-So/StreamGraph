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


// this is used for the error messages
int line_number;
bool has_error;

void log(std::string type, std::string message){
    std::cerr<<type << " on line " << line_number << ": " << message <<"\n";
}

void log_error(std::string message){
    log("ERROR", message);
    has_error = true;
}
void log_warning(std::string message){
    log("WARNING", message);
}


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
    std::string name;
    std::string command;

    // maybe use regex here
    // probably
}

Edge* parse_edge(std::istringstream & line){
    std::string start;
    std::string end;

    bool ok = (line >> start) && (line >> end);

    if (! ok){
	log_error("Incorrect edge syntax.\n"
		  "    Expected: e <node_name> <node_name>.\n"
		  "    Found: e " + start);
	return NULL;
    }

    std::string tmp;
    if(line >> tmp){
	log_error("Incorrect edge syntax.\n"
		  "    Expected: e <node_name> <node_name>.\n    "
		  "    Found: e " + start + " " + end + " " + tmp);
	return NULL;
    }

    Edge* e = new Edge();
    e->source_name = start;
    e->destination_name = end;

    return e;
}


// this function calls itself recursively whenever
// a new group is created inside the main scope
// or inside another group
Ast* ast_parse(std::ifstream & infile, std::stack<Ast*> & current_scope){
    Ast* next_subtree = NULL;

    std::string line = "";

    line_number = -1;
    has_error = false;
    
    while(getline(infile, line)){
	++line_number;
	
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

	Ast_element* nextElement;
	
	if(type=="node" || type == "n"){
	    nextElement = parse_node(iss);
	}else if (type == "edge" || type == "e"){
	    nextElement = parse_edge(iss);
	}else if (type == "group" || type == "g"){
	    
	}else{
	    log_error("Incorrect syntax:\n"
		      "    Expected: either 'node', 'edge' or 'group'"
		      "    Found: " + type);
	}

	current_scope.top()->elements.push_back(nextElement);


	
	
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

    if(has_error){
	// some errors have happened.
    }
}
