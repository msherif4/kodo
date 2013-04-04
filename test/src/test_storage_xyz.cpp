// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_storage_xyz.cpp Unit tests for storage encoder and decoders

#include <ctime>

#include <gtest/gtest.h>

#include <kodo/object_decoder.hpp>
#include <kodo/object_encoder.hpp>
#include <kodo/rfc5052_partitioning_scheme.hpp>

#include "basic_api_test_helper.hpp"

template<
    class Encoder,
    class Decoder,
    class Partitioning
    >
void invoke_object(uint32_t max_symbols,
                   uint32_t max_symbol_size,
                   uint32_t object_size)
{

    typedef kodo::storage_encoder<Encoder, Partitioning> storage_encoder;
    typedef kodo::storage_decoder<Decoder, Partitioning> storage_decoder;

    std::vector<uint8_t> data_in = random_vector(object_size);
    std::vector<uint8_t> data_out(object_size, '\0');

    typename Encoder::factory encoder_factory(max_symbols, max_symbol_size);
    typename Decoder::factory decoder_factory(max_symbols, max_symbol_size);

    storage_encoder storage_encoder(encoder_factory, sak::storage(data_in));
    storage_decoder storage_decoder(decoder_factory, sak::storage(data_out));

    EXPECT_TRUE(obj_encoder.encoders() == obj_decoder.decoders());

    for(uint32_t i = 0; i < obj_encoder.encoders(); ++i)
    {
        auto e = obj_encoder.build(i);
        auto d = obj_decoder.build(i);

        if(kodo::is_systematic_encoder(encoder))
            kodo::set_systematic_off(encoder);

        // Since the storage we encode is a multiple of the
        // block size we always expect that the encoder is
        // fully "filled" with data
        EXPECT_EQ(encoder->block_size(), encoder->bytes_used());
        EXPECT_EQ(decoder->block_size(), decoder->bytes_used());

        EXPECT_EQ(encoder->payload_size(), decoder->payload_size());

        std::vector<uint8_t> payload(encoder->payload_size());

        while(!decoder->is_complete())
        {
            encoder->encode( &payload[0] );
            decoder->decode( &payload[0] );

        }

    }

    EXPECT_TRUE(std::equal(data_in.begin(), data_in.end(), data_out.begin()));

}


// void test_object_coders(uint32_t symbols, uint32_t symbol_size, uint32_t multiplier)
// {
//     invoke_object<
//         kodo::full_rlnc2_encoder,
//         kodo::full_rlnc2_decoder,
//             kodo::rfc5052_partitioning_scheme>(symbols, symbol_size, multiplier);

//     invoke_object<
//         kodo::full_rlnc8_encoder,
//         kodo::full_rlnc8_decoder,
//             kodo::rfc5052_partitioning_scheme>(symbols, symbol_size, multiplier);

//     invoke_object<
//         kodo::full_rlnc16_encoder,
//         kodo::full_rlnc16_decoder,
//             kodo::rfc5052_partitioning_scheme>(symbols, symbol_size, multiplier);

//     invoke_object_partial<
//         kodo::full_rlnc2_encoder,
//         kodo::full_rlnc2_decoder,
//             kodo::rfc5052_partitioning_scheme>(symbols, symbol_size, multiplier);

//     invoke_object_partial<
//         kodo::full_rlnc8_encoder,
//         kodo::full_rlnc8_decoder,
//             kodo::rfc5052_partitioning_scheme>(symbols, symbol_size, multiplier);

//     invoke_object_partial<
//         kodo::full_rlnc16_encoder,
//         kodo::full_rlnc16_decoder,
//             kodo::rfc5052_partitioning_scheme>(symbols, symbol_size, multiplier);

// }


// TEST(TestObjectCoder, construct_and_invoke_the_basic_api)
// {
//     test_object_coders(32, 1600, 2);
//     test_object_coders(1, 1600, 2);

//     srand(static_cast<uint32_t>(time(0)));

//     uint32_t symbols = (rand() % 256) + 1;
//     uint32_t symbol_size = ((rand() % 100) + 1) * 16;

//     // Multiplies the data to be encoded so that the object encoder
//     // is expected to contain multiplier encoders.
//     uint32_t multiplier = (rand() % 10) + 1;

//     test_object_coders(symbols, symbol_size, multiplier);
// }











