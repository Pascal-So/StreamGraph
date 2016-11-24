#include<bits/stdc++.h>
#define token std::pair<std::string, std::string> // {token type, token content}


enum Io_type{INPUT, OUTPUT};
enum Name_modifier{HORIZONTAL, VERTICAL, INVERSE};

struct Node;
struct Edge;
struct Group;

struct Node{
    std::string name;
    
    bool has_input();
    bool has_output();
    
    // is modifier .inv applicable? Defaults to
    // false, overridden by Bash_node if known
    bool has_inverse(); 

    // takes the current namespace of groups as
    // map from group name to group pointer.
    // Defaults to noop but is overriden by the
    // instance nodes.
    bool link_groups(std::unordered_map<std::string, Group*>);

    // outwards edges in the DAG. The edge
    // structs contain the additional
    // information such as shadow node
    // modifiers.
    std::vector<Edge*> out_edges;
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
    bool link_groups(std::unordered_map<std::string, Group*>);
};

// these nodes are created by the parser,
// they appear in the source code only
// implicitly as the input and output node.
struct Stdio_node:Node{
    Io_type type;
    
};

struct Edge{
    std::string source_name;
    std::string destination_name;

    Node* source;
    Node* destination;

    // takes the current node namespace (only
    // the ones on the same group level) and
    // links the pointers if any of them match.
    bool link_nodes(std::unordered_map<std::string, Node*>);
};

struct Group{
    std::string name;
    std::vector<Group*> children_groups;
    std::vector<Bash_node*> children_bash_nodes;
    std::vector<Instance_node*> children_instance_nodes;
    std::vector<Io_node*> children_io_nodes;
    std::vector<Edge*> children_edges;
};
