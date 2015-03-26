// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_has_debug_cached_symbol_decoder.cpp Unit tests for the
///       has_debug_cached_symbol_decoder class

#include <cstdint>

#include <gtest/gtest.h>

#include <kodo/has_print_cached_symbol_coefficients.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/rlnc/on_the_fly_codes.hpp>

TEST(TestHasPrintCachedSymbolCoefficients, detect)
{
    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::full_rlnc_encoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::full_rlnc_encoder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::full_rlnc_decoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::full_rlnc_decoder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::on_the_fly_encoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::on_the_fly_encoder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::on_the_fly_decoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::on_the_fly_decoder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::full_rlnc_encoder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_print_cached_symbol_coefficients<
                     kodo::full_rlnc_encoder<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_print_cached_symbol_coefficients<
                    kodo::debug_full_rlnc_decoder<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_print_cached_symbol_coefficients<
                    kodo::debug_full_rlnc_decoder<fifi::binary8> >::value);

}




