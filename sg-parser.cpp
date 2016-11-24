#include<bits/stdc++.h>
#include "sg-parser.hpp"
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


bool Node::link_groups(){
    // noop
}


bool Instance_node::link_groups(std::unordered_map<std::string, Group*> & groups_namespace){
    if(groups_namespace.find(group_name) != groups_namespace.end()){
	group = groups_namespace[group_name]; // link to group
	return true;
    }else{
	return false;
    }
}


// Takes a stream of tokens and translates
// it in to the basic AST
Group* parse(std::vector<token>){
    
}


// Takes the AST where references are
// only stored by name and adds the
// pointers to where Elements are linked
// to by name.
bool link(Group* ast){
    
}


// takes the AST and adds the links
// within it that turn it in to the
// DAG represented by the code.
bool transform(Group* ast){
    
}
