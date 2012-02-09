// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#include <gtest/gtest.h>

#include <stdint.h>

#include <kodo/storage.h>


TEST(TestStorageFunction, test_storage_function)
{
    srand(time(0));

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



