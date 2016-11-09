# StreamGraph language

## Design phase thoughts

### Nodes and Edges

There are two basic entities in the graph, nodes and edges. It's not always clear whether an edge is, from the users's point of view, more strongly associated with the node it's leading in to (consider a join block where the user's thinking is "I want to get input from there and there") or the node it's leaving (user: "I want to send this stream to stdout"). Probably, the view is not constant for one edge, as the edge is considered once with the sending node and once with the receiving node. It is therefore best to not directly write it in either place. The user should think mainly about the nodes themselves, connections between them are something to be considered later on.

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

### Special nodes

## Known facts

Comments are denoted with the character `#` and can also be inlined. In other words, the interpreter ignores the rest of the line after encountering this character. This alignes with the bash comment character, which makes this very useful.