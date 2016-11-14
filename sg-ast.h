#ifndef SG_AST_H
#define SG_AST_H

#include<bits/stdc++.h>

// set up structs that will represent the abstract syntax tree

struct Ast_element{
    
};

struct Ast{  // this acts as group as well
    std::string name;
    std::vector<Ast_element*> elements;
};

struct Node : Ast_element{
    std::string name;
};

struct Bash_node : Node{
    std::string command;
};

struct Io_node : Node{
    bool input;
    int file_number;
    
    std::string io_instruction;
    // used while parsing. The exact io
    // instruction will be processed once
    // the entire file has been parsed.
};

struct Instance_node : Node{
    Ast* group;

    std::string group_name;
    // used while parsing. The group can
    // only be linked once the entire file
    // has been parsed.
};

struct Edge : Ast_element{
    Node* source;
    Node* destination;
    
    std::string source_name;
    std::string destination_name;
    // used while parsing. The nodes can
    // only be linked once the entire file
    // has been parsed.
};

Ast* parse_file(std::ifstream &infile);

#endif
