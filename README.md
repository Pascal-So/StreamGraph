# StreamGraph

*Note that the StreamGraph compiler is kinda working right now, but might still contain a few bugs.*

StreamGraph is a language that tries to solve the problem of linearity in bash one-liners. As a big fan of bash one-liners, I often ran in to situations where I would like to split up the flow of data throught the pipes, treat the separate streams differently and then merge the streams back together. Effectively, I wanted to create a DAG (directed, acyclic graph), where the nodes are bash commands and the edges are bash pipes. So this is what I did.

## Usage

The user writes a program in the StreamGraph language, which have the file extension `.sg`. This StreamGraph program can then be compiled with the following command:

```bash
./sg my-program.sg # will write the output to my-program.sh
```

or equivalently:
```bash
./sg my-program.sg -o my-program.sh
```

The generated shell script can be run on its own and doesn't rely on any further external libraries, because the "core" of the language is included in the compiled file.

The compiled program can take filenames as arguments, if the StreamGraph code requires it to do so. This makes StreamGraph a useful language, even if the project does not require its graph features, because the compiled bash script will print out readable error messages if an incorrect amount of filenames were provided as argument.


## The language

There are three kinds of objecs in StreamGraph:
* Nodes - The operations on data.
* Edges - The flow of data.
* Groups - The StreamGraph equivalent of functions.

The nodes can be further categorized in to three types:
* Bash nodes - Nodes that contain bash commands.
* Instance nodes - Nodes that instanciate groups.
* IO nodes - Nodes that can read from or write to one of the files provided in the arguments.

StreamGraph also allows for comments, which are started with `#` and continue until the end of the current line. Inline comments are allowed as well.

A StreamGraph object is always started with the keyword `n`, `e` or `g` for nodes, edges and groups respectively. Nodes and groups are then named, whereas the edges don't have a name themselves, but reference the names of the nodes they connect. 

An example program in StreamGraph looks like this:
```bash
g moveSecondFieldToStart{
    # nodes with a colon after the name represent
    # a bash command.
    n extract2: cut -d" " -f2


    # edges

    # an edge from the standard input to the node extract2
    e input extract2

    # the suffixes after the dots represent modifiers, which
    # will be explained further down.
    e extract2 output.h0
    e extract2.inv output.h1
}

# nodes with a dash after the name instanciate groups
n rearrange- moveSecondFieldToStart

# nodes where the name is followed by a slash are
# io nodes, they are of the form (infile|outfile) [0-9]+
n stuffToAppend/ infile 1 
n sort: sort # a node can be called the same as the bash command

# edges
e input rearrange
e rearrange output.v0
e stuffToAppend sort
e sort output.v1
```

### Node modifiers

Node modifiers are the suffixes on node names, for example `.h0` in `output.h0`. They are only used when referencing a node in an edge.

The only modifier that can be applied to the source node of an edge is `.inv`, which works on some bash commands (for now only `cut` and `grep`). It tells the edge to fetch the inverse of the output from this node. As an example, this means that getting the output from a bash node conatining the command `grep "a"` with a `.inv` modifier is equivalent to getting the output from the bash node if it had `grep "a" -v` as its command, without `.inv` modifier. Similarly, for `cut`, the part of the data not included in the normal output will be included in the `.inv` output.

```bash
e input node_a
e input node_b

n node_a: grep "a"
n node_b: grep "a" -v

# the edge from node_a to out_a has an inverse modifier
e node_a.inv out_a
e node_b out_b

n out_a/ outfile 1
n out_b/ outfile 2

# the two output files will contain the same data
```

Node modifiers on the destination node have the job of merging two streams. Streams can be merged either horizontally or vertically, as shown here:

Stream a:
```
a
b
c
```

Stream b:
```
1
2
3
```

Stream a and b merged vertically:
```
a
b
c
1
2
3
```

Stream a and b merged horizontally:
```
a 1
b 2
c 3
```

Horizontal and vertical merging are denoted by the `h` and `v` modifiers. The number after `h` or `v` represents the order in which the streams are merged.
