.. _guide-connecting:
User Documentation
==================
This part of the documentation introduces how Kodo may be use in your
own projects.

Before we start you may want to run the Kodo unit tests, see @todo here how.

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

Download
~~~~~~~~
There are several ways in which you may get the Kodo library and its
dependencies. We recommend downloading the libraries using git, this
will allow you to easily get new updates whenever the libraries are
updated. In addition to this it is also possible to download the
libraries as either zip or tar.gz archives. We will
describe both approaches in the following:

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

3. Clone and download the Boost C++ libraries by running:
  ::
  git clone git://github.com/steinwurf/external-boost.git

 Note, that boost also has its own repositories, if you
 wish you may also use those repositories or another
 Boost way of installing Boost @todo improve.

4. Clone and download the Kodo libraries by running:
  ::
  git clone git://github.com/steinwurf/kodo.git

Download the source code as archives
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Download the zip or tar.gz archives from the following locations and
extract them to a desired location.

1. Fifi can be downloaded from http://github.com/steinwurf/fifi.git
2. Kodo can be downloaded from http://github.com/steinwurf/kodo.git
3. Boost can be downloaded from http://github.com/steinwurf/external-boost.git

Running the initial example
---------------------------

After downloading Kodo and its dependencies we are ready to build the
initial example.

Encoding data
~~~~~~~~~~~~~

We will start this mini tutorial by showing how we may use the Kodo
library to encode data stored in different containers. If you are interested,
you may find a high-level overview of how the encoding process
typically is implemented @todo: here.

Due to computational constraints we often have to limit the amount of data
which can be encoded by a single encoder. Therefore we will typically encode a
large buffer or file by splitting it into several blocks of data.::

 block #      1          2                   n-1         n
          +-------+  +-------+            +-------+  +-------+
          |       |  |       |            |       |  |       |
          |       |  |       |  .......   |       |  |       |
          |       |  |       |            |       |  |       |
          +-------+  +-------+            +-------+  +-------+

Each block then contains a certain amount of data from the buffer or file.
The amount of data contained in a block is given by two parameters namely
the _symbols_ and the _symbol_size_.

The symbol_size denotes the number of bytes in every symbol and also represents
the size of an encoded symbol in bytes. The _symbols_ parameter
specifies the number of symbols contained in a block. A single
block may therefore contain:
::
   bytes in block = symbols * symbol_size

Now lets us see how we may map an encoder to a block of data. In the following
case we store the data to be encoded in a _std::vector<uint8_t>_, but in
practice you may use any container (also raw memory buffers). To accomedation
a wide range of applications Kodo has a flexible memory/storage implementation.
You can read more about how you may adapt it to your specific application
@todo here.


@todo: Janus can I include this from some source code?
::
  #include <kodo/rlnc/full_vector_codes.h>

  // Set the number of symbols (i.e. the generation size in RLNC
  // terminology) and the size of a symbol in bytes
  uint32_t symbols = 42;
  uint32_t symbol_size = 100;

  // Typdefs for the encoder/decoder type we wish to use
  typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;

  // In the following we will make an encoder/decoder factory.
  // The factories are used to build actual encoders/decoders
  rlnc_encoder::factory encoder_factory(symbols, symbol_size);
  rlnc_encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);

  // Allocate some data to encode. In this case we make a buffer with the
  // same size as the encoder's block size (the max. amount a single encoder
  // can encode)
  std::vector<uint8_t> data_in(encoder->block_size(), 'A');

  // Assign the data buffer to the encoder so that we may start
  // to produce encoded symbols from it
  kodo::set_symbols(kodo::storage(data_in), encoder);

In the example above we use a RLNC (Random Linear Network Code) code. You
may get an overview of the different codes and their capabilities on the
code @todo overview page.




covers a certain amount of bytes given

If we have a large amount of data
to encode we will typically create several encoders to encoded smaller chunks.In order to encode a buffer of data we use an encoder we must first decide on


