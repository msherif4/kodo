// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <kodo/rlnc/full_vector_codes.hpp>

#include <kodo/cached_symbol_decoder.hpp>
#include <kodo/debug_cached_symbol_decoder.hpp>
#include <kodo/debug_linear_block_decoder.hpp>

/// @example use_debug_layers.cpp
///
/// Simple example showing how to use some of the debug layers defined
/// in Kodo.

int main()
{
    // Seed rand
    srand(time(0));

    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    uint32_t symbols = 8;
    uint32_t symbol_size = 16;

    // Typdefs for the encoder/decoder type we wish to use
    typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;
    typedef kodo::debug_full_rlnc_decoder<fifi::binary8> rlnc_decoder;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build();

    rlnc_decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build();

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

    while( !decoder->is_complete() )
    {
        // Encode a packet into the payload buffer
        encoder->encode( &payload[0] );

        // Here we "simulate" a packet loss of approximately 50%
        // by dropping half of the encoded packets.
        // When running this example you will notice that the initial
        // symbols are received systematically (i.e. uncoded). After
        // sending all symbols once uncoded, the encoder will switch
        // to full coding, in which case you will see the full encoding
        // vectors being sent and received.
        if((rand() % 2) == 0)
            continue;

        // Pass that packet to the decoder
        decoder->decode( &payload[0] );

        std::cout << "Received symbol:" << std::endl;
        decoder->print_cached_symbol_coefficients(std::cout);

        std::cout << "Decoder state:" << std::endl;
        decoder->print_decoder_state(std::cout);

        std::cout << "Decoder rank:" << decoder->rank() << std::endl;
    }

    // The decoder is complete, now copy the symbols from the decoder
    std::vector<uint8_t> data_out(decoder->block_size());
    decoder->copy_symbols(sak::storage(data_out));

    // Check we properly decoded the data
    if (std::equal(data_out.begin(), data_out.end(), data_in.begin()))
    {
        std::cout << "Data decoded correctly" << std::endl;
    }
    else
    {
        std::cout << "Unexpected failure to decode "
                  << "please file a bug report :)" << std::endl;
    }
}

