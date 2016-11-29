#include<bits/stdc++.h>
#include "sg-lexer.hpp"
#include "sg-parser.hpp"
#include "sg-ast.hpp"
#include "sg-linker.hpp"
#define token std::pair<std::string, std::string> // token type, token content
#define Group_namespace std::unordered_map<std::string, Group*>

// The sg compiler
// Pascal Sommer, November 2016

// requires GCC >= 4.4




int main(){
    Lexer l ("program.sg");

    auto result = l.lex();

    Group* ast = parse(result);

    if (ast == 0){
	return 1;
    }
    
    // create an empty namespace to start the linker method
    Group_namespace gn;
    if ( ! link(ast, gn)){
	return 1;
    }

    traversal(ast, false);
    
    for(auto t:result){
	std::cout<< t.first << "\t" << t.second<<"\n";
    }
}
