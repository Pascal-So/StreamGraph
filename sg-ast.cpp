#include<bits/stdc++.h>
#include "sg-ast.hpp"
#define token std::pair<std::string, std::string> // {token type, token content}

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


bool Instance_node::link_groups(std::unordered_map<std::string, Group*> & groups_namespace){
    if(groups_namespace.find(group_name) != groups_namespace.end()){
	group = groups_namespace[group_name]; // link to group
	return true;
    }else{
	return false;
    }
}


// The edge constructor
Edge::Edge(std::string source_name,
	   std::string destination_name,
	   std::string mod_source,
	   std::string mod_destination)
    :
    source_name(source_name),
    destination_name(destination_name)
{
    
}
