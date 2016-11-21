import sglex
import sgparse
import sys
import functools, itertools

if len(sys.argv) == 1:
    sys.stderr.write("Need command line arguments")
    sys.exit(1)

input_files = sys.argv[1:]

def read_file( file_name ):
    with open(file_name, 'r') as my_file:
        return my_file.read()

source_codes = map( read_file , input_files )



# lexing the source files

def lex_file( file_content ):
    return sglex.lex(file_content, sglex.token_exprs)

tokens = map( lex_file , source_codes )

# merge the token streams together
tokens = functools.reduce(itertools.chain, tokens)

# print("\n".join(map( "\t".join, tokens) ))


# formatting the token stream

def cleanString(str):
    return " ".join(str.split()[1:])

def cleanStreamItem(tpl):
    return (tpl[0], cleanString(tpl[1])) if tpl[0] in ["node", "edge", "group"] else tpl

tokens = map(cleanStreamItem, tokens)

print("\n".join(map( "\t".join, tokens) ))


# parsing the token streams

def parseGroup(tokens):
    """Parses the main program body or a group/subgroup
    
    Args: tokens - the list of tokens (pair of token type and token content strings)
    Retunrs: pair of group object and remaining tokens
    """

    for (index,token) in enumerate(tokens):
        pass
