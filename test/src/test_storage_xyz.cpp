// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <stdint.h>

#include <gtest/gtest.h>

#include <kodo/storage.hpp>

TEST(TestStorageFunction, test_storage_function)
{
    srand(static_cast<uint32_t>(time(0)));

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

