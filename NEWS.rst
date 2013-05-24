News for Kodo
============

This file lists the major changes between versions. For a more detailed list
of every change, see the Git log.

Latest
------
* Bug: Fix behavior or deep_symbol_storage::set_symbols() to set all
  symbols.
* Bug: When reusing decoders contaning the linear_block_decoder layer
  systematic symbols where not correctly initialized. A fix was added
  to the linear_block_decoder and the unit test test_reuse_incomplete
  now checks for this issue.

9.0.0
-----
* Minor: Adding the shallow storage decoder which allows decoding objects
  larger than a single block or generation.
* Minor: Adding new example of an on-the-fly encoder. This example shows
  how to use an encoder before all symbols for that encoder has become
  available. This can be very useful in cases where data is produced
  on-the-fly, e.g. some form of streaming. The example can be found in the
  "examples/encode_on_the_fly" folder.
* Minor: Adding a deep storage decoder which provides the memory needed to
  decode a large object spanning several blocks.
* Minor: Adding simple decoding example. This example shows how to input
  symbols and coefficients directly into the decoder and how some of the
  debug layers can be applied to debug the decoding process.
* Bug: Made all factories non-copyable by making copy constructor and copy
  assignment constructor private.
* Bug: Fix issue in recoding_symbol_id, where sak::copy_storage triggered an
  assert when using an decoder with symbols less than max_symbols.
* Major: Changed construct() and initialize() functions of a coding layer to
  accept the factory as the only parameters. This breaks backwards
  compatibility with Kodo version 8.0.0.
* Major: Changed the factory build() function to accept no parameters.
  Factories are now configured e.g. using set_symbols(uint32_t) before
  calling build() to create a new encoder / decoder.
* Minor: Moved finite_field_counter layer from the benchmarks to the
  library source folder. This makes it possible to add it to any codec
  stack where the number of finite field operations should be monitored.
* Minor: Updated to waf-tools version 2.x.y
* Major: Updating dependency to gauge version 5.x.y

8.0.0
-----
* Updated layers to follow new API specifications. This change will break
  most existing layers. The API specification is available in the doxygen
  folder.
* New API for object/file encoders, makes it easier to support custom data
  sources e.g. such as memory mapped files.
* Added file encoder example.
* Replaced the old generator API with new Coefficient Generator API.
* Updated the factory APIs to pass the factory to each layer. This
  simplifies construction in several layers, where previously it was not
  possible to access the factory.
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


