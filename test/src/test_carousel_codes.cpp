// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <gtest/gtest.h>
#include <ctime>

#include <kodo/nocode/carousel_codes.h>
#include "basic_api_test_helper.h"


static void test_coders(uint32_t symbols, uint32_t symbol_size)
{

    invoke_basic_api
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

    uint32_t symbols = (rand() % 256) + 1;
    uint32_t symbol_size = ((rand() % 2000) + 1) * 2;

    test_coders(symbols, symbol_size);
}








