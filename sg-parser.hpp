#include<bits/stdc++.h>
#include "sg-ast.hpp"
#define token std::pair<std::string, std::string> // {token type, token content}

#ifndef SG_PARSER_HPP
#define SG_PARSER_HPP

Group* parse(std::vector<token> tokens);

#endif
