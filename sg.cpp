#include<bits/stdc++.h>

#include "sg-lexer.hpp"
#define token std::pair<std::string, std::string> // token type, token content

// The sg compiler
// Pascal Sommer, November 2016

// requires GCC >= 4.4




int main(){
    Lexer l ("ipLookup.sg");

    auto result = l.lex();
    
    for(auto t:result){
	std::cout<< t.first << "\t" << t.second<<"\n";
    }
}
