// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_TEST_AUTO_SRC_BASIC_API_TEST_HELPER_HPP
#define KODO_TEST_AUTO_SRC_BASIC_API_TEST_HELPER_HPP

#include <gtest/gtest.h>

#include <fifi/is_prime2325.hpp>
#include <fifi/prime2325_binary_search.hpp>
#include <fifi/prime2325_apply_prefix.hpp>

#include <kodo/generators/random_uniform.hpp>
#include <kodo/systematic_encoder.hpp>

/// @return a random number of symbols to use in the tests
inline uint32_t rand_symbols()
{
    return (rand() % 256) + 1;
}

/// @return a random symbol size to use in the tests
inline uint32_t rand_symbol_size()
{
    return (rand() % 1000) * 4;
}

template<class Encoder, class Decoder>
inline void invoke_basic_api(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    typename Encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    typename Decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);

    EXPECT_TRUE(symbols == encoder_factory.max_symbols());
    EXPECT_TRUE(symbol_size == encoder_factory.max_symbol_size());
    EXPECT_TRUE(symbols == encoder->symbols());
    EXPECT_TRUE(symbol_size == encoder->symbol_size());

    EXPECT_TRUE(symbols == decoder_factory.max_symbols());
    EXPECT_TRUE(symbol_size == decoder_factory.max_symbol_size());
    EXPECT_TRUE(symbols == decoder->symbols());
    EXPECT_TRUE(symbol_size == decoder->symbol_size());

    EXPECT_TRUE(encoder->symbol_length() > 0);
    EXPECT_TRUE(decoder->symbol_length() > 0);

    EXPECT_TRUE(encoder->block_size() == symbols * symbol_size);
    EXPECT_TRUE(decoder->block_size() == symbols * symbol_size);

    EXPECT_TRUE(encoder_factory.max_payload_size() >= encoder->payload_size());
    EXPECT_TRUE(decoder_factory.max_payload_size() >= decoder->payload_size());
    EXPECT_EQ(encoder_factory.max_payload_size(), decoder_factory.max_payload_size());

    // Encode/decode operations
    EXPECT_EQ(encoder->payload_size(), decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in(encoder->block_size(), 'a');

    kodo::random_uniform<uint8_t> fill_data;
    fill_data.generate(&data_in[0], data_in.size());

    // Make sure we have one extreme value (testing optimal prime)
    // Without the prefix mapping decoding will fail (just try :))
    if(data_in.size() > 4)
    {
        data_in[0] = 0xff;
        data_in[1] = 0xff;
        data_in[2] = 0xff;
        data_in[3] = 0xff;
    }

    std::vector<uint8_t> encode_data(data_in);

    // Only used for prime fields, lets reconsider how we implement
    // this less intrusive
    uint32_t prefix = 0;

    if(fifi::is_prime2325<typename Encoder::field_type>::value)
    {
        // This field only works for multiple of uint32_t
        assert((encoder->block_size() % 4) == 0);

        uint32_t block_length = encoder->block_size() / 4;

        fifi::prime2325_binary_search search(block_length);
        prefix = search.find_prefix(sak::storage(encode_data));

        fifi::apply_prefix(sak::storage(encode_data), prefix);
    }

    kodo::set_symbols(kodo::storage(encode_data), encoder);

    // Set the encoder non-systematic
    if(kodo::is_systematic_encoder(encoder))
        kodo::set_systematic_off(encoder);

    while( !decoder->is_complete() )
    {
        uint32_t payload_used = encoder->encode( &payload[0] );
        EXPECT_TRUE(payload_used <= encoder->payload_size());

        decoder->decode( &payload[0] );
    }

    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    kodo::copy_symbols(kodo::storage(data_out), decoder);

    if(fifi::is_prime2325<typename Encoder::field_type>::value)
    {
        // Now we have to apply the prefix to the decoded data
        fifi::apply_prefix(sak::storage(data_out), prefix);
    }

    EXPECT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));
}

template<class Encoder, class Decoder>
inline void invoke_out_of_order_raw(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    typename Encoder::pointer encoder =
        encoder_factory.build(symbols, symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    typename Decoder::pointer decoder =
        decoder_factory.build(symbols, symbol_size);

    // Encode/decode operations
    EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in(encoder->block_size());

    kodo::random_uniform<uint8_t> fill_data;
    fill_data.generate(&data_in[0], data_in.size());

    kodo::set_symbols(kodo::storage(data_in), encoder);

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

            encoder->encode_raw(&payload[0], symbol_id);
            decoder->decode_raw(&payload[0], symbol_id);

        }
    }

    EXPECT_EQ(encoder->block_size(), decoder->block_size());

    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    kodo::copy_symbols(kodo::storage(data_out), decoder);

    EXPECT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));

}

/// Checks that the encoders and decoders are in a clean state after using
/// the initialize function.
template<class Encoder, class Decoder>
inline void invoke_initialize(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    typename Encoder::pointer encoder = encoder_factory.build(symbols,
                                                              symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    typename Decoder::pointer decoder = decoder_factory.build(symbols,
                                                              symbol_size);


    for(uint32_t i = 0; i < 10; ++i)
    {
        encoder->initialize(symbols, symbol_size);
        decoder->initialize(symbols, symbol_size);

        std::vector<uint8_t> payload(encoder->payload_size());

        // Ensure that the we may re-use the encoder also with partial
        // data.
        uint32_t reduce_block = rand() % (encoder->block_size() - 1);
        uint32_t block_size = encoder->block_size() - reduce_block;

        std::vector<uint8_t> data_in(block_size, 'a');

        kodo::random_uniform<uint8_t> fill_data;
        fill_data.generate(&data_in[0], data_in.size());

        kodo::set_symbols(kodo::storage(data_in), encoder);

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
        kodo::copy_symbols(kodo::storage(data_out), decoder);

        EXPECT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));

    }

}



template<class Encoder, class Decoder>
inline void invoke_systematic(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    typename Encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    typename Decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);

    // Encode/decode operations
    EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in(encoder->block_size(), 'a');

    kodo::random_uniform<uint8_t> fill_data;
    fill_data.generate(&data_in[0], data_in.size());

    kodo::set_symbols(kodo::storage(data_in), encoder);

    // Ensure encoder systematic
    EXPECT_TRUE(kodo::is_systematic_encoder(encoder));
    kodo::set_systematic_on(encoder);

    uint32_t pkg_count = 0;

    while( !decoder->is_complete() )
    {
        encoder->encode( &payload[0] );
        decoder->decode( &payload[0] );

        ++pkg_count;
    }

    EXPECT_TRUE(pkg_count == encoder->symbols());

    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    kodo::copy_symbols(kodo::storage(data_out), decoder);

    EXPECT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));

}

#endif

