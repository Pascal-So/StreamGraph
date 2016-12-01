#include<bits/stdc++.h>
#include "sg-graph.hpp"

// SG graph checks and optimizations

// These functions perform some checks on the graph to
// eliminate dead ends and unneeded functions. This will
// lead to smaller compiled files in the end. Maybe this
// will be deferred to optimization flags later on.

// The responsibilities of these functions:
// - Check if a path from input to output exists
// - Remove out_edges on nodes leading to dead ends
// - Check for cycles
// - Remove unneeded groups
//
// The checks are performed in this order. They happen
// recursively on every level, starting from the hightest
// level. This way, nothing gets executed for the groups
// that aren't needed and the user isn't bothered by
// unnecessary warnings for groups that exist, but are
// not connected.



// Different typenames are possible if a vector of derived
// class pointers is appended to a vector of base class
// pointers. This leads to ugly gcc error messages though
// if the function is used incorrectly, so I might have
// to change this.
template<typename T, typename U>
void vector_append(std::vector<T> &a, std::vector<U> &b){
    a.insert(a.end(), b.begin(), b.end());
}

// might be deleted, as it's not needed right now, but
// maybe in the fututre
// template<typename T, typename U>
// std::vector<T> vector_cast(std::vector<U> in){
//     std::vector<T> out;
//     for(auto e:in){
// 	out.push_back(static_cast<T>(e));
//     }
//     return out;
// }


void reset_visited_nodes(Group* ast_node){
    for(auto n:ast_node->children_bash_nodes){
	n->visited = false;
    }
    for(auto n:ast_node->children_io_nodes){
	n->visited = false;
    }
    for(auto n:ast_node->children_instance_nodes){
	n->visited = false;
    }
    ast_node->input_node->visited = false;
    ast_node->output_node->visited = false;
}



// This function doesn't specifically check for cycles,
// but it is robust to cycles and will not result in
// undefined behaviour.
// The function doesn't go down in to group instances,
// the groups will be checked later and the function
// therefore considers instance and bash nodes exactly
// the same way.
bool dfs_reaches_output(Node* n){
    if(n->visited){
	return n->needed;
    }
    n->visited = true;

    // this is needed in case of cycles. If the
    // dfs reaches this node before the function
    // has finished for this node again , we want
    // to return false for output reaching, because
    // if there is a path to the output it will
    // still be found, but we don't want the cycle
    // to incorrectly return true.
    n->needed = false;
    
    if (n->is_output()){
	// reached an output
	n->needed = true;
	return true;
    }

    bool reaches_output = false;
    for(auto out_edge:n->out_edges){
	reaches_output |= dfs_reaches_output(out_edge->destination);
    }

    // flag node as needed because it lies on a path from in to out.
    n->needed = reaches_output;
    
    return reaches_output;
}

void dfs_remove_dead_out_edges(Node* n){
    if(! n->needed){
	// don't need to do anything on unneeded nodes,
	// they'll get removed anyway.
	return;
    }

    // copy only the out edges that point to nodes that
    // are needed.
    std::vector<Edge*> new_out_edges;
    for(auto e:n->out_edges){
	if(e->destination->needed){
	    new_out_edges.push_back(e);
	}
    }
    n->out_edges = new_out_edges;
}


// T should be a derived node like Bash_node
template<typename T>
void cleanup_nodes_vector(std::vector<T*> & nodes){
    int i = 0;
    for(auto & n:nodes){
	if(! n->needed){
	    // base class destructor has to be virtual,
	    // otherwise this results in undefined
	    // behaviour
	    delete n;
	}else{
	    nodes[i] = n;
	    ++i;
	}
    }
    nodes.resize(i);
}



// This function has to be called after dfs_remove_dead_out_edges,
// because otherwise the pointers in the out_edges vector might
// refer to deleted objects.
void remove_unneeded_nodes(Group* ast_node){
    cleanup_nodes_vector(ast_node->children_bash_nodes);
    cleanup_nodes_vector(ast_node->children_instance_nodes);
    cleanup_nodes_vector(ast_node->children_io_nodes);
    if( ! ast_node->input_node->needed ){
	delete ast_node->input_node;
	ast_node->input_node = 0;
    }
    if( ! ast_node->output_node->needed ){
	delete ast_node->output_node;
	ast_node->output_node = 0;
    }
}


bool check_group_connected(Group* ast_node){
    reset_visited_nodes(ast_node);

    bool connected = false;
    // check file input nodes
    for(auto n:ast_node->children_io_nodes){
	if(n->is_input()){
	    connected |= dfs_reaches_output(n);
	}
    }
    // check stdin node
    connected |= dfs_reaches_output(ast_node->input_node);

    return connected;
}


// currently not needed.
// void reset_group_visited(Group* current_group){
//     current_group->visited = false;
//     for(auto g:current_group->children_groups){
// 	reset_group_visited(g);
//     }
// }


// a group can only be instanciated from a instance
// node on the same level or one further down. The
// instanciation further down only matters if the
// group gets instanciated at least one from the
// outside. Therefore, to see if a group is needed,
// we only need to check on the same level.
// This function checks for a given ast_node all the
// subgroups.
void determine_groups_needed(Group* ast_node){
    for(auto g:ast_node->children_groups){
	// reset needed flags on groups.
	g->needed = false;
    }
    for(auto n:ast_node->children_instance_nodes){
        // we don't need to check for the `needed`
	// flag on the node because the function
	// `remove_unneeded_nodes` has taken care
	// of this.

	// set the needed flag on the group. If
	// the node references a group in a level
	// further up, this doesn't matter because
	// they already have the needed flag set
	// anyway.
	n->group->needed = true;
    }
}


void remove_unneeded_groups(Group* ast_node){
    int i = 0;
    std::vector<Group*> needed_groups;
    for(auto & g:ast_node->children_groups){
	if( ! g->needed){
	    delete g;
	}else{
	    needed_groups.push_back(g);
	}
    }

    // only keep the pointers to the needed groups,
    // because the other groups have been deleted.
    ast_node->children_groups = needed_groups;
}










std::string format_group_stack(std::stack<Group*> stack){
    std::vector<std::string> names;
    while(! stack.empty()){
	Group* g = stack.top();
	stack.pop();
	names.push_back(g->name);
    }

    std::reverse(names.begin(), names.end());

    return accumulate(names.begin(), names.end(), std::string("/"));
}


void print_unvisited_groups_and_nodes(std::pair<std::vector<Group*>,
				      std::vector<std::pair<Node*,std::stack<Group*> > > > unvisited){
    std::cerr<<"WARNING: some nodes and groups are not used:\n";
    for(auto g:unvisited.first ){
	std::cerr<<"    Group " << g->name << "\n";
    }
    for(auto n:unvisited.second ){
	std::cerr<<"    Node " << format_group_stack(n.second) << "/" <<  n.first->name << "\n";
    }
}






// This function will traverse the DAG to check
// if input to output is connected.
// If fail_on_warn is set, the function will
// treat a warning (unused node) as error.
bool traversal(Group* ast_root, bool fail_on_warn){
    return true;
}
