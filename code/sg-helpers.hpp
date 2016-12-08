#include<bits/stdc++.h>
#include "sg-ast.hpp"

#ifndef SG_HELPERS_HPP
#define SG_HELPERS_HPP


void print_error(std::string message);
void print_error(std::string message, std::string location);

void print_warning(std::string message);
void print_warning(std::string message, std::string location);


std::string join(std::vector<std::string> words, std::string delimiter);

std::vector<std::string> str_split(std::string input);

#endif