The following code example shows how to encode and decode a block of data
using a RLNC (Random Linear Network Coding). The full code example can be
found in the kodo/examples/encode_decode_simple folder inside the Kodo
project.

::
  #include <kodo/rlnc/full_vector_codes.h>

  // Set the number of symbols (i.e. the generation size in RLNC
  // terminology) and the size of a symbol in bytes
  uint32_t symbols = 42;
  uint32_t symbol_size = 100;

  // Typdefs for the encoder/decoder type we wish to use
  typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;
  typedef kodo::full_rlnc_decoder<fifi::binary8> rlnc_decoder;

  // In the following we will make an encoder/decoder factory.
  // The factories are used to build actual encoders/decoders
  rlnc_encoder::factory encoder_factory(symbols, symbol_size);
  rlnc_encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);

  rlnc_decoder::factory decoder_factory(symbols, symbol_size);
  rlnc_decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);

  // Allocate some storage for a "payload" the payload is what we would
  // eventually send over a network
  std::vector<uint8_t> payload(encoder->payload_size());

  // Allocate some data to encode. In this case we make a buffer with the
  // same size as the encoder's block size (the max. amount a single encoder
  // can encode)
  std::vector<uint8_t> data_in(encoder->block_size());

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

In the following we will discuss the components of the simple example.

Step 1: Create a factory which may be used to build encoders and decoders.
The factories are used to ensure that encoders/decoders are correctly
initialized and ready to use. The parameters to the factories are the
maximum symbols and the maximum symbol size. These

In this case we include the

Kodo architecture overview
--------------------------
In oder to understand how the Kodo API works we will briefly introduce the
design of Kodo here. Understanding this will enable you to optimize the
operations of Kodo towards your specific application. A Kodo encoder and
decoder are assembled by a series of different "layers". These layer are
created using a special techniques know as (Mixin-Layers you may read
further information about this technique in the following papers [x][y]).

Using the layers we may construct either an encoder or decoder which will
look conceptually as shown in the following diagram.

The user calls the encode and decoder function respectively, this is
then processed though the different layers as shown below:
::

                    User calls                   User calls
                        |                            |
                        v                            v
               +--------------------+      +--------------------+
  Layer 1      |   encode(...)      |      |  decode(...)       |
               +--------------------+      +--------------------+
                        |                            |
                        v                            v
               +--------------------+      +--------------------+
  Layer 2      |   encode(...)      |      |  decode(...)       |
               +--------------------+      +--------------------+
                        |                            |
                        v                            v
               +--------------------+      +--------------------+
  Layer 3      |   encode(...)      |      |  decode(...)       |
               +--------------------+      +--------------------+

Typically the initial layer (Layer 1) is a payload_encoder or payload_decoder
layer. The purpose of this layer is to provide a simple encode and decoder
interface.

::
  void payload_encoder::encode(uint8_t *payload);

::
  void payload_decoder::decode(uint8_t *payload);

The payload_encoder / payload_decoder layers purpose is to split the input
payload buffer into a coding symbol and symbol id:

Coding symbol: A coding symbol buffer contains either a buffer into which a new
symbol will be encoded or an already encoded symbol which should be decoded.

Symbol Id: The symbol id is specific to the ECC (Erasure Correction Code) used.
For example in RLNC the symbol id typically will contain the encoding vector,
where as the symbol id for a Reed-Solomon code is an index into the generator
matrix used in the code.

Typically all layers below the Layer 1 payload encoder/decoder will have the
following encode and decode signature:

::
  void xyz_layer::encode(uint8_t *symbol, uint8_t *symbol_id);

::
  void xyz_layer::decode(uint8_t *symbol, uint8_t *symbol_id);

This is however only needed if you are interested in the modifying or adding
a new layer.







From Layer 2 and below tAll other layers use a slightly more complicated interface.

Different layers may provide other APIs so as a developer of new codes
i.e. utilizing different layers than the default the deveoper has to ensure
that the layers are compatible.



