Overview
========

.. _overview:

In this document we will try to give a quick overview of Kodo to new users.

Features
--------

Kodo provides several different codes, primarily the basic Random Linear
Network Code and multiple variants.

Random Linear Network Coding (RLNC)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
One of the parameters we may tweak using Kodo is how the encoding vectors
are constructed and represented. The encoding vectors describe which symbols
are combined within a generation (a generation consists of a number of
data packets also refereed to as symbols).

Kodo allows the density / distribution of the encoding vectors to be change.
Some of the supported variants are:

* Dense variant (purely uniform).
* Sparse variant with fixed density
* Sparse variant with uniform density
* Systematic variants

The representation of the encoding vector affects the overhead on the wire
i.e. the number of bytes per coded packet containing meta data instead of
application data. Here some of the variants supported are:

* Variants where the coding vector is included (for recoding)
* Variants where a seed is included

Other codes and approaches
~~~~~~~~~~~~~~~~~~~~~~~~~~
We also support some other coding types:

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
  Kodo project in the overview on the main page.

The buildbot pages will also provide you with up-to-date information on which
platforms and compilers we are currently testing. If you have a specific
platform or compiler which you would like to see Kodo support, `drop us a line`_.

.. _drop us a line: http://steinwurf.com/contact-us/




