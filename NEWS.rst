News for Kodo
============

This file lists the major changes between versions. For a more detailed list
of every change, see the Git log.

Latest
------
* Updated waf to 1.7.9 and enabled use of the wurf_install_path tool.

7.0.1
-----
* Fixed missing include in storage.hpp

7.0.0
-----
* Update waf to use external tools repository
* Updated waf to not use git local clones which are creating hardlinks
  that are not allowed on the "afs" filesystem

6.0.0
-----
* Bump fifi to version 6
* Bump sak to version 7

5.0.0
-----
* Bump fifi to version 5.x.y.
* Bump sak to version 6.x.y.

4.0.0
-----
* Using .hpp extension instead of .h

3.0.1
-----
* Removing asserts on symbol size. Fifi should handle symbol sizes
  not a multiple of 16.

3.0.0
-----
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


