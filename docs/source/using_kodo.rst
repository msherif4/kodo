.. _using_kodo:
Using Kodo in Your Application
==============================
The purpose of this page is to describe how you may
include Kodo in your own projects.

Dependencies of Kodo
--------------------
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


Note, that in future version we plan to switch to C++11 which most likely will
mean that the Boost dependency is no longer needed. You can help with this
transition see @todo "how to contribute" for more information.

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
which shows how to invoke the ``g++`` compiler. The following may also
be found in the ``examples/sample_makefile`` folder in the Kodo sources.

To try it out either use the code provide there or save the following
code in a file called ``main.cpp``.
::
  #include <fifi/fifi_utils.h>
  #include <kodo/rlnc/full_vector_codes.h>

  int main()
  {
    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    uint32_t symbols = 42;
    uint32_t symbol_size = 100;

    typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;
    typedef kodo::full_rlnc_decoder<fifi::binary8> rlnc_decoder;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_encoder::factory encoder_factory(symbols, symbol_size);
    rlnc_encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);

    rlnc_decoder::factory decoder_factory(symbols, symbol_size);
    rlnc_decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in(encoder->block_size());

    // Just for fun - fill the data with random data
    kodo::random_uniform<uint8_t> fill_data;
    fill_data.generate(&data_in[0], data_in.size());

    // Assign the data buffer to the encoder so that we may start
    // to produce encoded symbols from it
    kodo::set_symbols(kodo::storage(data_in), encoder);

    while( !decoder->is_complete() )
    {
        // Encode a packet into the payload buffer
        encoder->encode( &payload[0] );

        // Pass that packet to the decoder
        decoder->decode( &payload[0] );
    }
  }

Using ``g++`` the example code may be compiled as::
::
  g++ main.cpp -o example -I ~/dev/steinwurf/kodo/ -I ~/dev/steinwurf/external-boost/boost -I ~/dev/steinwurf/fifi -I ~/dev/steinwurf/sak

As you can see we simply have to provide the include path directives.
You may run the application by typing:
::
  ./example


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

This example is available at:

*




