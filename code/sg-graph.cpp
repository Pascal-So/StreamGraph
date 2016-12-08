#include<bits/stdc++.h>
#include "sg-graph.hpp"
#include "sg-helpers.hpp"

// SG graph checks and optimizations

// These functions perform some checks on the graph to
// eliminate dead ends and unneeded functions. This will
// lead to smaller compiled files in the end. Maybe this
// will be deferred to optimization flags later on.

// The responsibilities of these functions:
// - Check for two elements of the same name
// - Check inverses and create split nodes
// - Check if a path from input to output exists
// - Remove out_edges on nodes leading to dead ends
// - Remove unneeded nodes
// - Remove unneeded edges
// - Remove unneeded groups
// - Check for cycles
// - Check inputs to nodes
//
// The checks are performed in this order. They happen
// recursively on every level, starting from the hightest
// level. This way, nothing gets executed for the groups
// that aren't needed and the user isn't bothered by
// unnecessary warnings for groups that exist, but are
// not connected.


// helper functions:
void reset_visited_nodes(Group* ast_node);
std::string format_group_stack(std::stack<Group*> stack);

template<typename T, typename U>
void vector_append(std::vector<T> &a, std::vector<U> &b);


// check fro two groups of the same name
bool check_duplicate_elements(Group* ast_node, std::string location);

// Check inverses and create split nodes
bool check_inverses_and_create_split_nodes(Group* ast_node, std::string location);

// check if path form input to output exists
bool check_group_connected(Group* ast_node);

// remove dead out edges
void dfs_remove_dead_out_edges(Node* n);

// remove unneeded nodes
void remove_unneeded_nodes(Group* ast_node, std::string location);

// remove unneeded edges
void remove_unneeded_edges(Group* ast_node);

// remove unneeded groups
void remove_unneeded_groups(Group* ast_node, std::string location);

// remove cycles
std::vector<Node*> dfs_check_cycles(Node* n);

// check inputs to nodes
bool check_inputs_to_nodes(Group* ast_node, std::string location);


// main functions ----------------------------------------------------------------------------------

// returns false in case of error. Prints error messages.
bool group_check(Group* ast_node, std::string location){
    bool duplicates_ok = check_duplicate_elements(ast_node, location);
    if( ! duplicates_ok){
	// check duplicate elements already prints its own error messages,
	// we don't need to print one here.
	return false;
    }

    bool inverses_ok = check_inverses_and_create_split_nodes(ast_node, location);
    if( ! inverses_ok){
	// check inverses already prints the relevant error messages, we
	// don't need to print one here.
	return false;
    }

    // check if group input to output is connected
    bool connected = check_group_connected(ast_node);
    if( ! connected){
	print_error("No path from input to output", location);
	return false;
    }

    // remove the out edges leading to dead nodes from every input point
    auto input_nodes = ast_node->list_inputs();
    for(auto n:input_nodes){
	dfs_remove_dead_out_edges(n);
    }

    remove_unneeded_nodes(ast_node, location);
    remove_unneeded_edges(ast_node);
    remove_unneeded_groups(ast_node, location);

    // check for cycles
    reset_visited_nodes(ast_node);
    std::vector<Node*> cycle;
    input_nodes = ast_node->list_inputs();
    for(auto n:input_nodes){
	cycle = dfs_check_cycles(n);
	if( ! cycle.empty()){
	    // found a cycle, stop checking any further.
	    break;
	}
    }
    if(! cycle.empty()){
	// found a cycle
	print_error("The program forms an infinite loop.", location);
	for(auto n:cycle){
	    std::cerr<<"  " << n->name << "\n";
	}
	return false;
    }

    bool inputs_ok = check_inputs_to_nodes(ast_node, location);
    if( ! inputs_ok){
	// check_inputs_to_nodes already prints its own relevant error messages, we
	// don't need to print one here
	return false;
    }
    
    return true;
}


