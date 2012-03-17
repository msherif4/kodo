// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#include <gtest/gtest.h>
#include <ctime>

#include <kodo/rlnc/full_vector_codes.h>
#include <kodo_debug/full_vector_decoder_debug.h>
#include "basic_api_test_helper.h"


void test_coders(uint32_t symbols, uint32_t symbol_size)
{

    invoke_basic_api
        <
            kodo::full_rlnc_encoder<fifi::binary>,
            kodo::full_rlnc_decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_basic_api
        <
            kodo::full_rlnc_encoder<fifi::binary8>,
            kodo::full_rlnc_decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_basic_api
        <
            kodo::full_rlnc_encoder<fifi::binary16>,
            kodo::full_rlnc_decoder<fifi::binary16>
            >(symbols, symbol_size);
}


TEST(TestRlncFullVectorCodes, basic_api)
{
    test_coders(32, 1600);
    test_coders(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = (rand() % 256) + 1;
    uint32_t symbol_size = ((rand() % 2000) + 1) * 2;

    test_coders(symbols, symbol_size);
}


void test_coders_systematic(uint32_t symbols, uint32_t symbol_size)
{

    invoke_systematic
        <
            kodo::full_rlnc_encoder<fifi::binary>,
            kodo::full_rlnc_decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_systematic
        <
            kodo::full_rlnc_encoder<fifi::binary8>,
            kodo::full_rlnc_decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_systematic
        <
            kodo::full_rlnc_encoder<fifi::binary16>,
            kodo::full_rlnc_decoder<fifi::binary16>
            >(symbols, symbol_size);

}

TEST(TestRlncFullVectorCodes, systematic)
{
    test_coders_systematic(32, 1600);
    test_coders_systematic(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = (rand() % 256) + 1;
    uint32_t symbol_size = ((rand() % 2000) + 1) * 2;

    test_coders_systematic(symbols, symbol_size);
}


void test_coders_raw(uint32_t symbols, uint32_t symbol_size)
{

    invoke_out_of_order_raw
        <
            kodo::full_rlnc_encoder<fifi::binary>,
            kodo::full_rlnc_decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_out_of_order_raw
        <
            kodo::full_rlnc_encoder<fifi::binary8>,
            kodo::full_rlnc_decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_out_of_order_raw
        <
            kodo::full_rlnc_encoder<fifi::binary16>,
            kodo::full_rlnc_decoder<fifi::binary16>
            >(symbols, symbol_size);

}

TEST(TestRlncFullVectorCodes, raw)
{
    test_coders_raw(32, 1600);
    test_coders_raw(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = (rand() % 256) + 1;
    uint32_t symbol_size = ((rand() % 2000) + 1) * 2;

    test_coders_raw(symbols, symbol_size);
}

//
// Testing recoding
//
template<class Encoder, class Decoder>
inline void invoke_recoding(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    typename Encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    typename Decoder::pointer decoder_one = decoder_factory.build(symbols, symbol_size);
    typename Decoder::pointer decoder_two = decoder_factory.build(symbols, symbol_size);

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in(encoder->block_size(), 'a');

    kodo::random_uniform<uint8_t> fill_data;
    fill_data.generate(&data_in[0], data_in.size());

    kodo::set_symbols(kodo::storage(data_in), encoder);

    // Set the encoder non-systematic
    encoder->systematic_off();

    while( !decoder_two->is_complete() )
    {
        encoder->encode( &payload[0] );
        decoder_one->decode( &payload[0] );

        decoder_one->recode( &payload[0] );
        decoder_two->decode( &payload[0] );

    }

    std::vector<uint8_t> data_out_one(decoder_one->block_size(), '\0');
    std::vector<uint8_t> data_out_two(decoder_two->block_size(), '\0');
    kodo::copy_symbols(kodo::storage(data_out_one), decoder_one);
    kodo::copy_symbols(kodo::storage(data_out_two), decoder_two);

    EXPECT_TRUE(std::equal(data_out_one.begin(), data_out_one.end(), data_in.begin()));
    EXPECT_TRUE(std::equal(data_out_two.begin(), data_out_two.end(), data_in.begin()));
}


void test_recoders(uint32_t symbols, uint32_t symbol_size)
{

    invoke_recoding
        <
            kodo::full_rlnc_encoder<fifi::binary>,
            kodo::full_rlnc_decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_recoding
        <
            kodo::full_rlnc_encoder<fifi::binary8>,
            kodo::full_rlnc_decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_recoding
        <
            kodo::full_rlnc_encoder<fifi::binary16>,
            kodo::full_rlnc_decoder<fifi::binary16>
            >(symbols, symbol_size);

}

TEST(TestRlncFullVectorCodes, recoding_simple)
{
    test_recoders(32, 1600);
    test_recoders(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = (rand() % 256) + 1;
    uint32_t symbol_size = ((rand() % 2000) + 1) * 2;

    test_recoders(symbols, symbol_size);
}





