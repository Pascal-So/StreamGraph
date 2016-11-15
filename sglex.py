import sys
import re

# the sg language is context sensitive, so let me attempt some weird hack:
# the function lex() takes an argument `context`, which stores a dict of
# context flags. This will then be mutated while lexing. Tadaaa, we now
# have a CSG :)

def lex(characters, token_exprs):

    pos = 0
    tokens = []

    while pos < len(characters):
        match = None

        for token_expr in token_exprs:
            pattern, tag = token_expr
            regex = re.compile(pattern, flags=re.IGNORECASE)
            match = regex.match(characters, pos)
            if match:
                text = match.group(0)
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



# The token expressions are pairs:
# (regex, token name)

regex_name = r'[a-zA-Z0-9]+'
regex_name_mod = r'[a-zA-Z0-9]+(\.[a-z0-9]+)?'

token_exprs = [
    (r'[ \n\t]+',     None),
    (r'#[^\n]*',      None),

    (r':',                                        'node_delim_bash'),
    (r'-',                                        'node_delim_instance'),
    (r'/',                                        'node_delim_io'),
    (r'{',                                        'bracket_l'),
    (r'}',                                        'bracket_r'),
    (r'node '+regex_name,                         'node'),
    (r'edge '+regex_name_mod+' '+regex_name_mod,  'edge'),
    (r'group '+regex_name,                        'group'),
    (r'n '+regex_name,                            'node'),
    (r'e '+regex_name_mod+' '+regex_name_mod,     'edge'),
    (r'g '+regex_name,                            'group'),
    (r'infile',                                   'k_infile'),
    (r'outfile',                                  'k_outfile'),
    (r'[0-9]+',                                   'number'),
    (r'[^\n]+',                                   'bash_command'),
]
