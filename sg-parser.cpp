#include<bits/stdc++.h>
#include "sg-parser.hpp"
#define token std::pair<std::string, std::string> // {token type, token content}
#define Node_namespace std::unordered_map<std::string, Node*>
#define Group_namespace std::unordered_map<std::string, Group*>


void print_compile_error(std::string message){
    std::cerr<<"COMPILE ERROR: " << message << "\n";
}

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


// takes a group pointer and an existing group namespace
// and updates the namespace to contain the subgroups
// of the referenced group.
void update_group_namespace(Group* g, Group_namespace & group_namespace){
    for(auto sg : g->children_groups){
	group_namespace[sg->name] = sg;
    }
}


// takes a group pointer and returns the map from name
// to node pointer for all the nodes available in this
// group
Node_namespace create_namespace(Group* g){
    Node_namespace out;
    for(auto n : g->children_bash_nodes){
	out[n->name] = n;
    }
    for(auto n : g->children_io_nodes){
	out[n->name] = n;
    }
    for(auto n : g->children_instance_nodes){
	out[n->name] = n;
    }
    out["input"] = g->input_node;
    out["output"] = g->output_node;
    return out;
}


// links the instance nodes to the groups they're
// pointing to. Returns false if a nonexistent
// group is being pointed to.
bool link_nodes_to_groups(std::vector<Instance_node*> instance_nodes,
			  Group_namespace & group_namespace){
    
    
    return true;
}


// links edges to the source and destination
// nodes they're pointing to. Returns false if
// a nonexistent node is being pointed to, or
// if a node that only has no input is being
// used as destination or vice versa.
bool link_edges_to_nodes(Group* ast, Node_namespace node_namespace){

    return true;
}


// Takes the AST where references are
// only stored by name and adds the
// pointers to where Elements are linked
// to by name.
bool link(Group* ast, Group_namespace group_namespace){
    update_group_namespace(ast, group_namespace);

    link_nodes_to_groups(ast->children_instance_nodes, group_namespace);
    
    Node_namespace node_namespace = create_namespace(ast);

    link_edges_to_nodes(ast, node_namespace);
    
}


// takes the AST and adds the links
// within it that turn it in to the
// DAG represented by the code.
bool transform(Group* ast){
    return true;
}
