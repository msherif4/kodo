News for Kodo
============

This file lists the major changes between versions. For a more detailed list
of every change, see the Git log.

Latest (head of the master branch)
----------------------------------
* Moved source files to the src folder
* Added operations counter benchmark
* Switched to use the cxx-gauge project for the benchmark code
* Bump fifi to version 3
* Bump boost to version 2
* Bump sak to version 4

2.0.1
-----
* Created linear_block_xyz codes as a building block for future and existing
  block code implementations.
* Refactored RLNC recoding functionality to utilize allow a larger re-use of
  existing code.
* Added initial version of encoding/decoding throughput benchmarks.
* Fixed Mac support in build scripts

2.0.0
-----
* Updated the Waf build system to handle dependencies better. The new tools
  used are not compatible with the old versions so we have to bump the major
  version.


