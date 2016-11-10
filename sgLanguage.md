# StreamGraph language

The StreamGraph language is a small language that will be used to represent the stream graph. It will later be converted to a bash script that includes the core of the StreamGraph language, together with the graph setup.

These notes are as much about the language itself as they are about the entire concept of creating a stream graph.

## Nodes and Edges

There are two basic entities in StreamGraph, nodes and edges, where the nodes represent operations on the data, and the edges represent the flow of the data. It's not always clear whether an edge is, from the users's point of view, more strongly associated with the node it's leading in to (consider a join block where the user's thinking is "I want to get input from there and there") or the node it's leaving (user: "I want to send this stream to stdout"). Probably, the view is not constant for one edge, as the edge is considered once with the sending node and once with the receiving node. It is therefore best to not directly write it in either place. The user should think mainly about the nodes themselves, connections between them are something to be considered later on.

As such it would make sense to have a section called "nodes" and a section called "edges" or "pipes" in the `.sg` file, where the respective entities are listed as one per line.

```
nodes:
(...)
edges:
(...)
```

We need to know from where to where the edges go, therefore we need some way of labelling the nodes.
```
nodes:

  n1: a node named `n1`
  n2: another node
```

That way, we can later reference them in the edges list.
```
edges:

  n1 n2 #an edge from node `n1` to `n2`
```

support for comments would probably be nice, so for the sake of simplicity let's just use `#` for that. Inline comments allowed.

Now the above is not very nice, because we can't have a node called `nodes` (a valid concern, even though this would definitely be bad practice to actually have a node called `nodes`), as it would be interpreted as the beginning of the nodes block. We therefore need a better way to delimit the nodes and edges block, or a different way to describe the edges.

* A possibility would be to not separate the nodes and edges at all, as long as there is still only one entity per line. This would still leave room for the user to separate them voluntarily and emulate the `nodes` and `edges` lables with comments.

  That way, however, there would have to be some kind of syntax to differentiate between the node and edge entries. To ensure that no ambiguity is created, we first need to define the set of allowed node names.

* An alternative would be to create a syntax that denotes nesting, for example curly braces, to delimit the `nodes` and the `edges` block.
  ```
  nodes{
    n1: a node named `n1`
    nodes: another node # this is not ambiguous anymore
  }
  edges{
    n1 nodes # an edge from node `n1` to `nodes`
  }
  ```
  This approach has the advantage that grouping becomes easier to add later on, as nesting is already a supported concept.

This decision seems to boil down to whether or not grouping of parts of the graph for reuse will be a feature worth adding at some point in the future (maybe even already with the initial version). So let's think about grouping first.

## Grouping

Grouping, or creating reusable functions, might be done by allowing the user to denote a subgraph by an identifier, such that the subgraph can be inserted in multiple places without duplicating the code for it.

This could be realized as inserting a subgraph as a single node in to the main graph, which would mean, that edges can go to this node and away from this node. This, in turn, requires the subgraph to have input and output nodes. Already we can see that this is starting to look like the source code will have a main graph and some graphs that represent functions to be used inside this main graph.

Now that I'm thinking about it, if we already went that far, why not allow recursion? After all, the bottom case could be an empty stream, since there are no other conditional structures. There is only stream or no stream (and the contents of the stream of course).

Allocating the stack frames might be a bigger problem tough. Because the core needs a place to store the values while they are being piped, this would quickly lead the script to fill the entire disk if no precautions are taken. An options would be to set a fixed recursion limit.

But now that we're dealing with a language that allows for blocks of codes to be called repeatedly and values to be piped through them, a language that will later be compiled to bash, the question arises: why not write bash scripts directly? What is the advantage of this language over pure bash?

I don't think that the language itself has a lot of advantages, but the things that can be built on top of this language, such as a visual editor. The power of this language lies in the visual representation of the graph, that makes it much easier for the user to see the flow of data. A visual editor might store some metadata like the position of the individual nodes (this could be done by special tags in inline comments), but other than that, the underlying project file for a visual StreamGraph editor would be virtually equivalent to pure StreamGraph files.

