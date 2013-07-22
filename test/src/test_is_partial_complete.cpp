// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_has_partial_decoding_tracker.cpp Unit tests for the
///       has_partial_decoding_tracker class

#include <cstdint>

#include <gtest/gtest.h>

#include <kodo/is_partial_complete.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/rlnc/on_the_fly_codes.hpp>

#include "basic_api_test_helper.hpp"

#include <kodo/debug_linear_block_decoder.hpp>

namespace kodo
{
        template<class Field>
    class dbg_on_the_fly_decoder :
        public // Payload API
               payload_recoder<recoding_stack,
               payload_decoder<
               // Codec Header API
               systematic_decoder<
               symbol_id_decoder<
               // Symbol ID API
               plain_symbol_id_reader<
               // Codec API
               debug_linear_block_decoder<
               aligned_coefficients_decoder<
               partial_decoding_tracker<
               linear_block_decoder<
               // Coefficient Storage API
               coefficient_storage<
               coefficient_info<
               // Storage API
               deep_symbol_storage<
               storage_bytes_used<
               storage_block_info<
               // Finite Field API
               finite_field_math<typename fifi::default_field<Field>::type,
               finite_field_info<Field,
               // Factory API
               final_coder_factory_pool<
               // Final type
               dbg_on_the_fly_decoder<Field>
                   > > > > > > > > > > > > > > > > >
    { };
}

TEST(TestIsPartialComplete, check_false)
{
    typedef fifi::binary field_type;
    typedef kodo::full_rlnc_encoder<field_type> encoder_type;
    typedef kodo::full_rlnc_decoder<field_type> decoder_type;

    uint32_t symbols = 16;
    uint32_t symbol_size = 100;

    encoder_type::factory encoder_factory(symbols, symbol_size);
    decoder_type::factory decoder_factory(symbols, symbol_size);

    auto encoder = encoder_factory.build();
    auto decoder = decoder_factory.build();

    // Check that this code compiles

    EXPECT_FALSE(kodo::has_partial_decoding_tracker<encoder_type>::value);
    if(kodo::has_partial_decoding_tracker<encoder_type>::value)
    {
        EXPECT_FALSE(kodo::is_partial_complete(encoder));
    }

    EXPECT_FALSE(kodo::has_partial_decoding_tracker<decoder_type>::value);
    if(kodo::has_partial_decoding_tracker<decoder_type>::value)
    {
        EXPECT_FALSE(kodo::is_partial_complete(decoder));
    }
}


TEST(TestIsPartialComplete, check_true)
{
    typedef fifi::binary field_type;
    typedef kodo::on_the_fly_encoder<field_type> encoder_type;
    typedef kodo::dbg_on_the_fly_decoder<field_type> decoder_type;

    uint32_t symbols = 16;
    uint32_t symbol_size = 100;

    encoder_type::factory encoder_factory(symbols, symbol_size);
    decoder_type::factory decoder_factory(symbols, symbol_size);

    auto encoder = encoder_factory.build();
    auto decoder = decoder_factory.build();

    // Check that this code compiles

    EXPECT_FALSE(kodo::has_partial_decoding_tracker<encoder_type>::value);
    if(kodo::has_partial_decoding_tracker<encoder_type>::value)
    {
        EXPECT_FALSE(kodo::is_partial_complete(encoder));
    }

    EXPECT_TRUE(kodo::has_partial_decoding_tracker<decoder_type>::value);
    if(kodo::has_partial_decoding_tracker<decoder_type>::value)
    {
        EXPECT_FALSE(kodo::is_partial_complete(decoder));
    }

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in = random_vector(encoder->block_size());

    auto symbol_sequence = sak::split_storage(
        sak::storage(data_in), symbol_size);


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

        // Check that the call works
        bool ok = kodo::is_partial_complete(decoder);
        (void)ok;
    }

    EXPECT_TRUE(kodo::is_partial_complete(decoder));

    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    decoder->copy_symbols(sak::storage(data_out));


    EXPECT_TRUE(std::equal(data_out.begin(),
                           data_out.end(),
                           data_in.begin()));



}



