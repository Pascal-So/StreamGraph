import sys
import re

# def is_in_context(pattern, mask):
#     for flag, state in pattern.items():
#         if flag in mask and mask[flag] != state:
#             return False
#     return True

# def update_context(context, switch):
#     for flag, state in switch.items():
#         context[flag] = state
#     return context


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
            pattern, tag, context_req, switch = token_expr
            if not is_in_context(context_req, context):
                continue
            regex = re.compile(pattern, flags=re.IGNORECASE)
            match = regex.match(characters, pos)
            if match:
                text = match.group(0)
                context = update_context(context, switch)
                if tag:
                    token = (text, tag)
                    tokens.append(token)
                break
        if not match:
            print("\n".join(map( "\t".join, tokens) ))
            sys.stderr.write('\nIllegal character: %s\n\n' % characters[pos])
            sys.exit(1)
        else:
            pos = match.end(0)
    return tokens


context = {}
context['bash_command'] = 0 # bash command following
context['names'] = 0 # nr of names to follow

# The token expressions are 4-tuples:
# (regex, token name, applicable in context, sets context)

# This context system does not scale very well, it is therefore useful
# reduce the reliance on the context system to a minimum, and make use
# of the precedence instead.

# every context value is an int which will be incremented and decremented by
# the update values. The context is checked for nonzero if 'applicable in
# context' for this value is set to nonzero, checked for zero otherwise.

token_exprs = [
    (r'[ \n\t]+',     None, {}, {}),
    (r'#[^\n]*',      None, {}, {}),

    (r':',            'node_delim_bash',     {'bash_command': 0, 'names': 0}, {'bash_command': 1}),
    (r'-',            'node_delim_instance', {'bash_command': 0, 'names': 0}, {'names': 1}),
    (r'/',            'node_delim_io',       {'bash_command': 0, 'names': 0}, {}),
    (r'{',            'bracket_l',           {'bash_command': 0, 'names': 0}, {}),
    (r'}',            'bracket_r',           {'bash_command': 0, 'names': 0}, {}),
    (r'node',         'k_node',              {'bash_command': 0, 'names': 0}, {'names': 1}),
    (r'edge',         'k_edge',              {'bash_command': 0, 'names': 0}, {'names': 2}),
    (r'group',        'k_group',             {'bash_command': 0, 'names': 0}, {'names': 1}),
    (r'n',            'k_node',              {'bash_command': 0, 'names': 0}, {'names': 1}),
    (r'e',            'k_edge',              {'bash_command': 0, 'names': 0}, {'names': 2}),
    (r'g',            'k_group',             {'bash_command': 0, 'names': 0}, {'names': 1}),
    (r'infile',       'k_infile',            {'bash_command': 0, 'names': 0}, {}),
    (r'outfile',      'k_outfile',           {'bash_command': 0, 'names': 0}, {}),
    (r'[0-9]+',       'number',              {'bash_command': 0, 'names': 0}, {}),
    
    (r'[a-zA-Z0-9]+(\.[a-z0-9]+)?',
                      'name_mod',            {'bash_command': 0, 'names': 2}, {'names': -1}),
    (r'[a-zA-Z0-9]+', 'name',                {'bash_command': 0, 'names': 2}, {'names': -1}),
    (r'[^\n]+',       'bash_command',        {'bash_command': 1},  {'bash_command': -1}),
    
]
