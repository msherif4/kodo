// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_largest_nonzero_index_decoder.cpp Unit tests for the
///       kodo::largest_nonzero_index_decoder

#include <cstdint>
#include <gtest/gtest.h>

#include <kodo/largest_nonzero_index_decoder.hpp>
#include <kodo/empty_decoder.hpp>
#include <kodo/rank_info.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>

#include <kodo/debug_linear_block_decoder.hpp>

#include "basic_api_test_helper.hpp"

namespace kodo
{
    template<class Field>
    class test_partial_stack
        : public // Payload API
                 // Codec Header API
                 // Symbol ID API
                 // Codec API
                 largest_nonzero_index_decoder<
                 forward_linear_block_decoder<
                 rank_info<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 test_partial_stack<Field>
                     > > > > > > > > > > >
    { };

}

/// Run the tests typical coefficients stack
TEST(TestLargestNonzeroIndexDecoder, test_index)
{
    typedef fifi::binary field_type;

    kodo::test_partial_stack<field_type>::factory f(5, 1600);

    auto d = f.build();

    EXPECT_EQ(d->coefficients_size(), 1U);
    EXPECT_EQ(d->encoder_rank(), 0U);
    EXPECT_FALSE(d->nonzero_index_seen());

    uint8_t coefficients[1];

    // Create an encoding vector looking like this: 01100
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);

    // Create a dummy symbol
    std::vector<uint8_t> symbol = random_vector(d->symbol_size());

    d->decode_symbol(&symbol[0], coefficients);

    EXPECT_EQ(d->rank(), 1U);
    EXPECT_EQ(d->encoder_rank(), 3U);
    EXPECT_TRUE(d->nonzero_index_seen());
    EXPECT_EQ(d->largest_nonzero_index(), 2U);

    // Create an encoding vector looking like this: 11000
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 1, 1U);

    d->decode_symbol(&symbol[0], coefficients);

    EXPECT_EQ(d->rank(), 2U);
    EXPECT_EQ(d->encoder_rank(), 3U);
    EXPECT_TRUE(d->nonzero_index_seen());
    EXPECT_EQ(d->largest_nonzero_index(), 2U);

    // Create an encoding vector looking like this: 11100
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);

    d->decode_symbol(&symbol[0], coefficients);

    EXPECT_EQ(d->rank(), 3U);
    EXPECT_EQ(d->encoder_rank(), 3U);
    EXPECT_TRUE(d->nonzero_index_seen());
    EXPECT_EQ(d->largest_nonzero_index(), 2U);

    // Create an encoding vector looking like this: 11101
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);
    fifi::set_value<field_type>(coefficients, 4, 1U);

    d->decode_symbol(&symbol[0], coefficients);

    EXPECT_EQ(d->rank(), 4U);
    EXPECT_EQ(d->encoder_rank(), 5U);
    EXPECT_TRUE(d->nonzero_index_seen());
    EXPECT_EQ(d->largest_nonzero_index(), 4U);

    // Create an encoding vector looking like this: 01110
    coefficients[0] = 0;

    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);
    fifi::set_value<field_type>(coefficients, 3, 1U);

    d->decode_symbol(&symbol[0], coefficients);

    EXPECT_EQ(d->rank(), 5U);
    EXPECT_EQ(d->encoder_rank(), 5U);
    EXPECT_TRUE(d->nonzero_index_seen());
    EXPECT_EQ(d->largest_nonzero_index(), 4U);

}



