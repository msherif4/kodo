// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_storage_reader.cpp Unit tests for the storage_reader object

#include <cstdint>

#include <gtest/gtest.h>

#include <boost/make_shared.hpp>

#include <kodo/storage_reader.hpp>

#include "basic_api_test_helper.hpp"

// A dummy encoder class to replace using a real encoder
class dummy_encoder
{
public:

    // Pointer to the dummy encoder
    typedef boost::shared_ptr<dummy_encoder> pointer;

public:

    /// @copydoc layer::set_symbols(const sak::const_storage&)
    void set_symbols(const sak::const_storage &symbol_storage)
        {
            m_symbol_storage = symbol_storage;
        }

    /// @copydoc layer::set_bytes_used(uint32_t)
    void set_bytes_used(uint32_t bytes_used)
        {
            m_bytes_used = bytes_used;
        }

public:

    // Save the storage if need in the test
    sak::const_storage m_symbol_storage;

    // Save the bytes use if needed in the test
    uint32_t m_bytes_used;

};

/// Tests:
///  - storage_reader::size() const
///  - storage_reader::read(pointer, uint32_t, uint32_t)
TEST(TestStorageReader, test_storage_reader)
{
    uint32_t data_size = 1000;

    std::vector<uint8_t> data = random_vector(data_size);
    sak::const_storage data_storage = sak::storage(data);

    kodo::storage_reader<dummy_encoder> reader(data_storage);

    EXPECT_EQ(reader.size(), data_size);

    auto encoder = boost::make_shared<dummy_encoder>();

    reader.read(encoder, 10U, 10U);

    EXPECT_EQ(encoder->m_bytes_used, 10U);
    EXPECT_TRUE(sak::equal(encoder->m_symbol_storage,
                           sak::storage(&data[10], 10U)));

    reader.read(encoder, 57U, 101U);

    EXPECT_EQ(encoder->m_bytes_used, 101U);
    EXPECT_TRUE(sak::equal(encoder->m_symbol_storage,
                           sak::storage(&data[57], 101U)));

    // Try with some random values
    uint32_t random_offset = rand_nonzero(data_size);
    uint32_t random_size = rand_nonzero(data_size - random_offset);

    reader.read(encoder, random_offset, random_size);

    EXPECT_EQ(encoder->m_bytes_used, random_size);
    EXPECT_TRUE(sak::equal(encoder->m_symbol_storage,
                           sak::storage(&data[random_offset], random_size)));

}





