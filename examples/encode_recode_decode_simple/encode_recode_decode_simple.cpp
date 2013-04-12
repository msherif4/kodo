// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @example encode_recode_decode_simple.cpp
///
/// In Network Coding applications one of the key features is the
/// ability of intermediate nodes in the network to recode packets
/// as they traverse them. In Kodo it is possible to recode packets
/// in decoders which provide the recode() function.
///
/// This example shows how to use one encoder and two decoders to
/// simulate a simple relay network as shown below (for simplicity
/// we have error free links, i.e. no data packets are lost when being
/// sent from encoder to decoder_1 and decoder_1 to decoder_2):
///
///         +-----------+      +-----------+      +------------+
///         |  encoder  |+---->| decoder_1 |+---->|  decoder_2 |
///         +-----------+      | (recoder) |      +------------+
///                            +-----------+
///
/// In a practical application recoding can be using in several different
/// ways and one must consider several different factors e.g. such as
/// reducing linear dependency by coordinating several recoding nodes
/// in the network.
/// Suggestions for dealing with such issues can be found in current
/// research litterature (e.g. MORE: A Network Coding Approach to
/// Opportunistic Routing).

#include <kodo/rlnc/full_vector_codes.hpp>

int main()
{
    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    uint32_t symbols = 42;
    uint32_t symbol_size = 160;

    // Typdefs for the encoder/decoder type we wish to use
    typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;
    typedef kodo::full_rlnc_decoder<fifi::binary8> rlnc_decoder;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build();

    rlnc_decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder_1 = decoder_factory.build();
    auto decoder_2 = decoder_factory.build();

    // Allocate some storage for a "payload" the payload is what we would
    // eventually send over a network
    std::vector<uint8_t> payload(encoder->payload_size());

    // Allocate some data to encode. In this case we make a buffer
    // with the same size as the encoder's block size (the max.
    // amount a single encoder can encode)
    std::vector<uint8_t> data_in(encoder->block_size());

    // Just for fun - fill the data with random data
    for(auto &e: data_in)
        e = rand() % 256;

    // Assign the data buffer to the encoder so that we may start
    // to produce encoded symbols from it
    encoder->set_symbols(sak::storage(data_in));

    while( !decoder_2->is_complete() )
    {
        // Encode a packet into the payload buffer
        encoder->encode( &payload[0] );

        // Pass that packet to decoder_1
        decoder_1->decode( &payload[0] );

        // Now produce a new recoded packet from the current
        // decoding buffer, and place it into the payload buffer
        decoder_1->recode( &payload[0] );

        // Pass the recoded packet to decoder_2
        decoder_2->decode( &payload[0] );
    }

    // Both decoder_1 and decoder_2 should now be complete,
    // copy the symbols from the decoders
    std::vector<uint8_t> data_out_1(decoder_1->block_size());
    std::vector<uint8_t> data_out_2(decoder_2->block_size());

    decoder_1->copy_symbols(sak::storage(data_out_1));
    decoder_2->copy_symbols(sak::storage(data_out_2));

    // Check we properly decoded the data
    if (std::equal(data_out_1.begin(), data_out_1.end(), data_in.begin()) &&
        std::equal(data_out_2.begin(), data_out_2.end(), data_in.begin()))
    {
        std::cout << "Data decoded correctly" << std::endl;
    }
    else
    {
        std::cout << "Unexpected failure to decode "
                  << "please file a bug report :)" << std::endl;
    }
}

