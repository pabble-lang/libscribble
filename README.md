libscribble
===========

Librarys for programming with Scribble/Pabble protocol description
language in C/C++.
This is a library for parsing and working with session-based protocols.
See [http://mrg.doc.ic.ac.uk/](http://mrg.doc.ic.ac.uk/) for details.

[![Build Status](https://travis-ci.org/sessionc/libscribble.svg)](https://travis-ci.org/sessionc/libscribble?branch=v2.0)

[Documentation](http://sessionc.github.io/libscribble) are automatically
generated from the latest source on
[github](https://github.com/sessionc/libscribble/tree/v2.0).

This library includes the role-parametrised **Pabble** extension of Scribble language.

## Build

1. At the project root, fetch the external dependencies with git
   `$ git submodule init && git submodule update`

2. Generate build files with `CMake`
   `$ cd build && cmake ..`

3. While in the `build/` directory, run `make` to build the files,
   then `make test` to run the unit tests to make sure everything is working.
   `$ make && make test`

4. You will find `libscribble.so``, `parser-tool`, `scribble-tool` in the `build/` directory.

## Dependencies

 * sessionc/libsesstype
 * A recent version of lex/yacc
