#include<bits/stdc++.h>
#include "sg-lexer.hpp"
#include "sg-parser.hpp"
#include "sg-ast.hpp"
#include "sg-linker.hpp"
#include "sg-graph.hpp"
#include "sg-bash-generator.hpp"
#include "core-script.cpp" // contains the sg core as string literal
#define token std::pair<std::string, std::string> // token type, token content
#define Group_namespace std::unordered_map<std::string, Group*>

// The sg compiler
// Pascal Sommer, November 2016

// requires GCC >= 4.4




int main(){
    
    Group* ast = new Group();
    generate_bash_script(ast, sg_core);

    
    
    /*
    Lexer l ("program.sg");

    // lex the source code
    auto result = l.lex();
    for(auto t:result){
	std::cout<< t.first << "\t" << t.second<<"\n";
    }

    // parse the token stream
    Group* ast = parse(result);
    if (ast == 0){
	return 1;
    }
    
    // create an empty namespace to start the linker method
    Group_namespace gn;
    // link the DAG pointers in the AST
    bool link_ok = link(ast, gn);
    if ( ! link_ok){
	return 1;
    }

    // create an empty group stack to start the graph checker
    std::stack<Group*> empty_group_stack;
    // check the graph for errors and remove unneccessary parts
    bool graph_ok = groups_check(ast, empty_group_stack);
    if ( ! graph_ok){
	return 1;
    }

    // generate the bash script
    std::string bash_script = generate_bash_script(ast);
    
    for(auto t:result){
	std::cout<< t.first << "\t" << t.second<<"\n";
    }
    */
}
