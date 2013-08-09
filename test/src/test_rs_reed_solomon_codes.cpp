// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cstdint>
#include <gtest/gtest.h>

#include <kodo/rs/reed_solomon_codes.hpp>

#include "basic_api_test_helper.hpp"


TEST(TestReedSolomonCodes, test_construct)
{

    {
        kodo::rs_encoder<fifi::binary8>::factory encoder_factory(255, 1600);
        encoder_factory.set_symbols(128);
        encoder_factory.set_symbol_size(1600);

        auto encoder = encoder_factory.build();

        kodo::rs_decoder<fifi::binary8>::factory decoder_factory(255, 1600);
        decoder_factory.set_symbols(128);
        decoder_factory.set_symbol_size(1600);

        auto decoder = decoder_factory.build();
    }

}


TEST(TestReedSolomonCodes, test_encode_decode)
{
    {
        kodo::rs_encoder<fifi::binary8>::factory encoder_factory(255, 1600);
        kodo::rs_decoder<fifi::binary8>::factory decoder_factory(255, 1600);

        uint32_t symbols = rand_symbols(255);
        uint32_t symbol_size = rand_symbol_size();

        encoder_factory.set_symbols(symbols);
        encoder_factory.set_symbol_size(symbol_size);

        decoder_factory.set_symbols(symbols);
        decoder_factory.set_symbol_size(symbol_size);

        auto encoder = encoder_factory.build();
        auto decoder = decoder_factory.build();

        // Encode/decode operations
        EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

        std::vector<uint8_t> payload(encoder->payload_size());
        std::vector<uint8_t> data_in = random_vector(encoder->block_size());

        encoder->set_symbols(sak::storage(data_in));

        uint32_t symbol_count = 0;
        while( !decoder->is_complete() )
        {
            encoder->encode( &payload[0] );
            decoder->decode( &payload[0] );
            ++symbol_count;
        }

        // A reed solomon code should be able to decode
        // with exactly k symbols
        EXPECT_EQ(symbol_count, symbols);

        std::vector<uint8_t> data_out(decoder->block_size(), '\0');
        decoder->copy_symbols(sak::storage(data_out));

        EXPECT_TRUE(std::equal(data_out.begin(),
                               data_out.end(),
                               data_in.begin()));

    }

}

