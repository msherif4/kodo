// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_debug_linear_block_decoder.cpp Unit tests for the
///       debug_linear_block_decoder functions

#include <cstdint>
#include <sstream>

#include <gtest/gtest.h>

#include <kodo/rlnc/full_vector_codes.hpp>

TEST(TestDebugLinearBlockDecoder, print)
{
    typedef fifi::binary field_type;
    typedef kodo::debug_full_rlnc_decoder<field_type> decoder_type;

    uint32_t symbols = 3;
    uint32_t symbol_size = 10;

    decoder_type::factory decoder_factory(symbols, symbol_size);

    auto decoder = decoder_factory.build();

    // Check that this code compiles
    {
        std::stringstream out;

        decoder->print_decoder_state(out);

        std::string result = "  0 ?:  0 0 0 \n"
                             "  1 ?:  0 0 0 \n"
                             "  2 ?:  0 0 0 \n";

        EXPECT_EQ(result, out.str());
    }


    // Create a empty symbol and put it in
    std::vector<uint8_t> coefficients(
        decoder->coefficients_size(), '\0');

    std::vector<uint8_t> symbol(
        decoder->symbol_size(), '\0');

    decoder->decode_symbol(&symbol[0], &coefficients[0]);

    {
        std::stringstream out;

        decoder->print_decoder_state(out);

        std::string result = "  0 ?:  0 0 0 \n"
                             "  1 ?:  0 0 0 \n"
                             "  2 ?:  0 0 0 \n";

        EXPECT_EQ(result, out.str());
    }

    // Create a dummy symbol and put it in
    std::fill(coefficients.begin(), coefficients.end(), '\0');
    fifi::set_value<field_type>(&coefficients[0], 1, 1U);
    fifi::set_value<field_type>(&coefficients[0], 2, 1U);

    std::fill(symbol.begin(), symbol.end(), 'a');

    decoder->decode_symbol(&symbol[0], &coefficients[0]);

    {
        std::stringstream out;

        decoder->print_decoder_state(out);

        std::string result = "  0 ?:  0 0 0 \n"
                             "  1 C:  0 1 1 \n"
                             "  2 ?:  0 0 0 \n";

        EXPECT_EQ(result, out.str());
    }

    std::fill(symbol.begin(), symbol.end(), 'b');


    decoder->decode_symbol(&symbol[0], 0U);

    {
        std::stringstream out;

        decoder->print_decoder_state(out);

        std::string result = "  0 U:  1 0 0 \n"
                             "  1 C:  0 1 1 \n"
                             "  2 ?:  0 0 0 \n";

        EXPECT_EQ(result, out.str());
    }
}




