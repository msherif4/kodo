Kodo Manual
=============
This is the Kodo manual in this document we will try to introduce
users to the Kodo library.

Platforms
---------
Kodo is a plain C++ library so it should be reasonable portable. We
try to keep everything working and most platforms. To ensure that
we don't break a supported platform we have a buildbot instance
building the Kodo libraries. You can check the status on the
`Buildbot Waterfall`_ page.

.. _Buildbot Waterfall: http://176.28.49.184:12344/waterfall

.. note:: The waterfall currently shows several different libraries
  you should look for the ones called kodo-*

The buildbot currently builds for the following platforms:

 * Windows XP and Windows 7.
 * Various Linux distributions.
 * Mac OS Lion.

See the details on the `build slaves`_ page.

.. _build slaves: http://176.28.49.184:12344/buildslaves

Tools Needed
------------
In order to build and work with Kodo you need the following tools:

1. **C++ compiler:** Kodo is a C++ library so to use it you will need a
   working C++ compiler installed. We have tested the following
   compilers with Kodo: g++ (v4.4.3 and up), clang++ (v3.0) and Visual
   Studio Express 10.

If you wish to build the Kodo examples and unit-tests it is recommended
that you also install the following:

2. **Python:** any version later than 2.7 or 3.0 should be fine.
3. **Git:** any version should be fine.

Download tools (Ubuntu or other Debian based distros)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Simply get the dependencies using the following command:
::
  sudo apt-get install g++ python git-core

Download tools (Windows)
~~~~~~~~~~~~~~~~~~~~~~~~

1. **C++ compiler:**: You need a working C++ compiler we have tested using
   `Visual Studio Express 10`_ compiler which is free of charge.
   Newer versions should also be fine.
2. **Python:** You need a working Python installation. Find the available
   download on the `Python homepage`_.
3. **Git:** There are several ways to get git on Windows. We have tested the
   msysgit_ tool. However, github now also have a tool for managing
   git repositories called `github for Windows`_ which may be used
   (we have not tested this).

.. _`Visual Studio Express 10`: http://www.microsoft.com/visualstudio/en-us/products/2010-editions/express
.. _`Python homepage`: http://www.python.org/download/
.. _msysgit: http://msysgit.github.com/
.. _`github for Windows`: http://windows.github.com/

Alternatively you may be able to build and test Kodo in
Cygwin but we have not tested this (let us know if you try :)).

Download the sources
--------------------
There are several ways in which you may get the Kodo library and its
dependencies. We recommend downloading the libraries using git, this
will allow you to easily get new updates whenever the libraries are
updated. In addition to this it is also possible to download the
libraries as either zip or tar.gz archives. We will
describe both approaches in the following:

Clone the git repository (recommended)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All dependencies needed are hosted on github.com and may be found at
http://github.com/steinwurf.

1. Create a suitable directory for the projects (optional)
   ::
     mkdir dev
     cd dev

2. Clone and download the Kodo libraries by running:
   ::
     git clone git://github.com/steinwurf/kodo.git

Download the source code as archives
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Download the zip or tar.gz archive from the following location and
extract it to a desired location.

* https://github.com/steinwurf/kodo/downloads

see :ref:`rsttest` for more info
:ref:`guide-connecting`

Waf (build system)
------------------
Once you have downloaded the Kodo source code to your computer you
may want to build the Kodo examples and unit-test. The easiest way
to do this is to use our Waf build scripts. Waf is a Python based
build tool.


Quick Start (building Kodo examples and unit tests)
---------------------------------------------------
If you are primarily interested in quickly trying some Kodo examples,
we have tried to make that really easy. Provided that you have the
`Tools Needed`_ installed.



Compiling an Example
....................

1. Checkout this repository
2. Build an example

   ::

      cd kodo/examples/encode_decode_simple
      python waf --bundle=ALL
      python waf configure build

3. Run the executable produced in the *build/platform/* directory

Running Tests
.............
Before starting to use Kodo you should build and run the unit tests in order to ensure that everything is working correctly.

   ::

      ./waf configure --bundle=ALL
      ./waf
      ./build/linux/test/kodo_tests

If you wish to use several of our project you might want to specify a common path to where bundles should be located, this way you avoid cloning each repository multiple times

   ::

      ./waf configure --bundle=ALL --bundle-path=../../bundles



Using Kodo
..........
Kodo is a header only library which essentially means that all you have to do to use it in your applications is to set the right include path. However, Kodo itself also has a couple dependencies:

1. Boost C++: Kodo depends on a number of headers from the Boost C++ libraries (this dependency will most likely be removed once we switch to C++11, however we need to ensure compiler support on all our target platforms first).
2. Sak: The Sak library is a small header only C++ collection of functionality.
3. Fifi: The Fifi C++ library provides Finite Field arithmetics needed by the encoding and decoding algorithms in Kodo.

*Note, that all dependencies are available as separate repositories on the Steinwurf GitHub page.*




