// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <gtest/gtest.h>

#include <kodo/rlnc/seed_codes.hpp>
#include <kodo_debug/full_vector_decoder_debug.hpp>

#include "basic_api_test_helper.hpp"

static void test_coders(uint32_t symbols, uint32_t symbol_size)
{
    invoke_basic_api
        <
            kodo::seed_rlnc_encoder<fifi::binary>,
            kodo::seed_rlnc_decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_basic_api
        <
            kodo::seed_rlnc_encoder<fifi::binary8>,
            kodo::seed_rlnc_decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_basic_api
        <
            kodo::seed_rlnc_encoder<fifi::binary16>,
            kodo::seed_rlnc_decoder<fifi::binary16>
            >(symbols, symbol_size);
}


TEST(TestRlncSeedCodes, basic_api)
{
    test_coders(32, 1600);
    test_coders(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_coders(symbols, symbol_size);
}


static void test_coders_systematic(uint32_t symbols, uint32_t symbol_size)
{

    invoke_systematic
        <
            kodo::seed_rlnc_encoder<fifi::binary>,
            kodo::seed_rlnc_decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_systematic
        <
            kodo::seed_rlnc_encoder<fifi::binary8>,
            kodo::seed_rlnc_decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_systematic
        <
            kodo::seed_rlnc_encoder<fifi::binary16>,
            kodo::seed_rlnc_decoder<fifi::binary16>
            >(symbols, symbol_size);

}

TEST(TestRlncSeedCodes, systematic)
{
    test_coders_systematic(32, 1600);
    test_coders_systematic(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_coders_systematic(symbols, symbol_size);
}

