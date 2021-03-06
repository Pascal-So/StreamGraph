#include<bits/stdc++.h>
#include<ctime>
#include "sg-bash-generator.hpp"


std::string sub_group(Group* ast_node);




// takes a varable of the form BASH_VAR without quotes
// and returns "${BASH_VAR}" with quotes in the string
std::string format_bash_variable(std::string var_name){
    return "\"${" + var_name + "}\" ";
}


// prints the command that will stdout the merged
// streams
// args:
//     stream_vars - vector of variable names of the fifos to read
//     horizontal - if true, merge horizontally, otherwise merge vertically
std::string merge_streams(std::vector<std::string> stream_vars, bool horizontal){
    std::string out;
    size_t nr_vars = stream_vars.size();
    
    if(nr_vars == 1){
	out = "cat " + format_bash_variable(stream_vars[0]);
	return out;
    }
    
    
    if(horizontal){
	out = "paste -d\" \" ";
	for(auto v:stream_vars){
	    out += format_bash_variable(v);
	}
    }else{
	// {cat a & cat b & cat c ; } is safer than `cat a b c` because
	// it won't block the execution if two of the pipes are created
	// in the same background process, but it won't preserve order
	// therefore I'm now using `cat <(cat a) <(cat b) <(cat c)`
	out = "cat ";
	for(size_t i = 0; i < nr_vars; ++i){
	    out+= "<(cat " + format_bash_variable(stream_vars[i]) + ") ";
	}
    }

    return out;
}

std::string pipe_to_stream(std::string stream_var){
    return "cat > " + format_bash_variable(stream_var);
}
std::string pipe_to_stream(std::vector<std::string> stream_vars){
    if(stream_vars.size() == 1){
	return pipe_to_stream(stream_vars[0]);
    }
    
    std::string out = "tee ";
    for(auto v:stream_vars){
	out+= format_bash_variable(v);
    }
    return out + ">/dev/null ";
}



std::string read_file(int file_number){
    std::string out;
    out  = "cat \"${input_files[";
    out += std::to_string(file_number-1);
    out += "]}\" ";
    return out;
}


std::string write_file(int file_number){
    std::string out;
    out  = "cat > \"${output_files[";
    out += std::to_string(file_number-1);
    out += "]}\" ";
    return out;
}



std::string execute_input_node(Node* node){
    std::string out;
    if(node->node_type == IO_NODE){
	Io_node* ion = static_cast<Io_node*>(node);
	out = read_file(ion->number);
    }else if(node->node_type == BASH_NODE){
	// input node has been overwritten
	Bash_node* bn = static_cast<Bash_node*>(node);
	out = bn->bash_command + " ";
    }else{
	out = "cat - ";
    }
    return out;
}

std::string execute_output_node(Node* node){
    std::string out;
    if(node->node_type == IO_NODE){
	Io_node* ion = static_cast<Io_node*>(node);
	out = write_file(ion->number);
    }else if(node->node_type == BASH_NODE){
	// output node has been overwritten
	Bash_node* bn = static_cast<Bash_node*>(node);
	out = bn->bash_command + " ";
    }else{
	out = "cat ";
    }
    return out;
}


// only runs bash and instance nodes. input/output nodes
// have to be called separately, because the pipes around
// them need to be set up differently
std::string execute_node(Node* node){
    std::string out;
    if(node->node_type == BASH_NODE){
	out = static_cast<Bash_node*>(node)->bash_command + " ";
    }else{
	Instance_node* inst_node = static_cast<Instance_node*>(node);
	// only call recursive group function if stdin is not empty
	std::string prefix = "";
	if(inst_node->recursive){
	    prefix = "ifne ";
	}
	out = prefix + inst_node->group->name + " ";
    }
    return out;
}

std::string edge_to_fifo_name(Edge* e){
    std::string out;
    out = e->destination->name;
    if(e->mod_destination != NONE ){
	out += "__" + std::to_string(e->mod_nr_destination);
    }
    return out;
}


