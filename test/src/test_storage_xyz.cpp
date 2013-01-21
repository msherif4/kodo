// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <stdint.h>

#include <gtest/gtest.h>

#include <kodo/storage.hpp>
#include <kodo/storage_encoder.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>


TEST(TestStorageFunction, test_storage_function)
{
    {
        uint32_t size = 500;

        std::vector<uint8_t> v(size);

        kodo::const_storage cs = kodo::storage(v);
        EXPECT_TRUE(cs.m_size == size);
        EXPECT_TRUE(cs.m_data == &v[0]);

        kodo::mutable_storage ms = kodo::storage(v);
        EXPECT_TRUE(ms.m_size == size);
        EXPECT_TRUE(ms.m_data == &v[0]);
    }

}



TEST(TestStorageEncoder, test_storage_encoder)
{

    {
        uint32_t size = 500;

        std::vector<uint8_t> v(size);

        kodo::const_storage cs = kodo::storage(v);

        typedef kodo::storage_encoder<kodo::full_rlnc2_encoder, kodo::rfc5052_partitioning_scheme>
            rlnc_storage_encoder;

        kodo::full_rlnc2_encoder::factory f(10,10);
        
        rlnc_storage_encoder encoder(f, cs);
        
        
    }

}




