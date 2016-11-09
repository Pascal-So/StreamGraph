# StreamGraph language

There are two basic entities in the graph, nodes and edges. It's not always clear whether an edge is, from the users's point of view, more strongly associated with the node it's leading in to (consider a join block where the user's thinking is "I want to get input from there and there") or the node it's leaving (user: "I want to send this stream to the ouput"). It is therefore best to not directly write it in either place. The user should think mainly about the nodes themselves, connections between them are something to be considered later on.

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

  n1: a node named n1
  n2: another node
```

That way, we can later reference them in the edges list.
```
edges:

  n1 n2 # an edge from node n1 to n2
```

support for comments would probably be nice, so let's just use `#` for that. Inline comments allowed

Now the above is not very nice, because we can't have a node called `nodes` (a valid concern, even though this would definitely be bad practice to actually have a node called `nodes`), as it would be interpreted as the beginning of the nodes block.