## Special nodes

One goal of the streamgraph language is to abstract the interaction with named pipes, inputs, outputs, etc. away, in favor of representing the pure flow of the data. This means (at least in my interpretation), that the language should inherently be hostile to side effects. Since the nodes run arbitrary shell commands, we can't stop them, but we can call side effects bad streamgraph coding style. The script should have one or multiple inputs and a single output that do all the file system interaction. If the thing you want to achieve cannot be modelled within these constraints, don't use streamgraph for this project (one exception might be interaction with a database).

All input and output will therefore be happening though some special nodes, that don't access the file system directly, the sg core will do the file system accessing. This has the advantage, that the code can be reused if the filenames change, without adjusting some string constants in the script.

### IO

As hinted at above, the script doesn't know the names of the files that will be accessed. The script represents actions on some general data and shouldn't be concerned with how we get this data.

In general, a script can have two types of input. One is stdin, the other is command line arguments. sg is mainly about applying the same transformation to a stream of data multiple times, taking the data from a different source every time, therefore, the command line arguments will only be filenames. While it is very well possible, that a script takes multiple files as input, the general case will be, that the script takes only one file as input, or reads all of its data directly from stdin.

If we have multiple inputs, we use multiple corresponding input nodes, where input node i relates to the file mentioned in the i-th command line argument. It should also be possible to still read from stdin in addition to reading from the files mentioned in the command line arguments. Thus, we have two kinds of input node:
* A `stdin` node that can only be used once.
* A `file-input` node that can be used multiple times but has to be numbered, where the number represents the position of the desired filename in the command line arguments.

If there is only one input source (probably the average case), the input should be accepted from either stdin or as a filename in a command line argument. This could be modelled by the user, by creating nodes for both inputs and concatenating the streams, but this is such a common case that this should probably be modelled in the language. If only one input node is given, be it `stdin` or `file-input`, the sg core should make the script accept the input to come from either end.

A similar setup might be possible for the ouput. The user could, when running the script, pass multiple output files as arguments (for example with `-o somefile.txt`), and there would still be stdout. We could therefore have two kinds of output nodes, one for file output and one for stdout, where, again, if the user provides only one output node, whatever its kind, the output would go to stdout if no `-o` argument is given and to the file otherwise.

This would result in a nice symmetry between how the input and output is handled. The only downside is that there might arise some confusion from how a single IO node can differently, depending on whether a file name argument is present or not. This could be fixed by defining a preferred form. Let's define, that, if this switching IO functionality is to be used (I really need a cool name for this concept), the single node should be the stdin/stdout node, not a filein/fileout node. Using a filein/fileout node for context dependent IO should still be allowed, but maybe print out a compiler warning that stdin/stdout nodes are preferred if only one input, respectively output node is used.

As for stderr: Each node can have some stderr ouput, but the graph should probably just ignore this, so that it will be displayed in the terminal when running the graph.

Groups need an input and output as well, and they should be limited to only a single input and a single output. It would make sense to reuse the stdin/stdout nodes here in order to not introduce even more node types, and that way, a group could quickly be converted to a main program and vice versa.

### Split

The whole point of having a graph is so that we can process data in parallel streams, and merge them back together afterwards. This means that at some point we have to have nodes with multiple outputs.

One way of having multiple outputs is to simply duplicate outputs. This should be possible with all nodes, by simply adding two outgoing edges from this node. The outgoing edges should duplicate the stream.

Another way to split the output would be by sending some of the lines to one outgoing edge, the other lines to the other edge. This should already be possible by using thee nodes. One that simply passes the data through, which will have an outgoing edge to both of the other nodes each. The other two nodes would be a `grep` node and a `grep -v` node respectively. This does represent some overhead though, and would be nicer to solve in a single node with two outputs. The two outputs would in this case be labelled, as we need a way to separate the positive and negative output.

