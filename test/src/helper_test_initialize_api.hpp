// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing


/// @file test_rlnc_on_the_fly_codes.cpp Unit tests for the full
///       vector codes (i.e. Network Coding encoders and decoders).

#include <ctime>

#include <gtest/gtest.h>

/// Checks that the encoders and decoders are in a clean state after using
/// the initialize function.
template<class Encoder, class Decoder>
inline void test_initialize(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build();

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build();


    for(uint32_t i = 0; i < 3; ++i)
    {
        encoder->initialize(encoder_factory);
        decoder->initialize(decoder_factory);

        std::vector<uint8_t> payload(encoder->payload_size());

        // Ensure that the we may re-use the encoder also with partial
        // data.
        uint32_t block_size = rand_nonzero(encoder->block_size());

        std::vector<uint8_t> data_in = random_vector(block_size);

        encoder->set_symbols(sak::storage(data_in));

        // Set the encoder non-systematic
        if(kodo::is_systematic_encoder(encoder))
            kodo::set_systematic_off(encoder);

        while( !decoder->is_complete() )
        {
            uint32_t payload_used = encoder->encode( &payload[0] );
            EXPECT_TRUE(payload_used <= encoder->payload_size());

            decoder->decode( &payload[0] );
        }

        std::vector<uint8_t> data_out(block_size, '\0');
        decoder->copy_symbols(sak::storage(data_out));

        bool data_equal = sak::equal(sak::storage(data_out),
                                     sak::storage(data_in));

        ASSERT_TRUE(data_equal);

    }

}



template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_initialize(uint32_t symbols, uint32_t symbol_size)
{

    test_initialize
        <
            Encoder<fifi::binary>,
            Decoder<fifi::binary>
            >(symbols, symbol_size);

    test_initialize
        <
            Encoder<fifi::binary8>,
            Decoder<fifi::binary8>
            >(symbols, symbol_size);

    test_initialize
        <
            Encoder<fifi::binary16>,
            Decoder<fifi::binary16>
            >(symbols, symbol_size);
}

template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_initialize()
{
    test_initialize<Encoder, Decoder>(32, 1600);
    test_initialize<Encoder, Decoder>(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_initialize<Encoder, Decoder>(symbols, symbol_size);
}



