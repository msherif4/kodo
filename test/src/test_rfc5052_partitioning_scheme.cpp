// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cstdint>
#include <ctime>
#include <algorithm>

#include <gtest/gtest.h>

#include <kodo/rfc5052_partitioning_scheme.hpp>

TEST(TestRfc5052PartitioningScheme, partition_in_hand)
{
    // Based on in-hand calculations according to the formulars described
    // in rfc5052
    uint32_t max_symbols = 16;
    uint32_t max_symbol_size = 1500;
    uint32_t object_size = 123456;

    kodo::rfc5052_partitioning_scheme partitioning(
        max_symbols, max_symbol_size, object_size);

    // Check that we have the expected total
    // number of blocks
    ASSERT_EQ(uint32_t(6), partitioning.blocks());

    uint32_t a_large = 14;
    uint32_t a_small = 13;

    // Check that the blocks have the right size
    ASSERT_EQ(partitioning.symbols(0), a_large);
    ASSERT_EQ(partitioning.symbols(1), a_large);
    ASSERT_EQ(partitioning.symbols(2), a_large);
    ASSERT_EQ(partitioning.symbols(3), a_large);
    ASSERT_EQ(partitioning.symbols(4), a_large);
    ASSERT_EQ(partitioning.symbols(5), a_small);

    // Check the symbol size
    ASSERT_EQ(partitioning.symbol_size(0), uint32_t(1500));
    ASSERT_EQ(partitioning.symbol_size(1), uint32_t(1500));
    ASSERT_EQ(partitioning.symbol_size(2), uint32_t(1500));
    ASSERT_EQ(partitioning.symbol_size(3), uint32_t(1500));
    ASSERT_EQ(partitioning.symbol_size(4), uint32_t(1500));
    ASSERT_EQ(partitioning.symbol_size(5), uint32_t(1500));
}



TEST(TestRfc5052PartitioningScheme, partition_two_blocks)
{
    // Making small transfere only containing
    // two blocks, one large, one small
    uint32_t max_symbols = 16;
    uint32_t max_symbol_size = 1000;
    uint32_t object_size = (max_symbols * max_symbol_size) + 1;

    kodo::rfc5052_partitioning_scheme partitioning(
        max_symbols, max_symbol_size, object_size);

    ASSERT_EQ(partitioning.blocks(), uint32_t(2));

    ASSERT_TRUE(partitioning.symbols(0)*partitioning.symbol_size(0) +
                partitioning.symbols(1)*partitioning.symbol_size(1)
		>= object_size);
}

TEST(TestRfc5052PartitioningScheme, partition_size)
{
    // Making a random number of blocks, and ensuring that
    // they are always either stay same size or a decrease
    // in size in regards to the previous block.
    uint32_t max_symbols = (rand() % 64) + 1;
    uint32_t max_symbol_size = (rand() % 3000) + 1;
    uint32_t object_size = rand()%1000000 +1;

    kodo::rfc5052_partitioning_scheme partitioning(
        max_symbols, max_symbol_size, object_size);

    ASSERT_TRUE(partitioning.blocks() > 0);

    for(uint32_t i = 1; i < partitioning.blocks(); ++i)
    {
        ASSERT_TRUE(partitioning.symbols(i-1) >= partitioning.symbols(i));
    }
}