bool groups_check(Group* current_group, std::stack<Group*> current_group_stack){
    current_group_stack.push(current_group);

    std::string location = format_group_stack(current_group_stack);
    
    if(! group_check(current_group, location)){
	return false;
    }
    
    for(auto g:current_group->children_groups){
	if( ! groups_check(g, current_group_stack)){
	    return false;
	}
    }

    return true;
}




// implementations -----------------------------------------------------------------


// - CHECK FOR TWO ELEMENTS OF THE SAME NAME -------------------------------------------
std::string check_duplicate_group(Group* ast_node){
    std::unordered_set<std::string> group_names;
    for(auto g:ast_node->children_groups){
	std::string name = g->name;
	if(group_names.find(name) != group_names.end()){
	    return name;
	}
	group_names.insert(name);
    }
    return "";
}


std::string check_duplicate_node(Group* ast_node){
    std::unordered_set<std::string> node_names;

    std::vector<Node*> nodes = ast_node->list_all_nodes();
    
    for(auto n:nodes){
	std::string name = n->name;
	if(node_names.find(name) != node_names.end()){
	    return name;
	}
	node_names.insert(name);
    }
    return "";
}

bool check_duplicate_elements(Group* ast_node, std::string location){
    // check if group has conflicting group or node definitions
    std::string duplicate_group = check_duplicate_group(ast_node);
    if(duplicate_group != ""){
	print_error("The group " + duplicate_group + " is defined multiple times.", location);
	return false;
    }

    std::string duplicate_node = check_duplicate_node(ast_node);
    if(duplicate_node != ""){
	print_error("The node " + duplicate_node + " is defined multiple times.", location);
	return false;
    }
    return true;
}



// - CHECK INVERSES AND CREATE SPLIT NODES ---------------------------------------



std::string invert_command(std::string command){
    std::vector<std::string> command_words = str_split(command);

    if(command_words.empty()){
	return "";
    }
    
    if(command_words[0] == "grep" ||
       command_words[0] == "egrep" ){
	return command + " -v";
    }

    if(command_words[0] == "cut"){
	return command + " --complement";
    }

    return ""; //error, can't invert this command.
}

bool check_inverses_and_create_split_nodes(Group* ast_node, std::string location){
    for(auto e:ast_node->children_edges){
	if(e->mod_source == INVERSE){
	    // check if the node to be inverted is a bash node
	    if(e->source->node_type != BASH_NODE){
		print_error("Can't invert node " + e->source->name +
				 " as it is not a bash node.", location);
		return false;
	    }
	}
    }

    auto bash_nodes = ast_node->children_bash_nodes;
    for(auto n:bash_nodes){
	std::vector<Edge*> inverted_out;
	std::vector<Edge*> normal_out;
	for(auto e:n->out_edges){
	    if(e->mod_source == INVERSE){
		inverted_out.push_back(e);
	    }else{
		normal_out.push_back(e);
	    }
	}
	if(inverted_out.empty()){
	    // current bash node does not have inverted output
	    continue;
	}
	
	std::string command = n->bash_command;
	    
	// check if command can be inverted
	std::string inverted = invert_command(command);
	if(inverted == ""){
	    print_error("Can't invert commad " + command + " in node " + n->name + ".", location);
	    return false;
	}

	// the node will now be splitted in to three nodes. one that keeps the
	// input from the original node, which makes it easier to handle cases
	// where the original node might be inputting from multiple places, one
	// that contains the original command and one that conatins the inverse
	// command.

	Bash_node* splitter = n; // the in edges should be pointing to `splitter`
	splitter->out_edges.clear();
	splitter->bash_command = "cat";
	
	Bash_node* cmd_node = new Bash_node(command, ast_node);
	Bash_node* inv_node = new Bash_node(inverted, ast_node);
	cmd_node->name = splitter->name + "__cmd";
	inv_node->name = splitter->name + "__inv";

	Edge* e_split_cmd = new Edge(splitter->name, "", cmd_node->name, "");
	Edge* e_split_inv = new Edge(splitter->name, "", inv_node->name, "");
	e_split_cmd->source = splitter;
	e_split_cmd->destination = cmd_node;
	e_split_inv->source = splitter;
	e_split_inv->destination = inv_node;

	ast_node->children_edges.push_back(e_split_cmd);
	ast_node->children_edges.push_back(e_split_inv);

	splitter->out_edges.push_back(e_split_cmd);
	splitter->out_edges.push_back(e_split_inv);

	cmd_node->out_edges = normal_out;
	inv_node->out_edges = inverted_out;

	ast_node->children_bash_nodes.push_back(cmd_node);
	ast_node->children_bash_nodes.push_back(inv_node);
    }
    
    return true;
}

