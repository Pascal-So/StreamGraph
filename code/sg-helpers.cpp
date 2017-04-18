#include<bits/stdc++.h>
#include "sg-helpers.hpp"

void print_error(std::string message){
    std::cerr<<"\033[0;31mERROR\033[0m: " << message <<"\n";
}
void print_error(std::string message, std::string location){
    std::cerr<<"\033[0;31mERROR\033[0m in " << location << ": " << message <<"\n";
}

void print_warning(std::string message){
    std::cout<<"\033[0;33mWarning\033[0m: " << message <<"\n";
}
void print_warning(std::string message, std::string location){
    std::cout<<"\033[0;33mWarning\033[0m in " << location << ": " << message <<"\n";
}




std::string join(std::vector<std::string> words, std::string delimiter){
    std::string out = "";

    size_t n = words.size();
    for(size_t i = 0; i < n; ++i){
	out += words[i];
	if(i < n-1){
	    out += delimiter;
	}
    }

    return out;
}


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
    if(tmp != ""){
	out.push_back(tmp);
	tmp = "";
    }
    return out;
}
