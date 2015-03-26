// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing


/// @file test_rlnc_on_the_fly_codes.cpp Unit tests for the full
///       vector codes (i.e. Network Coding encoders and decoders).

#include <ctime>

#include <gtest/gtest.h>

template<class Encoder, class Decoder>
inline void test_mix_uncoded(uint32_t symbols, uint32_t symbol_size)
{
    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build();

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build();

    // Encode/decode operations
    EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in = random_vector(encoder->block_size());

    encoder->set_symbols(sak::storage(data_in));

    if(kodo::is_systematic_encoder(encoder))
        kodo::set_systematic_off(encoder);

    while( !decoder->is_complete() )
    {

        if((rand() % 100) > 50)
        {
            encoder->encode( &payload[0] );
            decoder->decode( &payload[0] );
        }
        else
        {
            uint32_t symbol_id = rand() % encoder->symbols();

            if(decoder->symbol_pivot(symbol_id))
            {
                continue;
            }

            ASSERT_TRUE(encoder->symbol_size() <= payload.size());

            encoder->copy_symbol(
                symbol_id, sak::storage(payload));

            const uint8_t *symbol_src = encoder->symbol(symbol_id);
            const uint8_t *symbol_dest = &payload[0];

            EXPECT_TRUE(std::equal(symbol_src,
                                   symbol_src + encoder->symbol_size(),
                                   symbol_dest));


            decoder->decode_symbol(&payload[0], symbol_id);

        }
    }

    EXPECT_EQ(encoder->block_size(), decoder->block_size());

    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    decoder->copy_symbols(sak::storage(data_out));

    EXPECT_TRUE(std::equal(data_out.begin(),
                           data_out.end(),
                           data_in.begin()));

}


template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_mix_uncoded(uint32_t symbols, uint32_t symbol_size)
{
    test_mix_uncoded
        <
        Encoder<fifi::binary>,
        Decoder<fifi::binary>
        >(symbols, symbol_size);

    test_mix_uncoded
        <
        Encoder<fifi::binary8>,
        Decoder<fifi::binary8>
        >(symbols, symbol_size);

    test_mix_uncoded
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
inline void test_mix_uncoded()
{
    test_mix_uncoded<Encoder,Decoder>(32, 1600);
    test_mix_uncoded<Encoder,Decoder>(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_mix_uncoded<Encoder, Decoder>(symbols, symbol_size);
}