Since rewriting grep is neither an option nor a sensible thing to do, the option arises to add functionality to `grep` nodes. Say a user writes a grep node, the system could add a second, virtual node in the background. This node could be named with a special suffix to the node. Let's disallow dots in node names, which would mean we can add suffixes after a dot that the user can then still refer to in outgoing edges. A grep node called `findWarnings` would create a shadow node called `findWarnings.inv` that adds `-v` to the grep command, without the user having to write any additional sg code. Dealing with `-A` and `-B` might be tricky though..

To ensure preformance, the `.inv` shadow node would only be created if the user is actually referencing it. Actually, this is relevant for more than just performance, beause if we have a node writing data in to a named pipe but nothing is reading from that pipe, the process might not terminate.

Streams could also be split within the lines, using a tool like `cut`. This should also be possible to invert and create a shadow node from it, but might require a bit more trickery. Since `cut` might take fields from the middle of the line, the inverse would be the part around it. This could just be evaluated separately and concatenated to form .. oh never mind, cut has a `--complement` option.. (might not be entirely porable though, requires GNU cut afaik)

### Merge

For merging back together, there are, again, two options. Merging within the line and just concatenating the streams. Actually it would be useful if every node could concatenate streams by just sending multiple pipes there. The order could not be guaranteed though, but for most purposes this should be fine. If the order needs to be set, the user could prepend a string to each of the streams and sort by the first field after merging to get the desired arrangement.

Merging within the line might be tricky, since the order is important. Actually, this could be solved with shadow nodes, where the user adds a suffix to the destination node of the edge. A suffix like `.after` could make sg join the streams line by line, the unsuffixed input being the base input and the `.after` stream being appended to every line of the base stream.

Since we can't ask the users to always use the same field separator, as this could be a space, a tab, a semicolon, a comma, etc., we have to add a way for the user to tell sg the field separator to be used for merging inline. This might be done with a command line option when running the script, such as `-d$'\t'`. Either space or tab should be the default, as these should be the most common options.

A difficulty arises when there are more than two streams to be merged. If we however replace the suffixes with something more flexible, this should work. The inline merge suffix could be anything that will then be merged in lexicographical order. This might even be possible to use with stream concatenation merging (I will call this vertical merging from now on, while inline merging is horizontal merging).

The virtual node suffixes could be the command for the merging direction (vertical or horizontal), and a number, by which the streams will be sorted before merging. The virtual node suffixes would therefore look something like this in the edge syntax:

```
sourceA mergingNode.v0
sourceB mergingNode.v1
```

This would concatenate the streams from sourceA and sourceB, where the data from sourceA would occur first in the merged stream. The `v` stands for vertical.

Analogously, the syntax for edges to a node where the streams would be merged horizontally would look like this:

```
sourceA mergingNode.h0
sourceB mergingNode.h1
sourceC mergingNode.h2
```

where `h` stands for horizontally. As demonstrated here, the system should be able to join an arbitrary number of streams.

Now the question arises, what should be done with an edge streaming to the node without a suffix? Probably just display a warning when compiling, but allow it anyway, just as if `.h0` / `.v0` had been written. If two clashing suffixes or suffixes of vertical and horizontal type appear at the same time, the compiler will display an error message.

## Nodes again

Nodes can either be bash statements or a group instance. The whole point of groups is to have multiple instances, we therefore need to instance them so we can pipe data through the individual instances independently (this is gonna be an important part in the implementation. How efficiently can this be implemented?).

Thus we need a syntax to differentiate between a group instance and a simple node, otherwise we can't have group names that are equal to bash commands. Initially, I was going to separate the node name from the command with a colon, this could be altered to use the colon syntax only for bash command nodes, and some other character for instantiation, for example a dash.

```
someNode: sed 's/a/b/'
otherNode- groupName
```

Once the group has been instanciated in a node, the node can be used like every other node.

