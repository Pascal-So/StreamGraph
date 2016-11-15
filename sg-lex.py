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

# This context system does not scale very well, it is therefore useful
# reduce the reliance on the context system to a minimum, and make use
# of the precedence instead.

token_exprs = [
    (r'[ \n\t]+',     None, {}, {}),
    (r'#[^\n]*',      None, {}, {}),

    (r':',            'node_delim_bash',     {'bash_command': False}, {'bash_command': True}),
    (r'-',            'node_delim_instance', {'bash_command': False}, {}),
    (r'/',            'node_delim_io',       {'bash_command': False}, {}),
    (r'{',            'bracket_l',           {'bash_command': False}, {}),
    (r'}',            'bracket_r',           {'bash_command': False}, {}),
    (r'node',         'k_node',              {'bash_command': False}, {}),
    (r'edge',         'k_edge',              {'bash_command': False}, {}),
    (r'group',        'k_group',             {'bash_command': False}, {}),
    (r'n',            'k_node',              {'bash_command': False}, {}),
    (r'e',            'k_edge',              {'bash_command': False}, {}),
    (r'g',            'k_group',             {'bash_command': False}, {}),
    
    (r'[a-zA-Z0-9]+\.(inv | v[0-9]* | h[0-9]*)',
                      'name_mod',            {'bash_command': False}, {}),
    (r'[a-zA-Z0-9]+', 'name',                {'bash_command': False}, {}),
    (r'[^\n]+',       'bash_command',        {'bash_command': True},  {'bash_command': False}),
    
]
