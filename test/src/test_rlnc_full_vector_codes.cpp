// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing


/// @file test_rlnc_full_vector_codes.cpp Unit tests for the full
///       vector codes (i.e. Network Coding encoders and decoders).

#include <ctime>

#include <gtest/gtest.h>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/recoding_symbol_id.hpp>
#include <kodo/proxy_layer.hpp>
#include <kodo/payload_recoder.hpp>
#include <kodo/partial_shallow_symbol_storage.hpp>
#include <kodo/linear_block_decoder_delayed.hpp>
#include <kodo/storage_aware_generator.hpp>
#include <kodo/shallow_symbol_storage.hpp>

#include "basic_api_test_helper.hpp"

namespace kodo
{

    /// Implementation of RLNC decode using the delayed
    /// backwards substitution layer.
    template<class Field>
    class full_rlnc_decoder_delayed
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
                 linear_block_decoder_delayed<
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field Math API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 full_rlnc_decoder_delayed<Field>
                     > > > > > > > > > > > > > > > >
    {};

    /// Implementation of RLNC decode using the delayed
    /// backwards substitution layer.
    template<class Field>
    class full_rlnc_decoder_delayed_shallow
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
                 linear_block_decoder_delayed<
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Storage API
                 mutable_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field Math API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 full_rlnc_decoder_delayed_shallow<Field>
                     > > > > > > > > > > > > > > > >
    {};


    /// Implementation of RLNC encoder using a storage aware generator
    template<class Field>
    class full_rlnc_encoder_storage_aware
        : public // Payload Codec API
                 payload_encoder<
                 // Codec Header API
                 systematic_encoder<
                 symbol_id_encoder<
                 // Symbol ID API
                 plain_symbol_id_writer<
                 // Coefficient Generator API
                 storage_aware_generator<
                 uniform_generator<
                 // Codec API
                 storage_aware_encoder<
                 encode_symbol_tracker<
                 zero_symbol_encoder<
                 linear_block_encoder<
                 // Coefficient Storage API
                 coefficient_info<
                 // Symbol Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 full_rlnc_encoder_storage_aware<Field>
                     > > > > > > > > > > > > > > > > >
    { };

    /// Implementation of RLNC encoder using a storage aware generator
    template<class Field>
    class full_rlnc_encoder_shallow
        : public // Payload Codec API
                 payload_encoder<
                 // Codec Header API
                 systematic_encoder<
                 symbol_id_encoder<
                 // Symbol ID API
                 plain_symbol_id_writer<
                 // Coefficient Generator API
                 storage_aware_generator<
                 uniform_generator<
                 // Codec API
                 storage_aware_encoder<
                 encode_symbol_tracker<
                 zero_symbol_encoder<
                 linear_block_encoder<
                 // Coefficient Storage API
                 coefficient_info<
                 // Symbol Storage API
                 partial_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 full_rlnc_encoder_storage_aware<Field>
                     > > > > > > > > > > > > > > > > >
    { };


}

template<template <class> class Encoder,
         template <class> class Decoder>
void test_coders(uint32_t symbols, uint32_t symbol_size)
{
    invoke_basic_api
        <
            Encoder<fifi::binary>,
            Decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_basic_api
        <
            Encoder<fifi::binary8>,
            Decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_basic_api
        <
            Encoder<fifi::binary16>,
            Decoder<fifi::binary16>
            >(symbols, symbol_size);

    invoke_basic_api
        <
            Encoder<fifi::prime2325>,
            Decoder<fifi::prime2325>
            >(symbols, symbol_size);
}


void test_coders(uint32_t symbols, uint32_t symbol_size)
{

    test_coders<
        kodo::full_rlnc_encoder_shallow,
        kodo::full_rlnc_decoder
        >(symbols, symbol_size);

    test_coders<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder
        >(symbols, symbol_size);

    // The delayed decoders
    test_coders<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed
        >(symbols, symbol_size);
}

