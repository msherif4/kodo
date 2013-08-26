// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_has_debug_linear_block_decoder.cpp Unit tests for the
///       has_debug_linear_block_decoder class

#include <cstdint>

#include <gtest/gtest.h>

#include <kodo/has_debug_linear_block_decoder.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/rlnc/on_the_fly_codes.hpp>

TEST(TestHasDebugLinearBlockDecoder, detect)
{
    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::full_rlnc_encoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::full_rlnc_encoder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::full_rlnc_decoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::full_rlnc_decoder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::on_the_fly_encoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::on_the_fly_encoder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::on_the_fly_decoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::on_the_fly_decoder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::full_rlnc_encoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_debug_linear_block_decoder<
                     kodo::full_rlnc_encoder<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_debug_linear_block_decoder<
                    kodo::debug_full_rlnc_decoder<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_debug_linear_block_decoder<
                    kodo::debug_full_rlnc_decoder<fifi::binary8> >::value);

}



