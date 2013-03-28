.. _using-kodo-in-your-application:

Using Kodo in your application
==============================
In the following we will describe what you need to do to use Kodo in your
application / project.

.. _kodo-dependencies:
Kodo dependencies
-----------------
In Kodo we rely on a number of external libraries, these must be available
in order to successfully compile an application using Kodo.

The functionality used from these libraries are **header-only** which
means that you only have to specify the correct includes paths to use
them. The libraries are:

1. **Fifi**: this library contains
   finite field arithmetics used in ECC (Error Correcting Code) algorithms.

   * http://github.com/steinwurf/fifi

2. **Sak**: this library contains a few
   utility functions used in Kodo such as endian conversion.

   * http://github.com/steinwurf/sak

3. **Boost** C++ libraries: this library contains wide range
   of C++ utilities. We use only a subset of these functionalities such as
   smart-pointers.

   * http://boost.org

If you have tried to use the Kodo build scripts you will notice that these
download a number of additional libraries. These libraries are
only needed when/if you want to compile the Kodo unit-tests, examples or
benchmarks.

Download Kodo Dependencies
--------------------------

All dependencies required by Kodo are hosted on github.com and may be found at
github.com/steinwurf.

There are several ways in which you may get the Kodo library and its
dependencies. Which approach you prefer might depend on your intended
use-case for Kodo.

1. As shown in :ref:`quick-start` the Kodo build scripts supports downloading
   the dependency repositories. The build script with do a git clone and
   checkout the latest compatible tagged version of the dependency.

2. You may wish to manually download Kodo's dependencies as separate git
   repositories. In the following we describe this option.

Clone the git repository (recommended)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Create a suitable directory for the projects (optional)

   ::

     mkdir dev
     cd dev

2. Clone and download the Fifi libraries by running:

   ::

     git clone git://github.com/steinwurf/fifi.git

3. Clone and download the Sak libraries by running:

   ::

     git clone git://github.com/steinwurf/sak.git

4. Clone and download the Boost C++ libraries by running:

   ::

     git clone git://github.com/steinwurf/external-boost.git

  .. note:: Boost can be installed in many different ways.
            For example many Linux distributions support installing Boost
            via the package manager. Alternatively Boost also provides
            its own version control repositories, if you
            wish, you may also use download Boost using those repositories.

Example using makefile / command-line
-------------------------------------

If you would like to see an example of building an application with
Kodo without using any fancy build-system we provide a small makefile
which shows how to invoke the ``g++`` compiler. The example can be found
in the ``examples/sample_makefile`` folder in the `Kodo repository`_.

.. _`Kodo repository`: https://github.com/steinwurf/kodo



..
   Example using Waf (extended)
   ----------------------------

   .. warning:: This example still need to be added to the sources

   If you wish to use Waf to build your application you may utilize its
   functionality to setup dependencies for Kodo. The ``waf`` build-script
   found in the Kodo source package has been extended with a number of
   tools to make managing dependencies easier. The tools added to the
   ``waf`` executable may be found here: https://github.com/steinwurf/external-waf

   If you wish to use the same approach you can find an example building
   an application using Kodo and the modified ``waf`` executable here:

   If are already using a unmodified version of Waf or would like to
   use a plain Waf build-script see next section.


   Example using Waf (unmodified)
   ------------------------------

   .. warning:: This example still need to be added to the sources

   If you wish to use a plain Waf version downloaded from
   http://code.google.com/p/waf/ the following example shows one
   possible way of using it to build an application with Kodo.





