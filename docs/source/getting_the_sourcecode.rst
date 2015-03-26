Kodo Dependencies
=================

.. _sourcecode:



Obtain a license for KODO
-------------------------

Before you download or use KODO **you MUST obtain a valid license**. 
This can be done by filling out the license request form_.

.. _form: http://steinwurf.com/license/


Download the Kodo sourcecode
----------------------------

There are several ways in which you may get the Kodo library and its
dependencies. Which approach you prefer might depend on your intended
use-case for Kodo.

1. If you wish to simply experiment with Kodo e.g. running some experiments
   or similar. You may use the build-scripts provided in the Kodo project
   to fetch the dependencies automatically.
2. If you wish to use Kodo in a separate project, possibly your own build
   tools. You may wish to download Kodo's dependencies as separate git
   repositories. For more information about this see the
   section :ref:`using-kodo-in-your-application`.

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
Provided that you have the :ref:`tools-needed` installed.

.. note:: We recommend trying to build and run the unit-tests, before
          using Kodo in your own project. However, if you want to skip
          this step you may jump directly to
          :ref:`using-kodo-in-your-application`


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

