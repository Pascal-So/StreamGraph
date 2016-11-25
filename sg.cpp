#include<bits/stdc++.h>
#include "sg-lexer.hpp"
#include "sg-parser.hpp"
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

    // create an empty namespace to start the linker method
    Group_namespace gn;
    link(ast, gn);
    
    for(auto t:result){
	std::cout<< t.first << "\t" << t.second<<"\n";
    }
}