/// Tests the basic API functionality this mean basic encoding
/// and decoding
TEST(TestRlncFullVectorCodes, basic_api)
{
    test_coders(32, 1600);
    test_coders(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_coders(symbols, symbol_size);
}

template
    <
    template <class> class Encoder,
    template <class> class Decoder
    >
void test_initialize(uint32_t symbols, uint32_t symbol_size)
{

    invoke_initialize
        <
            Encoder<fifi::binary>,
            Decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_initialize
        <
            Encoder<fifi::binary8>,
            Decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_initialize
        <
            Encoder<fifi::binary16>,
            Decoder<fifi::binary16>
            >(symbols, symbol_size);

    // invoke_initialize
    //     <
    //         Encoder<fifi::prime2325>,
    //         Decoder<fifi::prime2325>
    //         >(symbols, symbol_size);

}

void test_initialize(uint32_t symbols, uint32_t symbol_size)
{

    test_initialize<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder>(symbols, symbol_size);

    // The delayed decoders
    test_initialize<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed>(symbols, symbol_size);

}

/// Test that the encoders and decoders initialize() function can be used
/// to reset the state of an encoder and decoder and that they therefore
/// can be safely reused.
TEST(TestRlncFullVectorCodes, initialize_function)
{
    test_initialize(32, 1600);
    test_initialize(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_initialize(symbols, symbol_size);
}

template<template <class> class Encoder,
         template <class> class Decoder>
void test_coders_systematic(uint32_t symbols, uint32_t symbol_size)
{
    invoke_systematic<
        Encoder<fifi::binary>,
        Decoder<fifi::binary>
        >(symbols, symbol_size);

    invoke_systematic<
        Encoder<fifi::binary8>,
        Decoder<fifi::binary8>
        >(symbols, symbol_size);

    invoke_systematic<
        Encoder<fifi::binary16>,
        Decoder<fifi::binary16>
        >(symbols, symbol_size);

    // invoke_systematic<
    //     Encoder<fifi::prime2325>,
    //     Decoder<fifi::prime2325>
    //     >(symbols, symbol_size);

}


void test_coders_systematic(uint32_t symbols, uint32_t symbol_size)
{
    test_coders_systematic<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder
        >(symbols, symbol_size);


    // The delayed decoders
    test_coders_systematic<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed
        >(symbols, symbol_size);

}

/// Tests that an encoder producing systematic packets is handled
/// correctly in the decoder.
TEST(TestRlncFullVectorCodes, systematic)
{
    test_coders_systematic(32, 1600);
    test_coders_systematic(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_coders_systematic(symbols, symbol_size);
}

template<template <class> class Encoder,
         template <class> class Decoder>
void test_coders_raw(uint32_t symbols, uint32_t symbol_size)
{
    invoke_out_of_order_raw<
        Encoder<fifi::binary>,
        Decoder<fifi::binary>
        >(symbols, symbol_size);

    invoke_out_of_order_raw<
        Encoder<fifi::binary8>,
        Decoder<fifi::binary8>
        >(symbols, symbol_size);

    invoke_out_of_order_raw<
        Encoder<fifi::binary16>,
        Decoder<fifi::binary16>
        >(symbols, symbol_size);

    // invoke_out_of_order_raw<
    //     Encoder<fifi::prime2325>,
    //     Decoder<fifi::prime2325>
    //     >(symbols, symbol_size);

}

void test_coders_raw(uint32_t symbols, uint32_t symbol_size)
{
    test_coders_raw<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder
        >(symbols, symbol_size);

    // The delayed decoders
    test_coders_raw<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed
        >(symbols, symbol_size);
}

/// Tests whether mixed un-coded and coded packets are correctly handled
/// in the encoder and decoder.
TEST(TestRlncFullVectorCodes, raw)
{
    test_coders_raw(32, 1600);
    test_coders_raw(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_coders_raw(symbols, symbol_size);
}

// Small helper structure holding the parameters needed for the
// recoding tests.
struct recoding_parameters
{
    uint32_t m_max_symbols;
    uint32_t m_max_symbol_size;
    uint32_t m_symbols;
    uint32_t m_symbol_size;
};

//
// Testing recoding
//
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
    // buffers to use for the decoding (for simplicity we always do this
    // whether or not it is actually needed)
    std::vector<uint8_t> buffer_decoder_one(decoder_one->block_size(), '\0');
    std::vector<uint8_t> buffer_decoder_two(decoder_two->block_size(), '\0');

    decoder_one->set_symbols(sak::storage(buffer_decoder_one));
    decoder_two->set_symbols(sak::storage(buffer_decoder_two));

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
        encoder->encode( &payload[0] );
        decoder_one->decode( &payload[0] );

        decoder_one->recode( &payload[0] );
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


template
    <
    template <class> class Encoder,
    template <class> class Decoder
    >
void test_recoders(recoding_parameters param)
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

    // invoke_recoding<
    //     Encoder<fifi::prime2325>,
    //     Decoder<fifi::prime2325> >(
    //     symbols, symbol_size);

}

void test_recoders(recoding_parameters param)
{

    test_recoders
        <
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder>(param);

    test_recoders<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed>(param);

    test_recoders<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed_shallow>(param);

}

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
TEST(TestRlncFullVectorCodes, recoding_simple)
{
    recoding_parameters param;
    param.m_max_symbols = 32;
    param.m_max_symbol_size = 1600;
    param.m_symbols = param.m_max_symbols;
    param.m_symbol_size = param.m_max_symbol_size;

    test_recoders(param);

    param.m_max_symbols = 1;
    param.m_max_symbol_size = 1600;
    param.m_symbols = param.m_max_symbols;
    param.m_symbol_size = param.m_max_symbol_size;


    test_recoders(param);

    param.m_max_symbols = 8;
    param.m_max_symbol_size = 8;
    param.m_symbols = param.m_max_symbols;
    param.m_symbol_size = param.m_max_symbol_size;

    test_recoders(param);

    param.m_max_symbols = rand_symbols();
    param.m_max_symbol_size = rand_symbol_size();
    param.m_symbols = rand_symbols(param.m_max_symbols);
    param.m_symbol_size = rand_symbol_size(param.m_max_symbol_size);

    test_recoders(param);
}


template
    <
    template <class> class Encoder,
    template <class> class Decoder
    >
void test_set_symbol(uint32_t symbols, uint32_t symbol_size)
{
    invoke_set_symbol<
        Encoder<fifi::binary>,
        Decoder<fifi::binary> >(
        symbols, symbol_size);

    invoke_set_symbol<
        Encoder<fifi::binary8>,
        Decoder<fifi::binary8> >(
        symbols, symbol_size);

    invoke_set_symbol<
        Encoder<fifi::binary16>,
        Decoder<fifi::binary16> >(
        symbols, symbol_size);

    // invoke_set_symbol<
    //     Encoder<fifi::prime2325>,
    //     Decoder<fifi::prime2325> >(
    //     symbols, symbol_size);

}

void test_set_symbol(uint32_t symbols, uint32_t symbol_size)
{

    test_set_symbol
        <
        kodo::full_rlnc_encoder_storage_aware,
        kodo::full_rlnc_decoder>(symbols, symbol_size);

    test_set_symbol
        <
        kodo::full_rlnc_encoder_storage_aware,
        kodo::full_rlnc_decoder_delayed>(symbols, symbol_size);
}

/// Tests that we can progressively set on symbol at-a-time on
/// encoder
TEST(TestRlncFullVectorCodes, set_symbol)
{
    test_set_symbol(32, 1600);
    test_set_symbol(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_set_symbol(symbols, symbol_size);
}


template<template <class> class Encoder,
         template <class> class Decoder>
void test_reuse(uint32_t symbols, uint32_t symbol_size)
{
    invoke_reuse
        <
            Encoder<fifi::binary>,
            Decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_reuse
        <
            Encoder<fifi::binary8>,
            Decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_reuse
        <
            Encoder<fifi::binary16>,
            Decoder<fifi::binary16>
            >(symbols, symbol_size);

}


void test_reuse(uint32_t symbols, uint32_t symbol_size)
{

    test_reuse<
        kodo::full_rlnc_encoder_shallow,
        kodo::full_rlnc_decoder
        >(symbols, symbol_size);

    test_reuse<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder
        >(symbols, symbol_size);

    // The delayed decoders
    test_reuse<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed
        >(symbols, symbol_size);
}

/// Tests the basic API functionality this mean basic encoding
/// and decoding
TEST(TestRlncFullVectorCodes, test_reuse)
{
    test_reuse(32, 1600);
    test_reuse(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_reuse(symbols, symbol_size);
}



