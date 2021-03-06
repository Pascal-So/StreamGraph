#include<bits/stdc++.h>
#define token std::pair<std::string, std::string> // {token type, token content}

// SG AST

// The AST stores an abstract representation of the sg
// program as a tree, which can then be linked to create
// the DAG that is described by the sg code.

// Pascal Sommer, November 2016

#ifndef SG_AST_HPP
#define SG_AST_HPP

enum Io_type{INPUT, OUTPUT};
enum Name_modifier{INVERSE, HORIZONTAL, VERTICAL, NONE};
enum Node_type{IO_NODE, STDIO_NODE, BASH_NODE, INSTANCE_NODE};

struct Node;
struct Edge;
struct Group;

struct Node{
    std::string name;
    Node_type node_type;

    virtual ~Node();

    Group* parent;

    bool visited;

    // is set to false by default because if
    // the node is never even visited by the
    // dfs we know that it cannot be reached.
    bool needed;

    // used by dfs_check_cycles to see if the
    // dfs has entered this node but not left
    // yet.
    bool cycle_dfs_active;
    
    virtual bool is_input() = 0;
    virtual bool is_output() = 0;

    // outwards edges in the DAG. The edge
    // structs contain the additional
    // information such as shadow node
    // modifiers.
    std::vector<Edge*> out_edges;

    std::vector<Edge*> in_edges;
};

struct Bash_node:Node{
    std::string bash_command;

    bool is_input();
    bool is_output();

    Bash_node(std::string bash_command, Group* parent);
};

struct Io_node:Node{
    Io_type io_type;
    int number;

    bool is_input();
    bool is_output();

    Io_node(std::string io_type, int number, Group* parent);
};

struct Instance_node:Node{
    std::string group_name;

    bool recursive;

    bool is_input();
    bool is_output();
    
    Group* group;

    // takes the current namespace of groups as
    // map from group name to group pointer.
    bool link_groups(std::unordered_map<std::string, Group*> &);

    Instance_node(std::string group_name, Group* parent);
};

// these nodes are created by the parser,
// they appear in the source code only
// implicitly as the input and output node.
struct Stdio_node:Node{
    Io_type io_type;
    
    bool is_input();
    bool is_output();

    Stdio_node(Io_type io_type, Group* parent);
};

struct Edge{
    std::string source_name;
    std::string destination_name;
    Name_modifier mod_source;
    Name_modifier mod_destination;
    int mod_nr_destination;

    Node* source;
    Node* destination;

    // does the edge lie on a path from input
    // to output?
    bool needed;

    // takes the current node namespace (only
    // the ones on the same group level) and
    // links the pointers if any of them match.
    bool link_nodes(std::unordered_map<std::string, Node*> &);

    Edge(std::string source_name,
	 std::string mod_source_str,
	 std::string destination_name,
	 std::string mod_destination_str);
};

struct Group{
    bool visited; //used by sg-linker -> link to avoid infinite loop

    Group* parent;
    
    //used by sg-graph checker to see if any instances are made of
    // this group
    bool needed;
    std::string name;
    std::vector<Group*> children_groups;
    std::vector<Bash_node*> children_bash_nodes;
    std::vector<Instance_node*> children_instance_nodes;
    std::vector<Io_node*> children_io_nodes;
    std::vector<Edge*> children_edges;
    Node* input_node;
    Node* output_node;

    Group(Group* parent);
    std::vector<Node*> list_inputs();
    std::vector<Node*> list_all_nodes();
};

#endif
