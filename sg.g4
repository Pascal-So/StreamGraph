grammar sg;

r : program ;

program : ( expression WS+ ) + ;
expression : ( group | node | edge ) ;

group : K_GROUP WS + NAME WS * LBRACKET WS *  program RBRACKET ;
node : K_NODE WS + NAME WS * ( node_bash | node_io | node_instance ) ;
edge : K_EDGE WS + name_mod WS + name_mod ;


node_bash : BASH_COMMAND;
node_io : NODE_DELIMITER_IO WS + ( INFILE | OUTFILE ) WS * NR ;
node_instance : NODE_DELIMITER_INSTANCE WS * NAME ;

name_mod : NAME ( '.' MOD ) ? ;


NODE_DELIMITER_BASH : ':::' ;
NODE_DELIMITER_IO : '///' ;
NODE_DELIMITER_INSTANCE : '---' ;

INFILE : 'infile' ;
OUTFILE : 'outfile' ;

LBRACKET : '{' ;
RBRACKET : '}' ;

K_GROUP : 'group' | 'g' ;
K_NODE : 'node' | 'n' ;
K_EDGE : 'edge' | 'e' ;

WS : ' ' | '\t' | '\n' ;

NR : [1-9]* DIGIT ;
DIGIT : [0-9] ;

COMMENT : '#' ~[\n]* -> skip ;



MOD : 'inv' | 'h' NR | 'v' NR ;

NAME : [a-zA-Z] [a-zA-Z0-9_]* ;

BASH_COMMAND : NODE_DELIMITER_BASH ~[\n]+ ;