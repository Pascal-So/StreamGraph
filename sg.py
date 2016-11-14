import sys
import re

# the sg language is context sensitive, so let me attempt some weird hack:
# the function lex() takes an argument `context`, which stores a dict of
# context flags. Tadaaa, we now have a CSG :)

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



token_exprs = [
    (r'[ \n\t]+',   None),
    (r'#[^\n]*',    None),

    (r':',          'Reserved'),
    (r'-',          'Reserved'),
    (r'/',          'Reserved'),
    (r'{',          'Reserved'),
    (r'}',          'Reserved'),
    (r'node',       'Reserved'),
    (r'edge',       'Reserved'),
    (r'group',      'Reserved'),
    (r'n',          'Reserved'),
    (r'e',          'Reserved'),
    (r'g',          'Reserved'),
]
