# StreamGraph

## Considerations
* Split by
  * lines
  * within lines
* n outputs?
  allow streamarrays of variable length? -> error handling?
  consider string to always be array, separated by IFS
* reduce to smallest number of blocks necessary
* types of inputs
  * file input
  * stdin
* goal?
  * fast prototyping
  * one-time complex operations
* side effects?
* idempotence?
* error handling?
* grouping of commands for reuse?
* translate to c++?
  using bash underneath, just export that..
* xargs?
* cyclic?
* blocks with non-fixed number of inputs?

## problems with pipes
* long commands chains illegible
* parallel pipes
* using named pipes is complicated

## operations
* split
  * by line
  * within line
* join

## implementation
* in general, create 1 unnamed pipe per block
  use multiple pipes for special split blocks.
* in later version, concat linear node chains to one command

## design
* `.sg` file, using a new language describing the blocks and pipes
* c++ translater to bash script
  * with option to run immediately
  * maybe make immediately running option default,
    add option to "compile"
* inputs
  * allow file inputs and stdin
  * if one input is used multiple times, print
    a warning, but allow anyway
  * if file argument is passed to script, but none required,
    stdin will read from that file.
    In other words: to have independent input, use a single stdin node
* outputs
  * stdout
    * If multiple output nodes given, order is not guaranteed
    * don't allow file output directly, use flag when running instead
  * stderr
    * every block outputs stderr independently, stderr is (at least
      for now) not handled by sg


## StreamGraph language

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