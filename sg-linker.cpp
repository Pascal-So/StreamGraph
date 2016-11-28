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
Node_namespace create_node_namespace(Group* g){
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
    for(auto n:instance_nodes){
	if( ! n->link_groups(group_namespace)){
	    // linking failed, because the user
	    // might have linked to a nonexistent
	    // group
	    return false;
	}
    }
    return true;
}


// links edges to the source and destination
// nodes they're pointing to. Returns false if
// a nonexistent node is being pointed to, or
// if a node that only has no input is being
// used as destination or vice versa.
bool link_edges_to_nodes(std::vector<Edge*> & edges, Node_namespace node_namespace){
    for(auto e:edges){
	if( ! e->link_nodes(node_namespace)){
	    // linking failed, user might have linked
	    // to a nonexistent node
	    return false;
	}
    }
    return true;
}


// Takes the AST where references are
// only stored by name and adds the
// pointers to where Elements are linked
// to by name.
// this function is called recursively
bool link(Group* ast, Group_namespace group_namespace){
    update_group_namespace(ast, group_namespace);
    if( ! link_nodes_to_groups(ast->children_instance_nodes, group_namespace)){
	// linking has failed, probably because of a missing
	// group or spelling error
	return false;
    }
    
    Node_namespace node_namespace = create_node_namespace(ast);
    if ( ! link_edges_to_nodes(ast->children_edges, node_namespace)){
	// linking has failed, probably because of a missing
	// node or because a input node was used as output
	// or vice versa
	return false;
    }

    // call recursively for subgroups
    for(auto g : ast->children_groups){
	if( ! link(g, group_namespace)){
	    // something has gone wrong in the
	    // sub group. Fail entire compilation
	    return false;
	}
    }
    
    return true;
}


// takes the AST and adds the links
// in the nodes that turn it in to
// the DAG represented by sg code.
//
// This function calls itself
// recursively
void transform(Group* ast){
    for(auto e:ast->children_edges){
	e->source->out_edges.push_back(e);
    }

    for(auto g:ast->children_groups){
	transform(g);
    }
}

