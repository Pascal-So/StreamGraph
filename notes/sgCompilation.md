# SG Compilation

## Scanner
The scanner is the interface to the file, it reads in to a buffer and passes the data on to the lexer. This way, there is one central place where we can keep track of the line number and character within the line.

Also, this makes it possible to provide a unified interface for some simple backtracking functionality, that allows the lexer to test for a match and jump back to the previous position if the string didn't match.

## Lexer
The lexer gets text from the scanner which it then groups in to tokens. The output is a stream of pairs of the token name and the token content.

## Parser
This is where it gets difficult. The parser must first construct some kind of abstract syntax tree, where the AST-nodes are either groups, nodes or edges, where groups can have sub AST-nodes.

This then has to be converted to the directed acyclic graph described in the `.sg` source file. This could probably be done by adding pointers to the existing AST, and then extracting the DAG by iterating through it. To make this easier, we need some starting points, and it would therefore make sense to add in the hidden input and output node, so that we have no dangling edges (the same goes for the groups).

The additional information carried in the edges (shadow node) could be left in the data structure used to store the edge in the AST-node. It might even be possible to leave the groups in the AST datastructure and just point to them. This means, that we don't have to write a seperate structure, but rather just implement the pointers within the existing structure. This might lead to a lot of pointer confustion though, therefore this needs to be done carefully.