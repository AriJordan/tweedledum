[![Build Status](https://travis-ci.org/boschmitt/tweedledum.svg?branch=master)](https://travis-ci.org/boschmitt/tweedledum)
[![License](https://img.shields.io/badge/license-MIT-000000.svg)](https://opensource.org/licenses/MIT)

<img src="https://cdn.rawgit.com/boschmitt/tweedledum/master/tweedledum.svg" width="64" height="64" align="left" style="margin-right: 12pt"/>

# tweedledum
tweedledum is a C++-17 header-only library for writing, manipulating, and optimizing quantum circuits.

The library is written to be scalable up to problem sizes in which quantum circuits outperform classical
ones. Also, it is meant to be used both independently and alongside established tools for example
compilers or more general and high level frameworks, e.g., IBM's QISKit, Rigetti's Forest, ProjectQ.

tweedledum powers [RevKit v3](https://github.com/msoeken/cirkit/).
## Installation requirements

A modern compiler is required to build *tweedledum*. We are continuously
testing with:
  * Clang 6.0.1
  * GCC 7.3.0
  * GCC 8.2.0.

## Disclaimer

**tweedledum is in version *Alpha***. Hence, the software is still under active
development and not feature complete, meaning the API is subject to big changes.
This is released for developers or users who are comfortable living on the absolute
bleeding edge.

## License

This software is licensed under the MIT licence (see `LICENSE`).
