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

### IO nodes

As demonstrated above, the IO nodes are all numbered, and the numbers then correspond to the filenames given as arguments when calling the compiled scripts.
```bash
./my-compiled-program.sh in_a.txt in_b.txt -o out_a.txt -o out_b.txt
```

To write to the file `out_a.txt` from within the script, the user would now pipe data to this node:
```bash
n that_output_file/ outfile 1
```
Note that the indexes are 1-based.

I decided to go with numbers representing the command line arguments, instead of directly writing down the filenames, for several reasons:
* On the conceptual side, StreamGraph scripts are supposed to care only about the flow of data through it, not about where exactly it comes from. This makes the scripts reusable.
* StreamGraph scripts are compiled to bash scripts. If the filenames were hardcoded in the script, it would either require the user to recompile the script to change the filenames or to find the filenames in the compiled script if the filename changes on the disk.
* The compiled StreamGraph scripts are supposed to be (at least somewhat) portable. Running a StreamGraph script should be independent of the directory structure.

### Groups and recursion

Yes, StreamGraph allows for recursion. An instance node can instanciate any of the groups defined on the same level, or any of its ancestor groups. This makes it possible for the recursion to even go up multiple layers.

The base case of recursion is an empty stream, or a stream with an empty first line. This applies not only to recursion, but holds in general: a group will not be called, if the stream or its first line is empty. Instead, the instance node will return an empty string.

```bash
g print_tail{
  n tail: tail -n +2
  n rec- print_tail
  e input tail
  e tail rec
  e rec output.h1
  e input output.h0
}

n tail- print_tail
e input tail
```
This simple example of recursion produces output like this:

```bash
$ ./sg recursion.sg
$ chmod +x recursion.sh
$ echo -e "a\nb\nc\nd" | ./recursion.sh
a b c d 
b c d 
c d 
d
$
```

Note, however, that recursion is reeeeally slow, because groups internally use bash functions. More than 20 levels of recursion is not recommended.