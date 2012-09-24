// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo/rlnc/full_vector_codes.hpp>

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

