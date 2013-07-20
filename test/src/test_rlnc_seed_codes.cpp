// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <gtest/gtest.h>
#include <kodo/rlnc/seed_codes.hpp>

#include "basic_api_test_helper.hpp"

#include "helper_test_reuse_api.hpp"
#include "helper_test_recoding_api.hpp"
#include "helper_test_on_the_fly_api.hpp"
#include "helper_test_basic_api.hpp"
#include "helper_test_initialize_api.hpp"
#include "helper_test_systematic_api.hpp"
#include "helper_test_mix_uncoded_api.hpp"


/// Tests the basic API functionality this mean basic encoding
/// and decoding
TEST(TestSeedCodes, test_basic_api)
{
    test_basic_api<kodo::seed_rlnc_encoder,kodo::seed_rlnc_decoder>();
}

/// Test that the encoders and decoders initialize() function can be used
/// to reset the state of an encoder and decoder and that they therefore
/// can be safely reused.
TEST(TestSeedCodes, test_initialize_api)
{
    test_initialize<kodo::seed_rlnc_encoder,kodo::seed_rlnc_decoder>();
}

/// Tests that an encoder producing systematic packets is handled
/// correctly in the decoder.
TEST(TestSeedCodes, test_systematic_api)
{
    test_systematic<kodo::seed_rlnc_encoder,kodo::seed_rlnc_decoder>();
}

/// Tests whether mixed un-coded and coded packets are correctly handled
/// in the encoder and decoder.
TEST(TestSeedCodes, mix_uncoded_api)
{
    test_mix_uncoded<kodo::seed_rlnc_encoder, kodo::seed_rlnc_decoder>();
}

/// Tests that we can progressively set on symbol at-a-time on
/// encoder
TEST(TestSeedCodes, test_reuse_api)
{
    test_reuse<kodo::seed_rlnc_encoder, kodo::seed_rlnc_decoder>();
}

/// Tests that we can progressively set on symbol at-a-time on
/// encoder
TEST(TestSeedcodes, test_reuse_incomplete_api)
{
    test_reuse_incomplete<kodo::seed_rlnc_encoder, kodo::seed_rlnc_decoder>();
}




