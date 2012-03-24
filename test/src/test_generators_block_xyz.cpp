// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <gtest/gtest.h>
#include <ctime>
#include <kodo/generators/block.h>
#include <kodo/generators/block_seed.h>
#include <kodo/generators/block_cache.h>


template<template <class> class Block, class ValueType>
void create_blocks(uint32_t length)
{
    std::vector<ValueType> zero(length);
    std::vector<ValueType> one(length);
    std::vector<ValueType> two(length);
    
    typename Block<ValueType>::factory f;
    typename Block<ValueType>::pointer block = f.build(length);

    block->fill(0, &zero[0]);
    block->fill(0, &one[0]);
    block->fill(0, &two[0]);
    
    EXPECT_TRUE(std::equal(zero.begin(), zero.end(), one.begin()));
    EXPECT_TRUE(std::equal(one.begin(), one.end(), two.begin()));

    block->fill(99, &one[0]);
    block->fill(99, &two[0]);
    EXPECT_FALSE(std::equal(zero.begin(), zero.end(), one.begin()));
    EXPECT_TRUE(std::equal(one.end(), one.end(), two.begin()));

    block->fill(0, &one[0]);
    EXPECT_TRUE(std::equal(zero.begin(), zero.end(), one.begin()));    
}

TEST(TestGeneratorBlock, basic)
{
    create_blocks<kodo::block_uniform, uint8_t>(73);
    create_blocks<kodo::block_uniform, uint16_t>(42);
    create_blocks<kodo::block_uniform, uint32_t>(42);
    create_blocks<kodo::block_uniform, uint64_t>(13);
}

TEST(TestGeneratorBlock, block_seed)
{
    create_blocks<kodo::block_seed_uniform, uint8_t>(73);
    create_blocks<kodo::block_seed_uniform, uint16_t>(42);
    create_blocks<kodo::block_seed_uniform, uint32_t>(42);
    create_blocks<kodo::block_seed_uniform, uint64_t>(13);
}

TEST(TestGeneratorBlock, block_cache)
{
    create_blocks<kodo::block_cache_uniform, uint8_t>(73);
    create_blocks<kodo::block_cache_uniform, uint16_t>(42);
    create_blocks<kodo::block_cache_uniform, uint32_t>(42);
    create_blocks<kodo::block_cache_uniform, uint64_t>(13);
}

TEST(TestGeneratorBlock, block_seed_cache)
{
    create_blocks<kodo::block_cache_seed_uniform, uint8_t>(73);
    create_blocks<kodo::block_cache_seed_uniform, uint16_t>(42);
    create_blocks<kodo::block_cache_seed_uniform, uint32_t>(42);
    create_blocks<kodo::block_cache_seed_uniform, uint64_t>(13);
}