// - CHECK IF A PATH FROM INPUT TO OUTPUT EXISTS ----------------------------------


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

    if( reaches_output ){
	// flag node as needed because it lies on a path from in to out.
	n->needed = true;
    }else{
	// set `needed` flag on edges leading away from this node to
	// false as well.
	for(auto e:n->out_edges){
	    e->needed = false;
	}
	n->needed = false;
    }
    
    return reaches_output;
}


bool check_group_connected(Group* ast_node){
    reset_visited_nodes(ast_node);

    bool connected = false;
    auto input_nodes = ast_node->list_inputs();
    // check file input nodes
    for(auto n:input_nodes){
	connected |= dfs_reaches_output(n);
    }

    return connected;
}



// - REMOVE OUT_EDGES ON NODES LEADING TO DEAD ENDS -----------------------------



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
	}else{
	    // before this point, only edges leading out of unneeded nodes
	    // were set to unneeded, now edges leading in to unneeded nodes
	    // will be set to unneeded as well.
	    e->needed = false;
	}
    }
    n->out_edges = new_out_edges;
}


// - REMOVE UNNEEDED NODES ----------------------------------------------

// T should be a derived node like Bash_node or an edge
// the elements in the vector that have the `needed` flag set to false will
// be returned in a new pointer vector and removed from the original vector,
// the other ones remain, filling in the now empty places.
template<typename T>
std::vector<T*> extract_unneeded_elements_from_vector(std::vector<T*> & elements){
    int i = 0;
    std::vector<T*> out;
    for(auto & el:elements){
	if(! el->needed){
	    out.push_back(el);
	}else{
	    elements[i] = el;
	    ++i;
	}
    }
    elements.resize(i);

    return out;
}


// This function has to be called after dfs_remove_dead_out_edges,
// because otherwise the pointers in the out_edges vector might
// refer to deleted objects.
void remove_unneeded_nodes(Group* ast_node, std::string location){
    auto unneeded_bash_nodes = extract_unneeded_elements_from_vector(ast_node->children_bash_nodes);
    auto unneeded_instance_nodes = extract_unneeded_elements_from_vector(ast_node->children_instance_nodes);
    auto unneeded_io_nodes = extract_unneeded_elements_from_vector(ast_node->children_io_nodes);
    
    std::vector<Node*> remove;
    vector_append(remove, unneeded_bash_nodes);
    vector_append(remove, unneeded_instance_nodes);
    vector_append(remove, unneeded_io_nodes);
    if( ! ast_node->input_node->needed ){
	remove.push_back(ast_node->input_node);
	ast_node->input_node = 0;
    }
    if( ! ast_node->output_node->needed ){
	remove.push_back(ast_node->output_node);
	ast_node->output_node = 0;
    }

    if(! remove.empty()){
	print_warning("Removing unneeded nodes:", location);
	for(auto n:remove){
	    std::cerr<<"    " << n->name <<"\n";
	    delete n;
	}
    }
}

// - REMOVE UNNEEDED EDGES -----------------------------------------

void remove_unneeded_edges(Group* ast_node){
    auto remove = extract_unneeded_elements_from_vector(ast_node->children_edges);
    for(auto e:remove){
	delete e;
    }
}

// - REMOVE UNNEEDED GROUPS -------------------------------------------------


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


void remove_unneeded_groups(Group* ast_node, std::string location){
    determine_groups_needed(ast_node);
    
    auto remove = extract_unneeded_elements_from_vector(ast_node->children_groups);
    if(! remove.empty()){
	print_warning("Removing unneeded groups:", location);
	for(auto g:remove){
	    std::cerr<<"    " << g->name <<"\n";
	    delete g;
	}
    }
}


