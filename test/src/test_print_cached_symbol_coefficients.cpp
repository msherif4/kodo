// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_has_partial_decoding_tracker.cpp Unit tests for the
///       has_partial_decoding_tracker class

#include <cstdint>
#include <sstream>

#include <gtest/gtest.h>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/rlnc/on_the_fly_codes.hpp>

#include <kodo/debug_linear_block_decoder.hpp>
#include <kodo/has_print_cached_symbol_coefficients.hpp>
#include <kodo/print_cached_symbol_coefficients.hpp>

TEST(TestPrintCachedSymbolCoefficients, print)
{
    typedef fifi::binary field_type;
    typedef kodo::debug_full_rlnc_decoder<field_type> decoder_type;

    uint32_t symbols = 3;
    uint32_t symbol_size = 10;

    decoder_type::factory decoder_factory(symbols, symbol_size);

    auto decoder = decoder_factory.build();

    // Check that this code compiles

    EXPECT_TRUE(kodo::has_print_cached_symbol_coefficients<decoder_type>::value);

    std::stringstream out;

    kodo::print_cached_symbol_coefficients(decoder, out);
    kodo::print_cached_symbol_coefficients(decoder, out);

    // Create a empty symbol and put it in
    std::vector<uint8_t> coefficients(
        decoder->coefficients_size(), '\0');

    std::vector<uint8_t> symbol(
        decoder->symbol_size(), '\0');

    kodo::print_cached_symbol_coefficients(decoder, out);

    // Create a dummy symbol and put it in
    std::fill(coefficients.begin(), coefficients.end(), '\0');
    fifi::set_value<field_type>(&coefficients[0], 1, 1U);
    fifi::set_value<field_type>(&coefficients[0], 2, 1U);

    std::fill(symbol.begin(), symbol.end(), 'a');

    decoder->decode_symbol(&symbol[0], &coefficients[0]);

    kodo::print_cached_symbol_coefficients(decoder, out);

}




