import parsec as ps

# setup the parsers.
#
# Notation conventions:
# parsers with the prefix "f_" return something that
# can be converted to json in a meaningful way
# the type of the parsed code.
# parsers with the prefix "k_" parse a keyword

ws = ps.spaces()
number = ps.regex("[0-9]+")



k_node = ps.string("node") ^ ps.string("n")
k_edge = ps.string("edge") ^ ps.string("e")
k_group = ps.string("group") ^ ps.string("g")


# name parsing

name = ps.regex("[a-zA-Z0-9]+")
name_mod = name + ps.string(".") + name

def format_name(res):
    return {"name": res}

f_name = ps.parsecmap(name, format_name)

def format_name_mod(res):
    return {"name_mod": (res[0][0], res[1])}

f_name_mod = ps.parsecmap(name_mod, format_name_mod)

# bash command parsing

bash_command = ps.regex("[^\n]+")

def format_bash_command(res):
    return {"bash_command": res}

f_bash_command = ps.parsecmap(bash_command, format_bash_command)


# file parsing

k_infile = ps.string("infile")
k_outfile = ps.string("outfile")
infile = k_infile + ws + number
outfile = k_outfile + ws + number

def format_file(res):
    return {res[0][0]: res[1]}

f_file = ps.parsecmap(infile ^ outfile, format_file)




# node parsing

k_delim_bash = ps.string(":")
k_delim_instance = ps.string("-")
k_delim_io = ps.string("/")

node_bash = k_delim_bash + ws + f_bash_command
node_instance = k_delim_instance + ws + f_name
node_io = k_delim_io + ws + f_file

node = k_node + ws + f_name + (node_bash ^ node_instance ^ node_io)

def format_node(res):
    """Returns node in form {"node": {"name": nodename, type: value}}"""
    node_content = {}
    if "name" in res[1][1]: # is an instance node
        node_content["instance"] = res[1][1]["name"]
    else: # is bash command or file
        node_content = res[1][1]
    
    node = {}
    node.update(res[0][1]) # name property
    node.update(node_content) # node type and value
    return {"node": node}

f_node = ps.parsecmap(node, format_node)



# edge parsing

edge = k_edge + ws + (f_name_mod ^ f_name) + ws + (f_name_mod ^ f_name)

def format_edge(res):
    return {"edge": (res[0][0][1], res[1])}

f_edge = ps.parsecmap(edge, format_edge)


# group parsing, needs to be a function to work recursively

def parse_program(program):
    group = k_group + ws + f_name + ps.string("{") + ws + ???
