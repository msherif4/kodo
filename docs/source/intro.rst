.. _introduction:
Introduction
============
In this document we will to introduce new users to the Kodo library.

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
dependencies. We recommend downloading the library using git, this
will allow you to easily get new updates whenever the library is
updated. In addition to this it is also possible to download the
library as either zip or tar.gz archive. We will
describe both approaches in the following:

Clone the git repository (recommended)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

Waf (build system)
------------------
We use the Waf build-system to build all Kodo examples and
unit-tests. Waf is a Python based build-system and supports
a wide varity of use-case. You may read more about Waf at
the project homepage: http://code.google.com/p/waf/

A Waf project contains two types of files:

1. The ``waf`` file this the actual build-system executable.
   This file is not meant to be edited.
2. A number of ``wscript`` files, these files contain the
   project build information. You can think of it as a type
   of ``makefile`` written in Python.

.. note:: See the ``waf`` build options by running ``python waf -h``
          in your favorite command-line shell.


Quick Start (building Kodo examples and unit tests)
---------------------------------------------------

If you are primarily interested in quickly trying some Kodo examples
or building the unit-tests, we have tried to make that really easy.
Provided that you have the `Tools Needed`_ installed.

.. note:: We recommend trying to build and run the unit-test, before
          using Kodo in your own project. However, if you want to skip
          this step you may jump directly to :ref:include-kodo-in-project


1. Navigate to the directory containing the Kodo sources:
   ::
     cd dev/kodo/

2. Invoke ``waf`` to build the Kodo unit-tests and examples.
   ::
     python waf configure --bundle=ALL --bundle-path=~/dev/bundle_dependencies

   The ``waf configure`` ensures that all tools needed by Kodo are
   available and prepares to build Kodo.
   Kodo relies on a number of auxiliary libraries (see :ref:kodo-dependencies)
   by specifying the ``--bundle=ALL`` command we instruct ``waf`` to
   automatically download these. The ``--bundle-path`` informs ``waf``
   about where the downloaded libraries should be placed. You may
   omit the ``--bundle-path`` option in that case ``waf`` will create a
   local directory in the Kodo folder called  ``bundle_dependencies`` and
   store the libraries there.

3. Invoke ``waf`` to build the unit-tests and examples.
   ::
     python waf build

4. Run the produced executables depending on the host platform.

   a. **Linux**: Run unit-tests from the Kodo directory by running
      ``./build/linux/test/kodo_tests`` in your shell.

   b. **Windows**: Run unit-tests from the Kodo directory by running
      ``build/win32/test/kodo_tests.exe`` in your command prompt.




