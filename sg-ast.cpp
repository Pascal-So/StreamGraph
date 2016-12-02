#include<bits/stdc++.h>
#include "sg-ast.hpp"
#define token std::pair<std::string, std::string> // {token type, token content}


void print_linker_error(std::string message){
    std::cerr<<"LINKER ERROR: " << message << "\n";
}

Node::~Node(){}

bool Node::is_input(){
    return false;
}


bool Node::is_output(){
    return false;
}


bool Node::has_inverse(){
    return false;
}


bool Node::link_groups(std::unordered_map<std::string, Group*> & ){
    return true; // noop
}


Bash_node::Bash_node(std::string bash_command):bash_command(bash_command){
    visited = false;
    needed = false;
    cycle_dfs_active = false;
    node_type = BASH_NODE;
}


Io_node::Io_node(std::string io_type_str, int number):number(number){
    io_type = (io_type_str == "infile") ? INPUT : OUTPUT;
    visited = false;
    needed = false;
    cycle_dfs_active = false;
    node_type = IO_NODE;
}


bool Io_node::is_input(){
    return io_type == INPUT;
}


bool Io_node::is_output(){
    return io_type == OUTPUT;
}


Stdio_node::Stdio_node(Io_type io_type):io_type(io_type){
    visited = false;
    needed = false;
    cycle_dfs_active = false;
    node_type = STDIO_NODE;
    name = (io_type == INPUT) ? "input" : "output";
}

bool Stdio_node::is_input(){
    return io_type == INPUT;
}


bool Stdio_node::is_output(){
    return io_type == OUTPUT;
}


Instance_node::Instance_node(std::string group_name):group_name(group_name){
    visited = false;
    needed = false;
    cycle_dfs_active = false;
    node_type = INSTANCE_NODE;
}


bool Instance_node::link_groups(std::unordered_map<std::string, Group*> & groups_namespace){
    if(groups_namespace.find(group_name) != groups_namespace.end()){
	group = groups_namespace[group_name]; // link to group
	return true;
    }else{
	print_linker_error("Group \"" + group_name + "\" could not be found.");
	return false;
    }
}


// The edge constructor
Edge::Edge(std::string source_name,
	   std::string destination_name,
	   std::string mod_source_str,
	   std::string mod_destination_str)
    :
    source_name(source_name),
    destination_name(destination_name)
{
    mod_source = (mod_source_str == "inv") ? INVERSE : NONE;
    if(mod_destination_str == ""){
	mod_destination = NONE;	
    }else{
	mod_destination = (mod_destination_str[0] == 'h') ? HORIZONTAL : VERTICAL;
    }
    
    mod_nr_destination = stoi(mod_destination_str.substr(1));

    needed = true;
}


bool Edge::link_nodes(std::unordered_map<std::string, Node*> & nodes_namespace){

    // link source node
    if(nodes_namespace.find(source_name) == nodes_namespace.end()){
	print_linker_error("Source node \"" + source_name + "\" could not be found.");
	return false;
    }

    if(nodes_namespace[source_name]->is_output()){
	print_linker_error("Node \"" + source_name + "\" cannot be used as source.");
	return false;
    }

    source = nodes_namespace[source_name];
    
    // link destination node
    if(nodes_namespace.find(destination_name) == nodes_namespace.end()){
	print_linker_error("Destination node \"" + destination_name + "\" could not be found.");
	return false;
    }

    if(nodes_namespace[destination_name]->is_input()){
	print_linker_error("Node \"" + destination_name + "\" cannot be used as destination.");
	return false;
    }

    destination = nodes_namespace[destination_name];
    
    return true;
}

Group::Group(){
    visited = false;
}

std::vector<Node*> Group::list_inputs(){
    std::vector<Node*> out;
    out.push_back(input_node);
    for(auto n:children_io_nodes){
	if(n->is_input()){
	    out.push_back(n);
	}
    }
    return out;
}

std::vector<Node*> Group::list_all_nodes(){
    std::vector<Node*> out;
    out.push_back(input_node);
    out.push_back(output_node);
    for(auto n:children_io_nodes){
	out.push_back(n);
    }
    for(auto n:children_bash_nodes){
	out.push_back(n);
    }
    for(auto n:children_instance_nodes){
	out.push_back(n);
    }
    return out;
}
