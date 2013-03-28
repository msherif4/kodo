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

#include <kodo/systematic_encoder.hpp>

/// @param max_value The maximum value to return
/// @return a random number between 1 and max_value
inline uint32_t rand_nonzero(uint32_t max_value = 256)
{
    assert(max_value > 0);
    return (rand() % max_value) + 1;
}

/// @param max_symbols The maximum symbols an encoder or decoder should
///        support.
/// @return a random number up to max_symbols to use in the tests
inline uint32_t rand_symbols(uint32_t max_symbols = 256)
{
    return rand_nonzero(max_symbols);
}

/// Returns a random symbol size. The symbol size has to be chosen as a
/// multiple of finite field elements that we use. E.g using a field 2^16
/// the symbol size must be a multiple of two bytes.
/// Currently the biggest field we support is 2^32 so we just always make
/// sure that the symbol size is a multiple of 4 bytes.
///
/// @param max_symbol_size The maximum symbol size in bytes that we support.
/// @return a random symbol size up to max_symbol_size to use in the tests
inline uint32_t rand_symbol_size(uint32_t max_symbol_size = 1000)
{
    assert(max_symbol_size >= 4);

    uint32_t elements = max_symbol_size / 4;

    uint32_t symbol_size = ((rand() % elements) + 1) * 4;
    assert(symbol_size > 0);
    assert((symbol_size % 4) == 0);

    return symbol_size;
}

/// Helper function used extensively in the tests below. This
/// function returns a std::vector<uint8_t> filled with random data.
/// @param size The size of the vector in bytes
inline std::vector<uint8_t> random_vector(uint32_t size)
{
    std::vector<uint8_t> v(size);
    for(uint32_t i = 0; i < v.size(); ++i)
    {
        v[i] = rand() % 256;
    }

    if(size > 4)
    {
        // The 0xffffffff value is illegal in the prime2325 field
        // so we want to make sure we handle it correctly
        v[0] = 0xffU; v[1] = 0xffU;
        v[2] = 0xffU; v[3] = 0xffU;
    }

    return v;
}

/// Helper function with for running tests with different fields.
/// The helper expects a template template class Stack which has an
/// unspecified template argument (the finite field or Field) and a
/// template template class Test which expects the final Stack<Field>
/// type.
/// @param symbols The number of symbols used in the tests
/// @param symbol_size The size of a symbol in bytes used in the tests
template<template <class> class Stack, template <class> class Test>
inline void run_test(uint32_t symbols, uint32_t symbol_size)
{
    {
        Test<Stack<fifi::binary> > test(symbols, symbol_size);
        test.run();
    }

    {
        Test<Stack<fifi::binary8> > test(symbols, symbol_size);
        test.run();
    }

    {
        Test<Stack<fifi::binary16> > test(symbols, symbol_size);
        test.run();
    }
}


template<class Encoder, class Decoder>
inline void
invoke_basic_api(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build(symbols, symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build(symbols, symbol_size);

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

    EXPECT_TRUE(encoder_factory.max_payload_size() >=
                encoder->payload_size());

    EXPECT_TRUE(decoder_factory.max_payload_size() >=
                decoder->payload_size());

    EXPECT_EQ(encoder_factory.max_payload_size(),
              decoder_factory.max_payload_size());

    // Encode/decode operations
    EXPECT_EQ(encoder->payload_size(), decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());

    std::vector<uint8_t> data_in = random_vector(encoder->block_size());
    std::vector<uint8_t> data_in_copy(data_in);

    sak::mutable_storage storage_in = sak::storage(data_in);
    sak::mutable_storage storage_in_copy = sak::storage(data_in_copy);

    EXPECT_TRUE(sak::equal(storage_in, storage_in_copy));

    // Only used for prime fields, lets reconsider how we implement
    // this less intrusive
    uint32_t prefix = 0;

    if(fifi::is_prime2325<typename Encoder::field_type>::value)
    {
        // This field only works for multiple of uint32_t
        assert((encoder->block_size() % 4) == 0);

        uint32_t block_length = encoder->block_size() / 4;

        fifi::prime2325_binary_search search(block_length);
        prefix = search.find_prefix(storage_in_copy);

        // Apply the negated prefix
        fifi::apply_prefix(storage_in_copy, ~prefix);
    }

    encoder->set_symbols(storage_in_copy);

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
    decoder->copy_symbols(sak::storage(data_out));

    if(fifi::is_prime2325<typename Encoder::field_type>::value)
    {
        // Now we have to apply the negated prefix to the decoded data
        fifi::apply_prefix(sak::storage(data_out), ~prefix);
    }

    EXPECT_TRUE(std::equal(data_out.begin(),
                           data_out.end(),
                           data_in.begin()));
}

template<class Encoder, class Decoder>
inline void
invoke_out_of_order_raw(uint32_t symbols, uint32_t symbol_size)
{
    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build(symbols, symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build(symbols, symbol_size);

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

/// Checks that the encoders and decoders are in a clean state after using
/// the initialize function.
template<class Encoder, class Decoder>
inline void
invoke_initialize(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build(symbols,symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build(symbols, symbol_size);


    for(uint32_t i = 0; i < 10; ++i)
    {
        encoder->initialize(symbols, symbol_size);
        decoder->initialize(symbols, symbol_size);

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



template<class Encoder, class Decoder>
inline void
invoke_systematic(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build(symbols, symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build(symbols, symbol_size);

    // Encode/decode operations
    EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in = random_vector(encoder->block_size());

    encoder->set_symbols(sak::storage(data_in));

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
    decoder->copy_symbols(sak::storage(data_out));

    EXPECT_TRUE(std::equal(data_out.begin(),
                           data_out.end(),
                           data_in.begin()));

}

/// Tests that an encoder support progressively specifying the symbols
template<class Encoder, class Decoder>
inline void
invoke_set_symbol(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build(symbols, symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build(symbols, symbol_size);

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
        encoder->encode( &payload[0] );
        decoder->decode( &payload[0] );

        if(encoder->rank() < symbols)
        {
            uint32_t i = rand() % symbols;
            encoder->set_symbol(i, symbol_sequence[i]);
        }
    }

    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    decoder->copy_symbols(sak::storage(data_out));

    EXPECT_TRUE(std::equal(data_out.begin(),
                           data_out.end(),
                           data_in.begin()));
}


#endif

