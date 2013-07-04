.. _c_bindings:

Kodo C bindings
===============

This repository C high-level bindings for the Kodo Network Coding library.
The bindings provide access to basic functionality provided by Kodo, such
as encoding and decoding of data. The examples folder provides sample
applications showing usage of the C API.

Getting started
---------------
The source code for the Kodo C bindings are available at our github.com
repository:

* https://github.com/steinwurf/kodo-c-bindings

To obtain the source code you can either clone the repository with
git or download a released version.

To build the library you need the tools described on the Kodo
:ref:`getting_started` page.

Building
--------
To use the Kodo C bindings you first have to fetch the necessary
dependencies. There are two ways of getting the dependencies:

1. Allowing the build scripts to automatically download the
   dependencies via git (this requires an Internet connection).
2. Downloading the dependencies manually.

For option 1 you may proceed directly to the `Build (automatically
downloading dependencies)`_ section which will do the automatic download.

For option 2 we have to first download the dependencies needed described in
the `Build (manually downloaded dependencies)`_ section.


Build (manually downloaded dependencies)
........................................
The dependencies required to build the Kodo C bindings are Kodo itself +
the dependencies of Kodo. Kodo's dependencies are specified on the
:ref:`kodo-dependencies` section. On that page you will also
find the description of how to build Kodo with manually downloaded
dependencies. This procedure is the same for the Kodo C-bindings.

Be sure to read the :ref:`selecting-the-correct-versions` section to see
which versions you need to download (here you should use the ``wscript``
in the kodo-c-bindings repository to obtain the version numbers).

Following the instructions on the :ref:`using-kodo-in-your-application` page
you should have downloaded the following dependencies: Fifi, Sak, Boost,
Waf-tools, Gtest, Gauge and Kodo.

To configure the project run:
::

  python waf configure --bundle=NONE --kodo-path=insert-path-to/kodo --fifi-path=insert-path-to/fifi --sak-path=insert-path-to/sak/ --boost-path=insert-path-to/external-boost-light/ --waf-tools-path=insert-path-to/external-waf-tools/ --gtest-path=insert-path-to/external-gtest/ --gauge-path=insert-path-to/cxx-gauge/

After configure run the following command to build the static library:
::

  python waf build

Build (automatically downloading dependencies)
..............................................
If you wish to let the waf build scripts automatically download the
source code you can use the steps outline here:

To configure the project run:
::

  python waf configure --bundle=ALL

This will configure the project and download all the dependencies needed
(the dependencies will be located in a folder called bundle_depencies) in
the folder where you run the command.

After configure run the following command to build the static library:
::

  python waf build


Linking with an application
---------------------------
Running the ``waf build`` step will produce a static library in
the ``build`` folder called ``libckodo.a`` on Linux and Mac and
``ckodo.lib`` on Windows. The following section will show you how to
link with the library in your application. To do this you need to
include ``ckodo.h`` in your application and link with ``libckodo``.
Here is a typical gcc link command:

::

  gcc myapp.c -o myapp -Ipath_to_ckodo_h -Wl,-Bstatic -Lpath_to_libkodo_a -lckodo -Wl,-Bdynamic -lstdc++

Substitute the ``path_to_ckodo_h`` with the path of ``ckodo.h`` similarly
Substitute the ``path_to_libkodo_a`` with the path of the ``libkodo.a``
library.


