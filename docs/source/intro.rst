Introduction
============

.. _introduction:

In this document we will to introduce new users to the Kodo library.

Features
--------

Kodo provides several different codes, primarily the basic Random Linear Network
Code and multiple variants.

* Random Linear Network Code (RLNC)

  * Dense variant
  * Sparse variant with fixed density
  * Sparse variant with uniform density
  * Systematic variants
  * Variants where the coding vector is included (for recoding)
  * Variants where a seed is included

* Other codes and approaches

  * Random Annex overlay code
  * Reed-Solomon code
  * Carousel code (round robin scheduling of symbols)


Platforms
---------
Kodo is a plain C++ library so it is portable to a wide range of platforms.
To ensure that we do not break compatibility with a supported platform we
have a buildbot instance building the Kodo libraries. You can check the
status on the `Steinwurf Buildbot`_ page.

.. _Steinwurf Buildbot: http://176.28.49.184:12344

.. note:: The buildbot is used for several different projects you will find the
  kodo project in the overview on the main page.

The buildbot pages will also provide you with up-to-date information on which
platforms and compilers we are currently testing. If you have a specific
platform or compiler which you would like to see Kodo support, `drop us a line`_.

.. _drop us a line: http://steinwurf.com/contact-us/

Tools Needed
------------
In order to start developing using Kodo you need the following tools installed:

1. A **C++11 compliant compiler:** Kodo is a C++11 library so to use it
   you will need a C++ compiler supporting the new C++11 standard. You
   can see a list of compilers on our buildbot page (`Steinwurf Buildbot`_).

2. **Git:** we use the git version control system for managing our libraries.

If you wish to build the Kodo examples and unit-tests it is recommended
that you also install the following:

3. **Python:** needed by our build scripts. We use the Python based `waf`_
   build system.

.. _waf: https://code.google.com/p/waf/

In the following we will provide the links for installing the tools on
a number of different platforms.

Download tools (Ubuntu or other Debian based distributions)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Get the dependencies using the following command:

::

  sudo apt-get install g++ python git-core

Download tools (Windows)
~~~~~~~~~~~~~~~~~~~~~~~~

1. **C++11 compliant compiler:** You need a working C++ compiler we have
   tested using `Visual Studio Express 2012`_ compiler which is free of
   charge. Newer versions should also be fine.

2. **Python:** You need a working Python installation. Find the available
   download on the `Python homepage`_. If you are in doubt which version
   to install you may use the `Python 2.7.3 Windows Installer`_.

3. **Git:** There are several ways to get git on Windows. If you plan to use
   the waf build-scripts to build the Kodo examples and unit-tests, you should
   install the msysgit_ tool. If you do not know which version to install you
   may select the latest version from the `msysgit downloads`_ page.

.. _`Visual Studio Express 2012`: http://www.microsoft.com/visualstudio/eng/downloads
.. _`Python homepage`: http://www.python.org/download/
.. _`Python 2.7.3 Windows Installer`: http://www.python.org/ftp/python/2.7.3/python-2.7.3.msi
.. _msysgit: http://msysgit.github.com/
.. _`msysgit downloads`: https://code.google.com/p/msysgit/downloads/list?q=full+installer+official+git

Download the Kodo sourcecode
----------------------------
There are several ways in which you may get the Kodo library and its
dependencies. Which approach you prefer might depend on you intended
use-case for Kodo.

1. If you wish to simply experiment with Kodo e.g. running some experiments
   or similar. You may use the buildscripts provided in the Kodo project
   to fetch the dependencies automatically.
2. If you wish to use Kodo in a separate project, possibly your own build
   tools. You may wish to download Kodo's dependencies as seperate git
   repositories. For more information about this see the
   section `Using Kodo in Your Application`_.

In the following we will only describe option 1 (using the Kodo buildscripts).

We recommend downloading the library using git, this will allow you to
easily get new updates whenever the library is updated. In addition to
this it is also possible to download the library as either zip or tar.gz
archive. We will describe both approaches in the following:

Recommended: Clone the git repository
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Create a suitable directory for the projects (optional)

   ::

     mkdir dev
     cd dev

2. Clone and download the Kodo libraries by running:

   ::

     git clone git://github.com/steinwurf/kodo.git

Alternative: Download the source code as archives
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

.. _getting_started:

If you are primarily interested in quickly trying some Kodo examples
or building the unit-tests, we have tried to make that really easy.
Provided that you have the `Tools Needed`_ installed.

.. note:: We recommend trying to build and run the unit-tests, before
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

   a. **Linux & Mac**: Run unit-tests from the Kodo directory by running
      ``./build/linux/test/kodo_tests`` in your shell.

   b. **Windows**: Run unit-tests from the Kodo directory by running
      ``build/win32/test/kodo_tests.exe`` in your command prompt.

Customize the build
-------------------

You may customize the build process. The list below describes the most
common changes you may want for you build.

.. note:: You may skip this section if you just want to quickly start with Kodo.

* You may change the compiler.

  To change compiler, set the ``CXX`` variable to you preferred compiler.
  In case you want to use clang++, add ``CXX=clang++`` in front of ``waf configure``.

  ::

    CXX=clang++ python waf configure --bundle=ALL --bundle-path=~/dev/bundle_dependencies

