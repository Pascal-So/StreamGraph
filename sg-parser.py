import parsec as ps

# setup the parsers.
#
# Notation conventions:
# parsers with the prefix "f_" return something that
# can be converted to json in a meaningful way
# the type of the parsed code.
# parsers with the prefix "k_" parse a keyword

ws = ps.spaces()

name = ps.regex("[a-zA-Z0-9]+")
name_mod = name + ps.string(".") + name

bash_command = ps.regex("[^\n]+")

number = ps.regex("[0-9]+")
k_infile = ps.string("infile")
k_outfile = ps.string("outfile")
infile = k_infile + ws + number
outfile = k_outfile + ws + number

def format_file(res):
    #stuff

f_file = ps.parsecmap(infile ^ outfile, format_file)


k_node = ps.string("n") ^ ps.string("node")
k_edge = ps.string("e") ^ ps.string("edge")
k_group = ps.string("g") ^ ps.string("group")


