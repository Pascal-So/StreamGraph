import sys
import re

# the sg language is context sensitive, so let me attempt some weird hack:
# the function lex() takes an argument `context`, which stores a dict of
# context flags. This will then be mutated while lexing. Tadaaa, we now
# have a CSG :)

def lex(characters, token_exprs, context):

    pos = 0
    tokens = []

    while pos < len(characters):
        match = None

        for token_expr in token_exprs:
            pattern, tag = token_expr
            regex = re.compile(pattern)
            math = regex.match(characters, pos, flags=re.IGNORECASE)
            if match:
                text = match.group(0)
                if tag:
                    token = (text, tag)
                    tokens.append(token)
                break
        if not match:
            sys.stderr.write('Illegal character: %s\\n' % characters[pos])
            sys.exit(1)
        else:
            pos = match.end(0)
    return tokens


context = {}
context['bash_node'] = False

# The token expressions are 4-tuples:
# (regex, token name, applicable in context, sets context)

token_exprs = [
    (r'[ \n\t]+',   None, {}, {}),
    (r'#[^\n]*',    None, {}, {}),

    (r':',          'node_delim_bash', {}, {}),
    (r'-',          'node_delim_instance', {}, {}),
    (r'/',          'node_delim_io', {}, {}),
    (r'{',          'bracket_l', {}, {}),
    (r'}',          'bracket_r', {}, {}),
    (r'node',       'k_node', {}, {}),
    (r'edge',       'k_edge', {}, {}),
    (r'group',      'k_group', {}, {}),
    (r'n',          'k_node', {}, {}),
    (r'e',          'k_edge', {}, {}),
    (r'g',          'k_group', {}, {}),
    (r'[^\n]'       '')
]
