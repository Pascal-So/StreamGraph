#include<bits/stdc++.h>
#include "sg-linker.hpp"
#define Node_namespace std::unordered_map<std::string, Node*>
#define Group_namespace std::unordered_map<std::string, Group*>

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
