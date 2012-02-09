Introduction
------------

**Kodo** is a flexible C++ network coding, intended to be used for commercial applications and for research on implementation of Network Coding. The library enable researchers to implement, new codes and algorithms, perform simulations, and benchmark the coding operations on any platform where a C++ compiler is available. The library provide a multitude of build blocks and parameters that can be combined in order to create codes. To ensure ease of use several codes are predefined, and high level API's provided.

Tools Needed
------------
In order to build and work with Kodo you need the following tools:
 1. Python
 2. Git
 3. C++ compiler


Getting Started (Quick)
-----------------------
If you are primarily interested in quickly trying some Kodo examples, we have tried to make that really easy. What you have to do is the following:

1. Checkout this repository
2. Build an example
::
  cd kodo/examples/encode_decode_simple
  python waf --bundle=ALL
  python waf configure build

3. Run the executable produced in the build/*platform*/

Note, this requires that you have the `Tools Needed`_ installed.

Getting Started
---------------
Kodo is a header only library which essentially means that all you have to do to use it in your applications it to set the right include path. However, Kodo itself also has a couple dependencies of its own:

1. Boost C++: Kodo depends on a number of headers from the Boost C++ libraries (this dependency will most likely be removed once we switch to C++11, however we need to ensure compiler support on all our target platforms first).
2. Sak: The Sak library is a small header only C++ collection of functionality. 
3. Fifi: The Fifi C++ library provides Finite Field arithmetics needed by the encoding and decoding algorithms in Kodo.

Note, that all dependencies are available as separate repositories on the Steinwurf GitHub page.


Running Tests
~~~~~~~~~~~~~
Before starting to use Kodo it is a good idea to run the unit tests in order to ensure that everything is working as expected. 




