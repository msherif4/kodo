// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo/rlnc/full_vector_codes.hpp>

/// For an overview of the parameters used see the
/// encode_decode_simple example
///
/// In this example we modify the random generator of an encoder
/// so that it will always generate new random encoding vectors
/// without any reuse. One way to achieve this is to replace the
/// encoding vector generator which we do in the following:

namespace kodo
{
    template<class Field>
    class encoder_test
        : public payload_encoder<
                 systematic_encoder<
                 zero_symbol_encoder<
                 full_vector_encoder<
                 // Here we use the block_uniform_no_position generator
                 // which works by directly calling the random generator
                 linear_block_vector_generator<block_uniform_no_position,
                 linear_block_encoder<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_shallow_partial<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<encoder_test<Field>, Field>
                     > > > > > > > > > >
    {};
}

int main()
{
    uint32_t symbols = 16;
    uint32_t symbol_size = 160;

    typedef kodo::encoder_test<fifi::binary> rlnc_encoder;
    typedef kodo::full_rlnc_decoder<fifi::binary> rlnc_decoder;

    rlnc_encoder::factory encoder_factory(symbols, symbol_size);
    rlnc_decoder::factory decoder_factory(symbols, symbol_size);

    // Counts the symbols needed to decode
    uint32_t symbols_needed = 0;

    for(uint32_t i = 0; i < 5; ++i)
    {
        // Change the seed before we build the encoder
        encoder_factory.set_seed(i);

        auto encoder = encoder_factory.build(symbols, symbol_size);
        auto decoder = decoder_factory.build(symbols, symbol_size);

        // We switch any systematic operations off so we code
        // symbols from the beginning
        if(kodo::is_systematic_encoder(encoder))
            kodo::set_systematic_off(encoder);

        // Allocated the buffers needed
        std::vector<uint8_t> payload(encoder->payload_size());
        std::vector<uint8_t> data_in(encoder->block_size(), 'x');

        // Assign the data buffer to the encoder so that we may start
        // to produce encoded symbols from it
        kodo::set_symbols(kodo::storage(data_in), encoder);

        while( !decoder->is_complete() )
        {
            // Encode a packet into the payload buffer
            encoder->encode( &payload[0] );

            // Pass that packet to the decoder
            decoder->decode( &payload[0] );
            ++symbols_needed;
        }

        std::cout << "Symbols needed to decode " << symbols_needed
                  << " in iteration " << i << std::endl;

        symbols_needed = 0;

        // The decoder is complete, now copy the symbols from the decoder
        std::vector<uint8_t> data_out(decoder->block_size());
        kodo::copy_symbols(kodo::storage(data_out), decoder);

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
}

