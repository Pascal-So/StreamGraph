#include<bits/stdc++.h>
#include "sg-parser.hpp"
#define token std::pair<std::string, std::string> // {token type, token content}


void print_parse_error(std::string message){
    std::cerr<<"PARSE ERROR: " << message << "\n";
}


// Takes a stream of tokens and translates
// it in to the basic AST
Group* parse(std::vector<token> tokens){
    Group* out;
    
    std::stack<Group*> current_stack;
    current_stack.push(out);

    // note that `i` is changed from within the loop in
    // order to always parse the entire node or edge.
    // (These have already been lexed together, therefore
    // we know that the order is correct.)
    for(size_t i = 0; i < tokens.size(); ++i){
	token t = tokens[i];
	
	if(t.first == "group"){
	    Group* new_group;
	    new_group->name = t.second;
	    
	    current_stack.top()->children_groups.push_back(new_group);
	    current_stack.push(new_group);
	}
	else if(t.first == "group_close"){
	    current_stack.pop();
	    if(current_stack.size() < 1){
		print_parse_error("Found '}' but no group is currently open.");
		return 0;
	    }
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
		Bash_node* n = new Bash_node(node_data.second);
		n->name = t.second;
		current_stack.top()->children_bash_nodes.push_back(n);
	    }
	    else if(node_data.first == "instance"){
		Instance_node* n = new Instance_node(node_data.second);
		n->name = t.second;
		current_stack.top()->children_instance_nodes.push_back(n);
	    }
	    else if(node_data.first == "infile" || node_data.first == "outfile"){
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


