// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#include <gtest/gtest.h>
#include <ctime>

#include <kodo/rlnc/seed_codes.h>
#include <kodo_debug/full_vector_decoder_debug.h>
#include "basic_api_test_helper.h"


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

    uint32_t symbols = (rand() % 256) + 1;
    uint32_t symbol_size = ((rand() % 2000) + 1) * 2;

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

    uint32_t symbols = (rand() % 256) + 1;
    uint32_t symbol_size = ((rand() % 2000) + 1) * 2;

    test_coders_systematic(symbols, symbol_size);
}





