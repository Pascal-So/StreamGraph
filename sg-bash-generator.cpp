#include<bits/stdc++.h>
#include<ctime>
#include "sg-bash-generator.hpp"

std::string version_number = "0.1";


std::vector<std::string> str_split(std::string input){
    std::vector<std::string> out;
    std::string tmp = "";

    for(auto c:input){
	if(c == ' ' || c == '\n' || c == '\t'){
	    if(tmp != ""){
		out.push_back(tmp);
		tmp = "";
	    }
	}else{
	    tmp+=c;
	}
    }
    return out;
}

std::string inverse_bash_command(std::string command){
    std::vector<std::string> command_words = str_split(command);

    if(command_words.empty()){
	return "";
    }
    
    if(command_words[0] == "grep"){
	return command + " -v";
    }

    if(command_words[0] == "cut"){
	return command + " --complement";
    }

    return ""; //error, can't inverse this command.
}


// takes a varable of the form BASH_VAR without quotes
// and returns "${BASH_VAR}" with quotes in the string
std::string format_bash_variable(std::string var_name){
    return "\"${" + var_name + "}\"";
}


// read_stream prints the command that stdouts the contents
// of the given stream.
std::string read_stream(std::string stream_var){
    return "cat " + format_bash_variable(stream_var) + " ";
}


// prints the command that will stdout the merged
// streams
// args:
//     stream_vars - vector of variable names of the fifos to read
//     horizontal - if true, merge horizontally, otherwise merge vertically
std::string merge_streams(std::vector<std::string> stream_vars, bool horizontal){
    std::string out;
    size_t nr_vars = stream_vars.size();
    if(horizontal){
	out = "paste -d\" \" ";
	for(auto v:stream_vars){
	    out += format_bash_variable(v);
	}
    }else{
	// {cat a & cat b & cat c ; } is safer than `cat a b c` because
	// it won't block the execution if two of the pipes are created
	// in the same background process
	out = "{ ";
	for(size_t i = 0; i < nr_vars; ++i){
	    out+= read_stream(stream_vars[i]);
	    if(i < nr_vars-1){
		out+= "& ";
	    }
	}
	out+= "; } ";
    }

    return out;
}

std::string pipe_to_stream(std::string stream_var){
    return "cat > " + format_bash_variable(stream_var);
}
std::string pipe_to_stream(std::vector<std::string> stream_vars){
    std::string out = "tee ";
    for(auto v:stream_vars){
	out+= format_bash_variable(v);
    }
    return out + " >/dev/null ";
}


std::string read_stdin(){
    return "cat - ";
}

std::string read_file(int file_number){
    std::string out;
    out  = "cat \"${input_files[";
    out += std::to_string(file_number);
    out += "]}\" ";
    return out;
}


std::string execute_input_node(Node* node){
    std::string out;
    if(node->node_type == IO_NODE){
	Io_node* ion = static_cast<Io_node*>(node);
	out = read_file(ion->number);
    }else{
	out = read_stdin();
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
	if(! inst_node->recursive_call){
	    out = inst_node->group_name + " ";
	}else{
	    out = "ifne " + inst_node->group_name + " ";
	}
    }
    return out;
}

std::string group_content(Group* ast_node){
    std::string out;

    

    return out;
}


std::string sub_group(Group* ast_node){
    std::string out;
    out = ast_node->name + " {\n";
    out+= group_content(ast_node);
    out+= " }\n";
    return out;
}


std::string print_header(std::string name, std::string sg_core, int input_files, int output_files){
    time_t now = time(0);
    // convert `now` to string form
    std::string str_datetime = ctime(&now);
    
    std::string out = "";
    out+="#!/bin/bash\n";
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
    out+="check_io_files " + std::to_string(input_files) + " ";
    out+=std::to_string(output_files) + "\n";
    out+="\n";
    out+="# Start actual generated code\n";
    out+="\n";
    return out;
}
std::string print_footer(){
    std::string out = "\n";
    out+="\n";
    out+="# cleanup\n";
    out+="wait_for_all_fifos_empty\n";
    out+="delete_fifo_dir\n";
    return out;
}

std::string generate_bash_script(Group* ast_root, std::string sg_core){
    std::string out = "";

    out += print_header(ast_root->name, sg_core, 1, 0);





    out += print_footer();
    std::cout<<out<<"\n";

    return out;
}
