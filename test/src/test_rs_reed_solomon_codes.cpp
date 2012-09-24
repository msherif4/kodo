// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <stdint.h>

#include <boost/make_shared.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>

#include <gtest/gtest.h>

#include <kodo/rs/reed_solomon_codes.hpp>

TEST(TestReedSolomonCodes, test_construct)
{
    // The uniform int distribution
    typedef boost::random::uniform_int_distribution<uint32_t>
            uniform_int;

    // The random generator
    boost::random::mt19937 random_generator;
    random_generator.seed(static_cast<uint32_t>(time(0)));


    {
        kodo::rs8_encoder::factory fenc(255, 1600);
        kodo::rs8_encoder::pointer penc = fenc.build(128, 1600);

        kodo::rs8_decoder::factory fdec(255, 1600);
        kodo::rs8_decoder::pointer pdec = fdec.build(128, 1600);


    }

}


TEST(TestReedSolomonCodes, test_encode_decode)
{

    // The uniform int distribution
    typedef boost::random::uniform_int_distribution<uint32_t>
            uniform_int;

    // The random generator
    boost::random::mt19937 random_generator;
    random_generator.seed(static_cast<uint32_t>(time(0)));


    {
        uniform_int choose_symbols
            = uniform_int(1, fifi::binary8::order - 1);

        uint32_t symbols = choose_symbols(random_generator);

        kodo::rs8_encoder::factory fenc(255, 1600);
        kodo::rs8_encoder::pointer encoder = fenc.build(symbols, 1600);

        kodo::rs8_decoder::factory fdec(255, 1600);
        kodo::rs8_decoder::pointer decoder = fdec.build(symbols, 1600);

        // Encode/decode operations
        EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

        std::vector<uint8_t> payload(encoder->payload_size());
        std::vector<uint8_t> data_in(encoder->block_size(), 'a');

        kodo::random_uniform<uint8_t> fill_data;
        fill_data.generate(&data_in[0], data_in.size());

        kodo::set_symbols(kodo::storage(data_in), encoder);

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
        kodo::copy_symbols(kodo::storage(data_out), decoder);

        EXPECT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));

    }

}

