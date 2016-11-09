# StreamGraph language

The StreamGraph language is a small language that will be used to represent the stream graph. It will later be converted to a bash script that includes the core of the StreamGraph language, together with the graph setup.

## Design phase thoughts

These notes are not meant to be a definitive guide to the StreamGraph language, they are mostly a place for me to ramble on about different possibilities and decisions about the language design. They might also be useful if I ever decide to do a write-up on the language and the process of creating it.

These notes are as much about the language itself as they are about the entire concept of creating a stream graph.

### Nodes and Edges

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

### Grouping

Grouping, or creating reusable functions, might be done by allowing the user to denote a subgraph by an identifier, such that the subgraph can be inserted in multiple places without duplicating the code for it.

This could be realized as inserting a subgraph as a single node in to the main graph, which would mean, that edges can go to this node and away from this node. This, in turn, requires the subgraph to have input and output nodes. Already we can see that this is starting to look like the source code will have a main graph and some graphs that represent functions inside this main graph.

Now that I'm thinking about it, if we already went that far, why not allow recursion? After all, the bottom case could be an empty stream, since there are no other conditional structures. There is only stream or no stream (and the contents of the stream of course).

Allocating the stack frames might be a bigger problem tough. Because the core needs a place to store the values while they are being piped, this would quickly lead the script to fill the entire disk if no precautions are taken. An options would be to set a fixed recursion limit.

But now that we're dealing with a language that allows for blocks of codes to be called repeatedly and values to be piped through them, a language that will later be compiled to bash, the question arises: why not write bash scripts directly? What is the advantage of this language over pure bash?

I don't think that the language itself has a lot of advantages, but the things that can be built on top of this language, such as a visual editor. The power of this language lies in the visual representation of the graph, that makes it much easier for the user to see the flow of data. A visual editor might store some metadata like the position of the individual nodes (this could be done by special tags in inline comments), but other than that, the underlying project file for a visual StreamGraph editor would be virtually equivalent to pure StreamGraph files.

### Special nodes

#### Input

#### Output

## Known facts

Comments are denoted with the character `#` and can also be inlined. In other words, the interpreter ignores the rest of the line after encountering this character. This alignes with the bash comment character, which makes this very useful.