// - CHECK FOR CYCLES -------------------------------------------------------



// if a cycle is encountered this returns the
// cycle nodes where the first node that was
// reached is both the first and the last entry
// in the returned vector.
std::vector<Node*> dfs_check_cycles(Node* n){
    std::vector<Node*> cycle_nodes;

    if(n->visited){
	// this has already been checked and
	// no cycle has been found.
	return cycle_nodes;
    }

    n->visited = true;
    
    if(n->cycle_dfs_active){
	cycle_nodes.push_back(n);
	return cycle_nodes;
    }

    n->cycle_dfs_active = true;

    for(auto e:n->out_edges){
	cycle_nodes = dfs_check_cycles(e->destination);
	
	if(! cycle_nodes.empty()){
	    // one of the sub-calls detected a cycle.

	    // check if the cycle nodes list already contains the
	    // entire cycle
	    if(cycle_nodes.size() > 1){
		if(cycle_nodes[0] == cycle_nodes[cycle_nodes.size()-1]){
		    // cycle has already been completely detected.
		    return cycle_nodes;
		}
	    }

	    // cycle nodes list is not complete yet, add current node
	    // to cycle nodes list
	    cycle_nodes.push_back(n);

	    return cycle_nodes;
	}
    }

    // no cycles were detected
    n->cycle_dfs_active = false;

    return cycle_nodes;
}



// - CHECK INPUTS TO NODES ------------------------------------------------

bool check_inputs_to_nodes(Group* ast_node, std::string location){
    // add edge pointer to the in_edges vector on all edge destinations
    for(auto e:ast_node->children_edges){
	e->destination->in_edges.push_back(e);
    }

    std::vector<Node*> nodes = ast_node->list_all_nodes();
    
    for(auto n:nodes){
        if(n->in_edges.size() > 1){
	    // check if only h or v at a time have been used
	    bool horizontal = false;
	    bool vertical = false;
	    for(auto e:n->in_edges){
		horizontal |= (e->mod_destination == HORIZONTAL);
		vertical |= (e->mod_destination == VERTICAL);
		if(e->mod_destination == NONE){
		    print_error("Node " + n->name + " has multiple inputs but at least " +
				     "one of them without join modifier.", location);
		    return false;
		}
	    }
	    if(horizontal && vertical){
		print_error("Node " + n->name + " has both horizontal and vertical " +
				 "inputs.", location);
		return false;
	    }

	    // sort the in-edges by the mod number
	    auto sorting_lambda = [](Edge* a, Edge* b) -> bool{
		return (a->mod_nr_destination) < (b->mod_nr_destination);
	    };
	    std::sort(n->in_edges.begin(), n->in_edges.end(), sorting_lambda);

	    // check if two in-edges have the same mod number
	    int last_mod_nr = n->in_edges[0]->mod_nr_destination;
	    for(int i = 1; i < (n->in_edges.size()); ++i){
		int curr_mod_nr = n->in_edges[i]->mod_nr_destination;
		if(curr_mod_nr == last_mod_nr){
		    print_error("Multiple inputs to node " + n->name + " with the same " +
				"merge number.", location);
		    return false;
		}
		last_mod_nr = curr_mod_nr;
	    }
	    
	}else{
	    // only one input, we don't care about extension
	}
    }
    return true;
}


// helper functions -------------------------------------------------------

// only resets the nodes on one level.
void reset_visited_nodes(Group* ast_node){
    std::vector<Node*> nodes = ast_node->list_all_nodes();
    for(auto n:nodes){
	n->visited = false;
	n->cycle_dfs_active = false;
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

    return join(names, "/");
}

// Different typenames are possible if a vector of derived
// class pointers is appended to a vector of base class
// pointers. This leads to ugly gcc error messages though
// if the function is used incorrectly, so I might have
// to change this.
template<typename T, typename U>
void vector_append(std::vector<T> &a, std::vector<U> &b){    
    a.insert(a.end(), b.begin(), b.end());
}
