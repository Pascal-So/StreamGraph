#include<bits/stdc++.h>
#include "sg-parser.hpp"


void print_parse_error(std::string message){
    std::cerr<<"PARSE ERROR: " << message << "\n";
}

std::string strip_bash_comments(std::string command){
    // this variable will be set to 0 while outside a
    // quote, and inside a quote it will hold the
    // character used to quote.
    char quote_char = 0;

    // if the current character is a backslash, we need
    // to ignore the next character.
    bool ignore_next = false;

    std::string out = "";
    for(auto c:command){
	if(ignore_next){
	    ignore_next = false;
	}
	else{
	    if(c == '\\'){
		ignore_next = true;
	    }
	    else if(c == '\'' || c == '"'){
		if(quote_char == 0){
		    quote_char = c;
		}
		else if(quote_char == c){
		    quote_char = 0;
		}
		else{
		    // do nothing, we encountered a single
		    // quote inside double quotes or vice
		    // versa
		}
	    }
	    else if(c == '#'){
		if(quote_char == 0){
		    return out;
		}
	    }
	}
	out += c;
    }

    return out;
}

// Takes a stream of tokens and translates
// it in to the basic AST
// This function is not called recursively,
// instead an internal stack is used to
// keep track of nested groups
Group* parse(std::vector<token> tokens){
    Group* out = new Group();
    
    std::stack<Group*> current_stack;
    current_stack.push(out);

    // note that `i` is changed from within the loop in
    // order to always parse the entire node or edge.
    // (These have already been lexed together, therefore
    // we know that the order is correct.)
    for(size_t i = 0; i < tokens.size(); ++i){
	token t = tokens[i];
	
	if(t.first == "group"){
	    Group* new_group = new Group();
	    new_group->name = t.second;
	    
	    current_stack.top()->children_groups.push_back(new_group);
	    current_stack.push(new_group);
	}
	else if(t.first == "group_close"){
	    if(current_stack.size() == 1){
		print_parse_error("Found '}' but no group is currently open.");
		return 0;
	    }
	    current_stack.pop();
	}
	else if(t.first == "edge"){
	    std::string source_name = tokens[++i].second;
	    std::string mod_source = tokens[++i].second;
	    std::string destination_name = tokens[++i].second;
	    std::string mod_destination = tokens[++i].second;

	    Edge* e = new Edge(source_name, mod_source, destination_name, mod_destination);

	    current_stack.top()->children_edges.push_back(e);
	}
	else if(t.first == "node"){
	    token node_data = tokens[++i];
	    if (node_data.first == "bash_command"){
		std::string clean_bash_command = strip_bash_comments(node_data.second);
		Bash_node* n = new Bash_node(clean_bash_command);
		n->name = t.second;
		current_stack.top()->children_bash_nodes.push_back(n);
	    }
	    else if(node_data.first == "instance"){
		Instance_node* n = new Instance_node(node_data.second);
		n->name = t.second;
		current_stack.top()->children_instance_nodes.push_back(n);
	    }
	    else if(node_data.first == "infile" || node_data.first == "outfile"){
		if(current_stack.size() > 1){
		    print_parse_error("Can't add file io nodes in a group.");
		    return 0;
		}
		int file_number = stoi(node_data.second);
		Io_node* n = new Io_node(node_data.first, file_number);
		n->name = t.second;
		current_stack.top()->children_io_nodes.push_back(n);
	    }
	}
    }

    if(current_stack.size() > 1){
	print_parse_error("Did you forget to close a group?");
	return 0;
    }
    
    return out;
}


