// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <stdint.h>

#include <gtest/gtest.h>

#include <kodo/storage_encoder.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>

#include "basic_api_test_helper.hpp"


struct test_storage_params
{
    uint32_t m_data_size;
    uint32_t m_max_symbols;
    uint32_t m_max_symbol_size;
    uint32_t m_symbols;
    uint32_t m_symbol_size;
    uint32_t m_offset;
    uint32_t m_size;

};

std::vector<uint8_t> create_storage(uint32_t size)
{
    std::vector<uint8_t> data(size);
    for(uint32_t i = 0; i < size; ++i)
    {
        data[i] = rand() % 255;
    }

    return data;
}


template<class Encoder>
void test_storage_reader(const test_storage_params& params)
{
    auto data = create_storage(params.m_data_size);

    typedef typename Encoder::factory factory_t;
    typedef typename Encoder::pointer encoder_t;

    factory_t factory(params.m_max_symbols, params.m_max_symbol_size);
    encoder_t encoder = factory.build(params.m_symbols, params.m_symbol_size);

    kodo::storage_reader<Encoder> reader(sak::storage(data));

    reader.read(encoder, params.m_offset, params.m_size);

    EXPECT_EQ(params.m_size, encoder->bytes_used());

    std::vector<uint8_t> data_out(encoder->bytes_used());
    encoder->copy_symbols(sak::storage(data_out));

    auto first = data.begin() + params.m_offset;
    auto last  = first + params.m_size;
    std::vector<uint8_t> data_in(first, last);

    EXPECT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));
}


void test_storage_reader(const test_storage_params& params)
{
    test_storage_reader<kodo::full_rlnc_encoder<fifi::binary>>(params);
    test_storage_reader<kodo::full_rlnc_encoder<fifi::binary8>>(params);
    test_storage_reader<kodo::full_rlnc_encoder<fifi::binary16>>(params);
}


TEST(TestStorageReader, test_storage_reader)
{
    {
        test_storage_params params;
        params.m_max_symbols = 16;
        params.m_max_symbol_size = 1600;
        params.m_symbols = params.m_max_symbols;
        params.m_symbol_size = params.m_max_symbol_size;
        params.m_data_size = params.m_max_symbols * params.m_max_symbol_size;
        params.m_offset = 0;
        params.m_size = params.m_data_size;

        test_storage_reader(params);
    }

    {
        test_storage_params params;
        params.m_max_symbols = rand_symbols();
        params.m_max_symbol_size = rand_symbol_size();
        params.m_symbols = params.m_max_symbols;
        params.m_symbol_size = params.m_max_symbol_size;
        params.m_data_size = params.m_max_symbols * params.m_max_symbol_size;
        params.m_offset = rand() % params.m_data_size;
        params.m_size = rand() % (params.m_data_size - params.m_offset);

        test_storage_reader(params);
    }

}

// TEST(TestStorageEncoder, test_storage_encoder)
// {

//     {
//         uint32_t size = 500;

//         std::vector<uint8_t> v(size);

//         kodo::const_storage cs = kodo::storage(v);

//         typedef kodo::storage_encoder<kodo::full_rlnc2_encoder, kodo::rfc5052_partitioning_scheme>
//             rlnc_storage_encoder;

//         kodo::full_rlnc2_encoder::factory f(10,10);
        
//         rlnc_storage_encoder encoder(f, cs);
        
        
//     }

// }
 



