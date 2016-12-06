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

## Problems with pipes
* long commands chains illegible
* parallel pipes
* using named pipes is complicated

## Operations
* split
  * by line
  * within line
* join

## Implementation
* in general, create 1 named pipe per block
  use multiple pipes for special split blocks.
* in later version, concat linear node chains to one command

## Design
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

