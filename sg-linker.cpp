#include<bits/stdc++.h>
#include "sg-linker.hpp"
#define Node_namespace std::unordered_map<std::string, Node*>
#define Group_namespace std::unordered_map<std::string, Group*>

// takes a group pointer and an existing group namespace
// and updates the namespace to contain the subgroups
// of the referenced group.
void update_group_namespace(Group* ast_node, Group_namespace & group_namespace){
    for(auto sg : ast_node->children_groups){
	group_namespace[sg->name] = sg;
    }
}


// takes a group pointer and returns the map from name
// to node pointer for all the nodes available in this
// group
Node_namespace create_node_namespace(Group* ast_node){
    Node_namespace out;
    for(auto n : ast_node->children_bash_nodes){
	out[n->name] = n;
    }
    for(auto n : ast_node->children_io_nodes){
	out[n->name] = n;
    }
    for(auto n : ast_node->children_instance_nodes){
	out[n->name] = n;
    }
    out["input"] = ast_node->input_node;
    out["output"] = ast_node->output_node;
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


// takes the AST and adds the links
// in the nodes that turn it in to
// the DAG represented by sg code.
void transform(Group* ast_node){
    for(auto e:ast_node->children_edges){
	e->source->out_edges.push_back(e);
	e->destination->in_edges.push_back(e);
    }
}


// Takes the AST where references are
// only stored by name and adds the
// pointers to where Elements are linked
// to by name.
// this function calls itself recursively
bool link(Group* current_group, Group_namespace group_namespace){
    update_group_namespace(current_group, group_namespace);
    if( ! link_nodes_to_groups(current_group->children_instance_nodes, group_namespace)){
	// linking has failed, probably because of a missing
	// group or spelling error
	return false;
    }
    
    Node_namespace node_namespace = create_node_namespace(current_group);
    if ( ! link_edges_to_nodes(current_group->children_edges, node_namespace)){
	// linking has failed, probably because of a missing
	// node or because a input node was used as output
	// or vice versa
	return false;
    }

    current_group->visited = true;

    // call recursively for subgroups
    for(auto g : current_group->children_groups){
	if(g->visited){
	    // group has already been processed,
	    // because the program is recursive.
	    // We don't have to visit this group
	    // again.
	    continue;
	}
	if( ! link(g, group_namespace)){
	    // something has gone wrong in the
	    // sub group. Fail entire compilation
	    return false;
	}
    }

    transform(current_group);
    
    return true;
}



// Set visited flag on nodes that are reachable
// from the end when traversing DAG backwards.
void visit_backwards(Group* ast_root){
    // the nodes queue contains nodes from all
    // levels. This works, because they're
    // pointers, not name references that are
    // specific to some scope.    
    std::queue<Node*> nodes;

    // add overall output node to queue
    nodes.push(ast_root->output_node);
    // add outfile nodes to queue
    for(auto n:ast_root->children_io_nodes){
	if(n->io_type == OUTPUT){
	    nodes.push(n);
	}
    }

    while( ! nodes.empty()){
	Node* n = nodes.front();
	nodes.pop();

	if(n->visited){
	    continue;
	}
	n->visited = true;

	if(n->node_type == INSTANCE_NODE){
	    // visit group
	    Instance_node* instance_node = static_cast<Instance_node*>(n);
	    nodes.push(instance_node->group->output_node);
	}

	for(auto in_edge:n->in_edges){
	    nodes.push(in_edge->source);
	}
    }
}


void reset_group_visited(Group* ast_root){
    ast_root->visited = false;
    for(auto g:ast_root->children_groups){
	reset_group_visited(g);
    }
}


std::pair<std::vector<Group*>, std::vector<Node*> > check_for_unvisited(Group* current_group){
    std::vector<Group*> groups_with_no_visited_nodes;
    std::vector<Node*> unvisited_nodes;

    if(current_group->visited){
	// already been here. Return empty result.
	return {groups_with_no_visited_nodes, unvisited_nodes};
    }


    // have any nodes in the current group been visited so far?
    bool nodes_visited = false;

    // collect all subnodes to make it easier to iterate over them.
    std::vector<Node*> subnodes;
    subnodes.insert(subnodes.end(),
		    current_group->children_bash_nodes.begin(),
		    current_group->children_bash_nodes.end());
    subnodes.insert(subnodes.end(),
		    current_group->children_instance_nodes.begin(),
		    current_group->children_instance_nodes.end());
    subnodes.insert(subnodes.end(),
		    current_group->children_io_nodes.begin(),
		    current_group->children_io_nodes.end());
    
    for(auto n:subnodes){
	if(n->visited){
	    nodes_visited = true; 
	}else{
	    unvisited_nodes.push_back(n);
	}
    }

    return {groups_with_no_visited_nodes, unvisited_nodes};
}



// This function will traverse the DAG to check
// if input to output is connected.
// If fail_on_warn is set, the function will
// treat a warning (unused node) as error.
void traversal(Group* ast_root, bool fail_on_warn){
    // set visited flags on nodes that reach output
    reset_group_visited(ast_root);
    visit_backwards(ast_root);


    // check for unvisited nodes and groups
    // with no visited nodes
    reset_group_visited(ast_root);
    check_for_unvisited(ast_root);

    
}
