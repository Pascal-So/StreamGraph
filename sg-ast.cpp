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

bool alphanumeric(std::string s){
    for(auto c:s){
	if(!isalnum(c)) return false;
    }
    return true;
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

    std::regex e ("^([a-zA-Z0-9]+)([-:/])\\s*(.*)");
    std::smatch sm;

    std::string sline = line.str();
    
    if(std::regex_match(sline, sm, e) && sm.size()==4){
	Node* n;

	char node_delimiter = sm.str(2)[0];
	if(node_delimiter == ':'){
	    Bash_node* temp_node = new Bash_node();
	    temp_node->command = sm.str(3);
	    n = temp_node;
	}else if( node_delimiter == '-'){
	    Instance_node* temp_node = new Instance_node();
	    temp_node->group_name = sm.str(3);
	    n = temp_node;
	}else{
	    Io_node* temp_node = new Io_node();
	    temp_node->io_instruction = sm.str(3);
	    n = temp_node;
	}

	n->name = sm.str(1);
	
    }else{
	log_error("Incorrect syntax:\n"
		  "    Expected: node <name>[:-/] <command>\n"
		  "    Found: node " + sline + "\n");
	return NULL;
    }
    
}

Edge* parse_edge(std::istringstream & line){
    std::string start;
    std::string end;

    bool ok = (line >> start) && (line >> end);

    if (! ok){
	log_error("Incorrect edge syntax:\n"
		  "    Expected: e <node_name> <node_name>.\n"
		  "    Found: e " + start + "\n");
	return NULL;
    }

    std::string tmp;
    if(line >> tmp){
	log_error("Incorrect edge syntax:\n"
		  "    Expected: e <node_name> <node_name>.\n    "
		  "    Found: e " + start + " " + end + " " + tmp + "\n");
	return NULL;
    }

    Edge* e = new Edge();
    e->source_name = start;
    e->destination_name = end;

    return e;
}

void start_next_subtree(std::stack<Ast*> & current_scope, Ast* & next_subtree){
    current_scope.push(next_subtree);
    next_subtree = NULL;
}


// this function calls itself recursively whenever
// a new group is created inside the main scope
// or inside another group
//
// due to the simple nature of the
// language, this should be in O(n)
Ast* ast_parse(std::ifstream & infile, std::stack<Ast*> & current_scope){
    Ast* next_subtree = NULL;

    std::string line = "";
    
    while(getline(infile, line)){
	++line_number; // start at line number 1
	
	line = trimWhitespace(line);

	// is line a comment or empty?
	if(line[0]=='#' || line.length() == 0){
	    continue;
	}

	// check for '}'
	if(line[0]=='}'){
	    // finish current subtree
	    if(current_scope.size() == 1){
		// can't close main group
		log_error("Incorrect syntax:\n"
			  "    Found: '}'\n"
			  "    Can't close main group.\n");
	    }else{
		current_scope.pop();
	    }
	}
	
	// also, check for '{' if the next subtree
	// hasn't opened yet.
	if(next_subtree != NULL){
	    if(line[0]=='{'){
		start_next_subtree(current_scope, next_subtree);
		continue;
	    }else{
		// maybe add empty group feature later on, for the sake of
		// quick prototyping, but for now this is an error
		log_error("Incorrect group syntax:\n"
			  "    Expected: start of group block '{'\n"
			  "    Found: " + line + "\n");
		next_subtree = NULL;
		continue;
	    }
	}
	
	
	// convert line to stringstream to make it
	// easier to read word by word
	std::istringstream iss(line);
	std::string type;
	iss >> type;

	type = stringToLower(type);

	Ast_element* next_element = NULL;
	
	if(type=="node" || type == "n"){
	    next_element = parse_node(iss);
	}else if (type == "edge" || type == "e"){
	    next_element = parse_edge(iss);
	}else if (type == "group" || type == "g"){
	    next_subtree = new Ast();
	    if ( ! (iss >> next_subtree->name)){
		log_error("Incorrect group syntax:\n"
			  "    Expected: group <group_name>\n");
	    }
	    if(! alphanumeric(next_subtree->name)){
		log_error("Incorrect group syntax:\n"
			  "    Group name has to be alphanumeric\n"
			  "    Found: " + next_subtree->name + "\n");
	    }
	    std::string rest = trimWhitespace(iss.str());
	    if(rest.length() > 0){
		if(rest[0] == '{'){
		    start_next_subtree(current_scope, next_subtree);
		}else{
		    log_error("Incorrect group syntax:\n"
			      "    Expected: '{' or '\n'"
			      "    Found: " + rest + "\n");
		}
	    }
	}else{
	    log_error("Incorrect syntax:\n"
		      "    Expected: either 'node', 'edge' or 'group'"
		      "    Found: " + type + "\n");
	}

	if(next_element != NULL){
	    current_scope.top()->elements.push_back(next_element);
	}
    }
}


void link_tree(Ast* ast, std::map<string, Ast_element*> variables){
    // get variables in current scope
    for(auto e:ast){
	
    }
    
}


// main parser function
// assumes that ifstream is ok
Ast* parse_file(std::ifstream &infile){

    Ast* root = new Ast();
    std::stack<Ast*> current_scope;

    current_scope.push(root);

    line_number = 0;
    has_error = false;
    
    root = ast_parse(infile, current_scope);

    
    
    if(has_error){
	// some errors have happened.
	std::cerr<<"\n\nCompilation failed\n";
	return NULL;
    }
}
