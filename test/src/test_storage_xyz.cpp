// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_storage_xyz.cpp Unit tests for storage encoder and decoders

#include <ctime>

#include <gtest/gtest.h>

#include <kodo/storage_decoder.hpp>
#include <kodo/storage_encoder.hpp>
#include <kodo/rfc5052_partitioning_scheme.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/partial_shallow_symbol_storage.hpp>

#include "basic_api_test_helper.hpp"

namespace kodo
{

    /// Decoder stack with shallow storage as required by the
    /// storage decoder.
    template<class Field>
    class shallow_rlnc_decoder
        : public // Payload API
                 payload_recoder<recoding_stack,
                 payload_decoder<
                 // Codec Header API
                 systematic_decoder<
                 symbol_id_decoder<
                 // Symbol ID API
                 plain_symbol_id_reader<
                 // Codec API
                 aligned_coefficients_decoder<
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Storage API
                 mutable_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 shallow_rlnc_decoder<Field>
                     > > > > > > > > > > > > > > >
    { };
}


template<
    class Encoder,
    class Decoder,
    class Partitioning
    >
void invoke_storage(uint32_t max_symbols,
                    uint32_t max_symbol_size,
                    uint32_t object_size)
{

    typedef kodo::storage_encoder<Encoder, Partitioning> storage_encoder;
    typedef kodo::storage_decoder<Decoder, Partitioning> storage_decoder;

    typename storage_encoder::factory encoder_factory(
        max_symbols, max_symbol_size);

    typename storage_decoder::factory decoder_factory(
        max_symbols, max_symbol_size);

    // The storage needed for all decoders
    uint32_t total_block_size = decoder_factory.total_block_size(object_size);

    std::vector<uint8_t> data_out(total_block_size, '\0');
    std::vector<uint8_t> data_in = random_vector(object_size);

    EXPECT_TRUE(object_size <= total_block_size);

    storage_encoder encoder(encoder_factory, sak::storage(data_in));
    storage_decoder decoder(decoder_factory, sak::storage(data_out));

    EXPECT_TRUE(encoder.encoders() == decoder.decoders());

    for(uint32_t i = 0; i < encoder.encoders(); ++i)
    {
        auto e = encoder.build(i);
        auto d = decoder.build(i);

        if(kodo::is_systematic_encoder(e))
            kodo::set_systematic_off(e);

        EXPECT_EQ(e->block_size(), d->block_size());
        EXPECT_EQ(e->bytes_used(), d->bytes_used());
        EXPECT_EQ(e->payload_size(), d->payload_size());

        std::vector<uint8_t> payload(e->payload_size());

        while(!d->is_complete())
        {
            e->encode( &payload[0] );
            d->decode( &payload[0] );

        }
    }

    // Resize the output buffer to contain only the object data
    data_out.resize(object_size);

    EXPECT_TRUE(std::equal(data_in.begin(),
                           data_in.end(),
                           data_out.begin()));
}


void test_storage_coders(uint32_t symbols,
                        uint32_t symbol_size,
                        uint32_t object_size)
{
    invoke_storage<
        kodo::full_rlnc_encoder<fifi::binary>,
        kodo::shallow_rlnc_decoder<fifi::binary>,
        kodo::rfc5052_partitioning_scheme>(
            symbols, symbol_size, object_size);
}
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


TEST(TestStorageCoder, construct_and_invoke_the_basic_api)
{
    test_storage_coders(32, 1600, 2);
    // test_object_coders(1, 1600, 2);

    // srand(static_cast<uint32_t>(time(0)));

    // uint32_t symbols = (rand() % 256) + 1;
    // uint32_t symbol_size = ((rand() % 100) + 1) * 16;

    // // Multiplies the data to be encoded so that the object encoder
    // // is expected to contain multiplier encoders.
    // uint32_t multiplier = (rand() % 10) + 1;

    // test_object_coders(symbols, symbol_size, multiplier);
}











