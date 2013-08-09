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
#include <kodo/has_shallow_symbol_storage.hpp>

#include "basic_api_test_helper.hpp"

#include "helper_test_reuse_api.hpp"
#include "helper_test_recoding_api.hpp"
#include "helper_test_on_the_fly_api.hpp"
#include "helper_test_basic_api.hpp"
#include "helper_test_initialize_api.hpp"
#include "helper_test_systematic_api.hpp"
#include "helper_test_mix_uncoded_api.hpp"

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
                 encode_symbol_tracker<
                 zero_symbol_encoder<
                 linear_block_encoder<
                 storage_aware_encoder<
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
                 full_rlnc_encoder_shallow<Field>
                     > > > > > > > > > > > > > > > > >
    { };


}


/// Tests the basic API functionality this mean basic encoding
/// and decoding
TEST(TestRlncFullVectorCodes, test_basic_api)
{
    test_basic_api<kodo::full_rlnc_encoder_shallow, kodo::full_rlnc_decoder>();
    test_basic_api<kodo::full_rlnc_encoder, kodo::full_rlnc_decoder>();

    // The delayed decoders
    test_basic_api<kodo::full_rlnc_encoder, kodo::full_rlnc_decoder_delayed>();
}



/// Test that the encoders and decoders initialize() function can be used
/// to reset the state of an encoder and decoder and that they therefore
/// can be safely reused.
TEST(TestRlncFullVectorCodes, test_initialize)
{
    test_initialize<kodo::full_rlnc_encoder_shallow, kodo::full_rlnc_decoder>();
    test_initialize<kodo::full_rlnc_encoder, kodo::full_rlnc_decoder>();

    // The delayed decoders
    test_initialize<kodo::full_rlnc_encoder, kodo::full_rlnc_decoder_delayed>();

}

/// Tests that an encoder producing systematic packets is handled
/// correctly in the decoder.
TEST(TestRlncFullVectorCodes, test_systematic)
{
    test_systematic<kodo::full_rlnc_encoder, kodo::full_rlnc_decoder>();
    test_systematic<kodo::full_rlnc_encoder, kodo::full_rlnc_decoder_delayed>();
}

/// Tests whether mixed un-coded and coded packets are correctly handled
/// in the encoder and decoder.
TEST(TestRlncFullVectorCodes, mix_uncoded)
{
    test_mix_uncoded<kodo::full_rlnc_encoder,kodo::full_rlnc_decoder>();
    test_mix_uncoded<kodo::full_rlnc_encoder,kodo::full_rlnc_decoder_delayed>();
}

/// The recoding
TEST(TestRlncFullVectorCodes, test_recoders_api)
{

    test_recoders<kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder>();

    test_recoders<kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed>();

    test_recoders<kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed_shallow>();

}

/// Tests the basic API functionality this mean basic encoding
/// and decoding
TEST(TestRlncFullVectorCodes, test_reuse_api)
{
    test_reuse<
        kodo::full_rlnc_encoder_shallow,
        kodo::full_rlnc_decoder>();

    test_reuse<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder>();

    // The delayed decoders
    test_reuse<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed>();
}

/// Tests the basic API functionality this mean basic encoding
/// and decoding
TEST(TestRlncFullVectorCodes, test_reuse_incomplete_api)
{
    test_reuse_incomplete<
        kodo::full_rlnc_encoder_shallow,
        kodo::full_rlnc_decoder>();

    test_reuse_incomplete<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder>();

    // The delayed decoders
    test_reuse_incomplete<
        kodo::full_rlnc_encoder,
        kodo::full_rlnc_decoder_delayed>();
}