The IO nodes might just have yet another separator, so that the input and output nodes can also be named with semantics in mind:

```
fileForDoingThingA/ 1
fileForDoingThingB/ 2
mainInput/ stdin
```

This would make the node `fileForDoingThingA` read from the file specified by the first command line argument, `fileForDoingThingB` would read from the filename in the second argument. `mainInput` would read from stdin.

The slash used as a separator is quite pretty actually, as it reminds of the filesystem, which is exactly the purpose of this kind of node.

Output nodes could use the backslash as a separator, although this would bring the number of separators up to 4 and the user might get confused between slash and backslash. Actually, they could both use the forward slash and be differentiated like this:

```
fileForReading/ infile 1
input/ stdin
fileForWriting/ outfile 1
output/ stdout
```

Actually, it's questionable whether stdin and stdout should be defined explicitly. They will be used almost always, so we could add them by default. Let's call them `input` and `output`. This has the effect that the concept for switching input changes a bit. If no file input node is defined but a file input is given, then this file will be used as stdin and the actual stdin will be ignored. Same goes for the output. This means that no nodes named `input` or `output` can be created, but that would be bad practice anyway.

Since nodes can have shadow nodes that use dots in the name, the language shouldn't allow the user to add any special characters in the node name. Allowed are therefore only letters (upper- and lowercase) and digits.

## Edges again

Edges should specify the node sending data in to the edge (the node where this edge is outgoing), and the node it's leading in to.

```
someNode otherNode
```

Shadow nodes can only be accessed from edges, the syntax for accessing them should look like this:

```
someNode.inv otherNode.v1
```

This requires `someNode` to be a `grep` or a `cut` node. `otherNode` will concat this stream with another stream from an edge also leading in to `otherNode`.

### Groups again

Nested groups might be useful for namespaces, so they should be allowed, even if they get rarely ever used. Groups can only be instanciated by nodes that are defined on the same level as the group, or by nodes in the group or in any of its descendant groups.

This is what allows for recursion, with a node reaching out to a group an arbitrary amount of layers up. Recursion will stop, once the stream is empty, because the group does not get opened. If a maximum recursion depth is reached

## Syntax

Now that some of the details are a bit clearer, it should be possible to decide on a syntax. The language should allow for nodes and edges in the main part and also in groups. Nested groups are allowed, the elements that can appear at every level are therefore:

* Node
* Edge
* Group
* Comment

Nodes and edges make up the main part of an sg program, they should therefore be as easy to write as possible, which means, no long prefixes required. An option would be to have prefixes anyway, but allow the user to write them in a short form consisting only of the first letter, similar to GDB:

```
group aGroup{
    node someNode: (node code)
    node otherNode- (group name)
    edge someNode otherNode
}
```

or written in the short form:

```
g aGroup{
    n someNode: (node code)
    n otherNode- (group name)
    e someNode otherNode
}
```

The indentation should be optional.

As an example, a sg source file might look like this:

```
g moveSecondFieldToEnd{
    # nodes
    n extract2: cut -d" " -f2

    # edges
    e input extract2
    e extract2 output.h0
    e extract2.inv output.h1
}

# nodes
n rearrange- moveSecondFieldToEnd
n stuffToAppend/ infile 1
n sort: sort # note how a node can have the same name as a command

# edges
e input rearrange
e rearrange output.v0
e stuffToAppend sort
e sort output.v1
```

Stream merging can be done on the output, as seen in this example.

## Coding style

I'm not sure yet whether separating the statements in to node and edges sections makes sense for larger projects, as one might lose track of the edges or nodes.

Due to the nature of the language, there will almost always be more edges than nodes, which might seem like unnecessary verbosity, but this makes the user think about the flow of data more than the details of the individual actions, which is sort of the goal of the language.

Nodes that don't have a path to an output (should) have no effect, as side effects are discouraged. These nodes will be deleted and therefore never executed, if one places side effects in such nodes, they are thus not guaranteed to run.
