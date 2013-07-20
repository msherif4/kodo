// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

/// @file helper_recoding_api.hpp Unit test helper for testing that recoding
///       works

#include <ctime>
#include <gtest/gtest.h>

#include <kodo/has_shallow_symbol_storage.hpp>

/// Small helper structure holding the parameters needed for the
/// recoding tests.
struct recoding_parameters
{
    uint32_t m_max_symbols;
    uint32_t m_max_symbol_size;
    uint32_t m_symbols;
    uint32_t m_symbol_size;
};

/// Tests that the recoding function works, this is done by using one
/// encoder
/// and two decoders:
///
///    +------------+      +------------+      +------------+
///    | encoder    |+---->| decoder #1 |+---->| decoder #2 |
///    +------------+      +------------+      +------------+
///
/// Where the encoder passes data to the first decoder which then
/// recodes and passes data to the second decoder
///
/// @param param The recoding parameters to use
template<class Encoder, class Decoder>
inline void invoke_recoding(recoding_parameters param)
{
    // Common setting
    typename Encoder::factory encoder_factory(
        param.m_max_symbols, param.m_max_symbol_size);

    encoder_factory.set_symbols(param.m_symbols);
    encoder_factory.set_symbol_size(param.m_symbol_size);

    auto encoder = encoder_factory.build();

    typename Decoder::factory decoder_factory(
        param.m_max_symbols, param.m_max_symbol_size);

    decoder_factory.set_symbols(param.m_symbols);
    decoder_factory.set_symbol_size(param.m_symbol_size);

    auto decoder_one = decoder_factory.build();
    auto decoder_two = decoder_factory.build();

    // If tested with a shallow decoder we have to remember to set the
    // buffers to use for the decoding
    std::vector<uint8_t> buffer_decoder_one(decoder_one->block_size(), '\0');
    std::vector<uint8_t> buffer_decoder_two(decoder_two->block_size(), '\0');

    if(kodo::has_shallow_symbol_storage<Decoder>::value)
    {
        decoder_one->set_symbols(sak::storage(buffer_decoder_one));
        decoder_two->set_symbols(sak::storage(buffer_decoder_two));
    }

    EXPECT_EQ(encoder->payload_size(), decoder_one->payload_size());
    EXPECT_EQ(encoder->payload_size(), decoder_two->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in = random_vector(encoder->block_size());

    encoder->set_symbols(sak::storage(data_in));

    // Set the encoder non-systematic
    if(kodo::is_systematic_encoder(encoder))
        kodo::set_systematic_off(encoder);

    while( !decoder_two->is_complete() )
    {
        uint32_t encode_size = encoder->encode( &payload[0] );
        EXPECT_TRUE(encode_size <= payload.size());
        EXPECT_TRUE(encode_size > 0);

        decoder_one->decode( &payload[0] );

        uint32_t recode_size = decoder_one->recode( &payload[0] );
        EXPECT_TRUE(recode_size <= payload.size());
        EXPECT_TRUE(recode_size > 0);

        decoder_two->decode( &payload[0] );
    }

    std::vector<uint8_t> data_out_one(decoder_one->block_size(), '\0');
    std::vector<uint8_t> data_out_two(decoder_two->block_size(), '\0');

    decoder_one->copy_symbols(sak::storage(data_out_one));
    decoder_two->copy_symbols(sak::storage(data_out_two));

    EXPECT_TRUE(std::equal(data_out_one.begin(),
                           data_out_one.end(),
                           data_in.begin()));

    EXPECT_TRUE(std::equal(data_out_two.begin(),
                           data_out_two.end(),
                           data_in.begin()));
}

/// Invokes the recoding API for the Encoder and Decoder with
/// typical field sizes
/// @param param The recoding parameters
template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_recoders(recoding_parameters param)
{
    invoke_recoding<
        Encoder<fifi::binary>,
        Decoder<fifi::binary> >(param);

    invoke_recoding<
        Encoder<fifi::binary8>,
        Decoder<fifi::binary8> >(param);

    invoke_recoding<
        Encoder<fifi::binary16>,
        Decoder<fifi::binary16> >(param);
}


/// Invokes the recoding API for the Encoder and Decoder with
/// typical field sizes
/// @param param The recoding parameters
template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_recoders()
{
    recoding_parameters param;
    param.m_max_symbols = 32;
    param.m_max_symbol_size = 1600;
    param.m_symbols = param.m_max_symbols;
    param.m_symbol_size = param.m_max_symbol_size;

    test_recoders<Encoder,Decoder>(param);

    param.m_max_symbols = 1;
    param.m_max_symbol_size = 1600;
    param.m_symbols = param.m_max_symbols;
    param.m_symbol_size = param.m_max_symbol_size;

    test_recoders<Encoder,Decoder>(param);

    param.m_max_symbols = 8;
    param.m_max_symbol_size = 8;
    param.m_symbols = param.m_max_symbols;
    param.m_symbol_size = param.m_max_symbol_size;

    test_recoders<Encoder,Decoder>(param);

    param.m_max_symbols = rand_symbols();
    param.m_max_symbol_size = rand_symbol_size();
    param.m_symbols = rand_symbols(param.m_max_symbols);
    param.m_symbol_size = rand_symbol_size(param.m_max_symbol_size);

    test_recoders<Encoder,Decoder>(param);
}

