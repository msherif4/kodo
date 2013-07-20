// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_carousel_codes.cpp Unit tests for the carousel nocode scheme

#include <ctime>

#include <gtest/gtest.h>

#include <kodo/nocode/carousel_codes.hpp>

#include "helper_test_basic_api.hpp"

static void test_coders(uint32_t symbols, uint32_t symbol_size)
{

    test_basic_api
        <
            kodo::nocode_carousel_encoder,
            kodo::nocode_carousel_decoder
        >(symbols, symbol_size);
}


TEST(TestNoCodeCarouselCodes, basic_api)
{
    test_coders(32, 1600);
    test_coders(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_coders(symbols, symbol_size);
}


