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


# parsing the token streams

