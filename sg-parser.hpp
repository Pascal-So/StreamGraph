#include<bits/stdc++.h>
#define token std::pair<std::string, std::string> // {token type, token content}


enum Io_type{INFILE, OUTFILE};
enum Name_modifier{HORIZONTAL, VERTICAL, INVERSE};




struct Node{
    std::string name;
    bool has_input();
    bool has_output();
    bool has_inverse();
    void link();
};

struct Bash_node:Node{
    std::string bash_command;

    bool has_inverse();
};

struct Io_node:Node{
    Io_type io_type;
    int number;

    bool has_input();
    bool has_output();
};

struct Instance_node:Node{
    std::string group_name;
    
    
    Group* group;
    void link();
};

struct Edge{
    std::string source_name;
    std::string destination_name;

    Node* source;
    Node* destination;
    void link();
};

struct Group{
    std::string name;
    std::vector<Group*> children_groups;
    std::vector<Node*> children_nodes;
    std::vector<Edge*> children_edges;
};
