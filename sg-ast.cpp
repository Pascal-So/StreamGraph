#include<bits/stdc++.h>
#include "sg-ast.hpp"
#define token std::pair<std::string, std::string> // {token type, token content}


void print_linker_error(std::string message){
    std::cerr<<"LINKER ERROR: " << message << "\n";
}


bool Node::has_input(){
    return true;
}


bool Node::has_output(){
    return true;
}


bool Node::has_inverse(){
    return false;
}


bool Node::link_groups(std::unordered_map<std::string, Group*> & ){
    return true; // noop
}


Bash_node::Bash_node(std::string bash_command):bash_command(bash_command){

}


Io_node::Io_node(std::string io_type_str, int number):number(number){
    io_type = (io_type_str == "infile") ? INPUT : OUTPUT;
}


bool Io_node::has_input(){
    return io_type == INPUT;
}


bool Io_node::has_output(){
    return io_type == OUTPUT;
}


Stdio_node::Stdio_node(Io_type io_type):io_type(io_type){
    
}

bool Stdio_node::has_input(){
    return io_type == INPUT;
}


bool Stdio_node::has_output(){
    return io_type == OUTPUT;
}


Instance_node::Instance_node(std::string group_name):group_name(group_name){
    
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
    mod_destination = (mod_destination_str[0] == 'h') ? HORIZONTAL : VERTICAL;
    mod_nr_destination = stoi(mod_destination_str.substr(1));
}


bool Edge::link_nodes(std::unordered_map<std::string, Node*> & nodes_namespace){

    // link source node
    if(nodes_namespace.find(source_name) == nodes_namespace.end()){
	print_linker_error("Source node \"" + source_name + "\" could not be found.");
	return false;
    }

    if( ! nodes_namespace[source_name]->has_output()){
	print_linker_error("Node \"" + source_name + "\" cannot be used as source.");
	return false;
    }

    source = nodes_namespace[source_name];
    
    // link destination node
    if(nodes_namespace.find(destination_name) == nodes_namespace.end()){
	print_linker_error("Destination node \"" + destination_name + "\" could not be found.");
	return false;
    }

    if( ! nodes_namespace[destination_name]->has_input()){
	print_linker_error("Node \"" + destination_name + "\" cannot be used as destination.");
	return false;
    }

    destination = nodes_namespace[destination_name];
    
    return true;
}
