#include<bits/stdc++.h>
#include "sg-ast.hpp"
#define Node_namespace std::unordered_map<std::string, Node*>
#define Group_namespace std::unordered_map<std::string, Group*>

#ifndef SG_LINKER_HPP
#define SG_LINKER_HPP


bool link(Group* ast, Group_namespace group_namespace);

bool transform(Group* ast);


#endif
