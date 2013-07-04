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

   * http://github.com/steinwurf/external-boost-light

If you have tried to use the Kodo build scripts you will notice that these
download a number of additional libraries. These libraries are
only needed when/if you want to compile the Kodo unit-tests, examples or
benchmarks.

4. **Waf-tools**: This repository contains additional tools used by
   out build system. These tools adds functionality to waf which are
   used e.g. by our continuous-integration build system.

   * http://github.com/steinwurf/external-waf-tools

5. **Gtest**: The Google C++ Unit Testing Framework is used by all the
   Kodo unit tests to ensure the library functions correctly.

   * http://github.com/steinwurf/external-gtest

6. **Gauge**: Gauge is a C++ benchmarking tool which we use in Kodo to
   profile the implemented algorithms.

   * http://github.com/steinwurf/cxx-gauge

.. _selecting-the-correct-versions:

Selecting the correct versions
------------------------------
If you use the automatic approach by letting the build scripts download the
dependencies, they will select the latest compatible version. If you download
the dependencies manually you will have to select a compatible version. This
information is stored in the ``wscript`` file found in Kodo's root folder.

Within that file you will find all Kodo's dependencies specified in the
following way:

::

  bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'fifi',
            git_repository = 'github.com/steinwurf/fifi.git',
            major_version = 9))

The above command sets up a dependency for the Fifi library. The version
required is specified in the ``major_version = 9`` line. This means that Kodo
requires version ``9.x.y`` of the Fifi library, where ``x.y`` should be
selected to pick the newest available version. Visiting the download page
at github.com for the Fifi library:

* https://github.com/steinwurf/fifi/releases

We get a list of available versions. At the time of writing this would be
version ``9.1.0``. These version numbers are available as ``git tags`` if you
choose to manually checkout the git repositories.

Download Kodo Dependencies
--------------------------

All dependencies required by Kodo are hosted on github.com and may be found
at http://github.com/steinwurf.

There are several ways in which you may get the Kodo library and its
dependencies. Which approach you prefer might depend on your intended
use-case for Kodo.

1. As shown in :ref:`getting_started` the Kodo build scripts supports
   downloading the dependency repositories automatically. The build
   script with do a ``git clone`` and checkout the latest compatible tagged
   version of the dependency.

2. You may wish to manually download Kodo's dependencies as separate git
   repositories. In the following we describe this option.

3. You can also download the Kodo dependencies as zip or tar.gz archives
   from the dependencies corresponding github.com page.

Before moving on it is important to stress that downloading all
dependencies is only necessary if you wish to build the Kodo unit tests
and benchmarks using the Kodo build system. If you simply want to use Kodo
in your application you only need to download the Fifi, Sak and Boost
dependencies and you do not need to build the Kodo library (since it is
header-only).
If that is your goal you can skip to the `Example application using
makefile`_ section after downloading the three required libraries..


Download using Git
..................

1. Create a suitable directory for the projects (optional)

   ::

     mkdir dev
     cd dev

2. Clone or download the Fifi libraries by running:

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
            wish, you may also download Boost using those repositories.

5. Clone and download the extra Waf-tools:

   ::

     git clone git://github.com/steinwurf/external-waf-tools.git

6. Clone and download the Gtest library.

   ::

     git clone git://github.com/steinwurf/external-gtest.git


7. Clone and download the Gauge library.

   ::

     git clone git://github.com/steinwurf/cxx-gauge.git

Now we have to visit the downloaded repositories and select the correct
versions e.g. for Fifi, first list the available tags:
::

  cd fifi
  git tag -l

Using the information from the ``wscript`` (described in
`Selecting the correct versions`_) we can checkout a tagged version:

::

  git checkout 9.1.0

We now do this for all the downloaded repositories.

Download as zip/tar.gz archives
...............................

Here we have to visit the download pages of the different dependencies
and download the correct versions (described in `Selecting the correct
versions`_):

1. Fifi:
   https://github.com/steinwurf/fifi/releases
2. Sak:
   https://github.com/steinwurf/sak/releases
3. Boost:
   https://github.com/steinwurf/external-boost-light/releases
4. Waf-tools:
   https://github.com/steinwurf/external-waf-tools/releases
5. Gtest:
   https://github.com/steinwurf/external-gtest/releases
6. Gauge:
   https://github.com/steinwurf/cxx-gauge/releases


Configuring Kodo with manually downloaded dependencies
------------------------------------------------------

After downloading all the dependencies manually we have to inform the
Kodo build scripts to use those instead of trying to automatically
downloading them. This is done using the following command:

::

  python waf configure --bundle=NONE --fifi-path=insert-path-to/fifi --sak-path=insert-path-to/sak/ --boost-path=insert-path-to/external-boost-light/ --waf-tools-path=insert-path-to/external-waf-tools/ --gtest-path=insert-path-to/external-gtest/ --gauge-path=insert-path-to/cxx-gauge/

The bundle options supports a number of different use-cases. The following
will bundle all dependencies but the Fifi library which we have to
manually specify a path for:
::

  python waf configure --bundle=ALL,-fifi --fifi-path=insert-path-to/fifi

Or we may bundle only Fifi:
::

  python waf configure --bundle=NONE,fifi --sak-path=insert-path-to/sak/ --boost-path=insert-path-to/external-boost-light/ --waf-tools-path=insert-path-to/external-waf-tools/ --gtest-path=insert-path-to/external-gtest/ --gauge-path=insert-path-to/cxx-gauge/

More libraries may be added to the ``--bundle=`` option using commas e.g.
bundle all but Fifi and Sak
::

    python waf configure --bundle=ALL,-fifi,-sak --fifi-path=insert-path-to/fifi --sak-path=insert-path-to/sak

The bundle options can be seen by running:
::

  python waf --help


Example application using makefile
-------------------------------------

If you would like to see an example of building an application with
Kodo without using any fancy build-system we provide a small makefile
which shows how to invoke the ``g++`` compiler. The example can be found
in the ``examples/sample_makefile`` folder in the `Kodo repository`_.

.. _`Kodo repository`: https://github.com/steinwurf/kodo

In this case it only requires that you have Fifi, Sak and Boost downloaded.



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





