#include<bits/stdc++.h>
#include<ctime>
#include "sg-bash-generator.hpp"

std::string version_number = "0.1";

std::string group_content(Group* ast_node){
    
}

std::string sub_group(Group* ast_node){
    
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
    out+=sg_core + "\n";
    out+="check_io_files " + std::to_string(input_files) + " ";
    out+=std::to_string(output_files) + "\n";
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

    out += print_header("asdf", sg_core, 1, 0);





    out += print_footer();
    std::cout<<out<<"\n";

    return out;
}
