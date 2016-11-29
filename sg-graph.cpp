#include<bits/stdc++.h>
#include "sg-graph.hpp"


// Different typenames are possible if a vector of derived
// class pointers is appended to a vector of base class
// pointers. This leads to ugly gcc error messages though
// if the function is used uncorrectly, so I might have
// to change this.
template<typename T, typename U>
void vector_append(std::vector<T> &a, std::vector<U> &b){
    a.insert(a.end(), b.begin(), b.end());
}



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


bool dfs_reaches_output(Node* n){
    if(n->visited){
	return n->needed;
    }
    n->visited = true;
    
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


bool check_group_connected(Group* ast_node){
    reset_visited_nodes(ast_node);

    bool connected = false;
    for(auto n:ast_node->children_io_nodes){
	if(n->is_input()){
	    connected |= dfs_reaches_output(n);
	}
    }
    connected |= dfs_reaches_output(ast_node->input_node);

    return connected;
}


void reset_group_visited(Group* current_group){
    current_group->visited = false;
    for(auto g:current_group->children_groups){
	reset_group_visited(g);
    }
}

// a group can only be instanciated from a instance
// node on the same level or one further down. The
// instanciation further down only matters if the
// group gets instanciated at least one from the
// outside. Therefore, to see if a group is needed,
// we only need to check on the same level.
void determin_groups_needed(Group* ast_node){
    for(auto n:ast_node->children_instance_nodes){
	if(n->needed){
	    
	}
    }
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
