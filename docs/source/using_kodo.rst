.. _using_kodo:

Using Kodo in Your Application
==============================

The following describes how you may include Kodo in your own projects
and applications. The following section

Dependencies of Kodo
--------------------

If you decide not to use the Kodo build-scripts to download the dependencies of
Kodo. You may do so manually - this section will describe the steps you need to
take.

In Kodo we try to not reinvent the wheel we therefore rely on
a number of external libraries. These must be available in order
to successfully compile an application using Kodo.

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


Clone the git repository (recommended)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All dependencies needed are hosted on github.com and may be found at
github.com/steinwurf.

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



