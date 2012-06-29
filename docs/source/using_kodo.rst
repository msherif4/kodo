.. _using_kodo:
Using Kodo in Your Application
==============================
The purpose of this page is to describe how you my
include Kodo in your own projects.

Dependencies of Kodo
--------------------
Kodo relies on two external libraries which you must download in order
to start using Kodo in your application. The functionality used from both these
libraries are header-only which means that you only have to specify the
correct includes paths to use them. The two libraries are:

1. Fifi (http://github.com/steinwurf/fifi), this library contains
   finite field arithmetics used in ECC (Error Correcting Code) algorithms.
2. Boost C++ libraries (http://boost.org), this library contains wide range
   of C++ utilities. We use only a subset of these functionalities such as
   smart-pointers.

Note, that in future version we plan to switch to C++11 which most likely will
mean that the Boost dependency is no longer needed. You can help with this
transition see @todo "how to contribute" for more information.



Example using Waf
-----------------

Example using makefile / command-line
-------------------------------------