std::string group_content(Group* ast_node){
    std::string out;

    // first create all the functions for the subgroups
    for(auto g:ast_node->children_groups){
	out += sub_group(g);
    }

    out += "\n";
    
    // create fifos
    for(auto e:ast_node->children_edges){
	out+= "local " + edge_to_fifo_name(e) + "=$(get_fifo)\n";
    }

    out += "\n";
    
    // store the commands in here and print them out later,
    // because we need to run all but one of them as back-
    // ground processes.
    std::vector<std::string> node_commands;
    std::vector<Node*> nodes = ast_node->list_all_nodes();
    for(auto n:nodes){
	std::string command = "";

	if( n->is_input()){
	    command += execute_input_node(n);
	}else{

	    // merge the input edges. This uses the merge_streams function, even if only
	    // one edge leads to this node. Uses "VERTICAL" by default.
	    bool horizontal = false;
	    std::vector<std::string> in_fifos;
	    for(auto e:n->in_edges){
		in_fifos.push_back(edge_to_fifo_name(e));

		// set horizontal to true if at least one of the input edges is horizontal. This
		// is effectively an or gate over the entire input.
		horizontal |= (e->mod_destination == HORIZONTAL);
	    }
	    
	    command += merge_streams(in_fifos, horizontal);
	    command += "| ";

	    if(n->is_output()){
		command += execute_output_node(n);
	    }else{
		command += execute_node(n);
	    }
	    
	}
	
	if( ! n->is_output()){
	    command += "| ";

	    std::vector<std::string> out_fifos;
	    for(auto e:n->out_edges){
		out_fifos.push_back(edge_to_fifo_name(e));
	    }

	    command += pipe_to_stream(out_fifos);
	}

	node_commands.push_back(command);
    }

    for(size_t i = 0; i < node_commands.size() -1; ++i){
	out+= node_commands[i] + "&\n";
    }
    out+= node_commands.back() + "\n";

    return out;
}


std::string sub_group(Group* ast_node){
    std::string out;
    out = ast_node->name + " () {\n";
    out+= group_content(ast_node);
    out+= "}\n";
    return out;
}


std::string print_header(std::string name, std::string sg_core, std::string version_number,
			 bool has_stdin, bool has_stdout,
			 int input_files, int output_files){
    time_t now = time(0);
    // convert `now` to string form
    std::string str_datetime = ctime(&now);
    
    std::string out = "";
    //out+="#!/bin/bash\n";
    out+="\n";
    out+="# " + name + "\n";
    out+="# Compiled on " + str_datetime;
    out+="\n";
    out+="# Compiler:\n";
    out+="# StreamGraph version " + version_number + "\n";
    out+="# Copyright (C) 2016\n";
    out+="# Pascal Sommer\n";
    out+="\n";
    out+="\n";
    out+="# SG Core\n";
    out+="\n";
    out+=sg_core + "\n";
    
    out+="check_io_files ";
    out+=std::to_string((int)has_stdin) + " ";
    out+=std::to_string((int)has_stdout) + " ";
    out+=std::to_string(input_files) + " ";
    out+=std::to_string(output_files) + "\n";
    out+="\n";
    
    out+="# Start actual generated code\n";
    out+="\n";
    return out;
}
std::string print_footer(bool has_stdin){
    std::string out = "\n";
    out+="call_main_function ";
    out+=std::to_string((int)has_stdin) + "\n\n";
    out+="# cleanup\n";
    out+="wait_for_all_fifos_empty\n";
    out+="delete_fifo_dir\n";
    return out;
}

std::string generate_bash_script(Group* ast_root, std::string sg_core, std::string version_number){
    std::string out = "";

    int nr_input_files = 0;
    int nr_output_files = 0;
    for(auto n:ast_root->children_io_nodes){
	Io_node* ion = static_cast<Io_node*>(n);
	if(n->is_input()){
	    nr_input_files = std::max(nr_input_files, ion->number);
	}else{
	    nr_output_files = std::max(nr_output_files, ion->number);
	}
    }

    bool has_stdin = (ast_root->input_node != 0) && ast_root->input_node->node_type == STDIO_NODE;
    bool has_stdout = (ast_root->output_node != 0) && ast_root->output_node->node_type == STDIO_NODE;
    out += print_header(ast_root->name, sg_core, version_number,
			has_stdin, has_stdout, nr_input_files, nr_output_files);

    ast_root->name = "main";
    out += sub_group(ast_root);

    out += print_footer(has_stdin);

    return out;
}
