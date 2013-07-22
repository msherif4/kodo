// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing


/// @file helper_on_the_fly_api.hpp Unit test helper for the on the fly API.
///       on-the-fly referrers to the fact the we progressively will set symbols
///       on the stack

#include <ctime>
#include <gtest/gtest.h>


/// Tests that an encoder support progressively specifying the symbols
template
<
    class Encoder,
    class Decoder
>
inline void test_on_the_fly(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build();

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build();

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in = random_vector(encoder->block_size());

    auto symbol_sequence = sak::split_storage(
        sak::storage(data_in), symbol_size);

    // Set the encoder non-systematic
    if(kodo::is_systematic_encoder(encoder))
        kodo::set_systematic_off(encoder);

    EXPECT_EQ(encoder->rank(), 0U);
    EXPECT_EQ(decoder->rank(), 0U);

    while( !decoder->is_complete() )
    {
        EXPECT_TRUE(encoder->rank() >= decoder->rank());

        encoder->encode( &payload[0] );
        decoder->decode( &payload[0] );

        if(((rand() % 2) == 0) && encoder->rank() < symbols)
        {
            uint32_t i = encoder->rank();
            encoder->set_symbol(i, symbol_sequence[i]);
        }
    }

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
inline void test_on_the_fly(uint32_t symbols, uint32_t symbol_size)
{

    test_on_the_fly<
        Encoder<fifi::binary>,
        Decoder<fifi::binary> >(
        symbols, symbol_size);

    test_on_the_fly<
        Encoder<fifi::binary8>,
        Decoder<fifi::binary8> >(
        symbols, symbol_size);

    test_on_the_fly<
        Encoder<fifi::binary16>,
        Decoder<fifi::binary16> >(
        symbols, symbol_size);

}

template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_on_the_fly()
{

    test_on_the_fly<Encoder, Decoder>(32, 1600);
    test_on_the_fly<Encoder, Decoder>(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_on_the_fly<Encoder, Decoder>(symbols, symbol_size);

}


/// This class tests on-the-fly adding symbols to the encoding while
/// using the systematic feature of the encoder
template<class Encoder, class Decoder>
inline void test_on_the_fly_systematic(uint32_t symbols, uint32_t symbol_size)
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

    auto symbol_sequence = sak::split_storage(
        sak::storage(data_in), symbol_size);

    // Make sure the encoder is systematic
    if(kodo::is_systematic_encoder(encoder))
        kodo::set_systematic_on(encoder);


    while( !decoder->is_complete() )
    {
        encoder->encode( &payload[0] );

        // Simulate some loss
        if((rand() % 2) == 0)
            continue;

        decoder->decode( &payload[0] );

        // set symbol 50% of the time ONLY if rank is not full
        if(((rand() % 2) == 0) && (encoder->rank() < symbols))
        {
            uint32_t i = encoder->rank();
            encoder->set_symbol(i, symbol_sequence[i]);
        }

        EXPECT_TRUE(encoder->rank() >= decoder->rank());
    }

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
inline void test_on_the_fly_systematic(uint32_t symbols, uint32_t symbol_size)
{
    test_on_the_fly_systematic
        <
        Encoder<fifi::binary>,
        Decoder<fifi::binary>
        >(symbols, symbol_size);

    test_on_the_fly_systematic
        <
        Encoder<fifi::binary8>,
        Decoder<fifi::binary8>
        >(symbols, symbol_size);

    test_on_the_fly_systematic
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
inline void test_on_the_fly_systematic()
{
    test_on_the_fly_systematic<Encoder, Decoder>(32, 1600);
    test_on_the_fly_systematic<Encoder, Decoder>(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_on_the_fly_systematic<Encoder, Decoder>(symbols, symbol_size);
}


