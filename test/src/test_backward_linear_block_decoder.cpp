// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_backward_linear_block_decoder.cpp Unit tests for the
///       kodo::backward_linear_block_decoder

#include <cstdint>
#include <gtest/gtest.h>

#include <kodo/backward_linear_block_decoder.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>

#include <kodo/debug_linear_block_decoder.hpp>

#include "basic_api_test_helper.hpp"

namespace kodo
{

    template<class Field>
    class test_backward_stack
        : public // Payload API
                 // Codec Header API
                 // Symbol ID API
                 // Codec API
                 debug_linear_block_decoder<
                 backward_linear_block_decoder<
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
                 test_backward_stack<Field>
                     > > > > > > > > > >
    { };

    template<class Field>
    class test_backward_delayed_stack
        : public // Payload API
                 // Codec Header API
                 // Symbol ID API
                 // Codec API
                 debug_linear_block_decoder<
                 linear_block_decoder_delayed<
                 backward_linear_block_decoder<
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
                 test_backward_stack<Field>
                     > > > > > > > > > > >
    { };

}

template<template <class> class Stack>
void test_backward_stack()
{
    typedef fifi::binary field_type;

    typename Stack<field_type>::factory f(8, 1600);

    auto d = f.build();

    EXPECT_EQ(d->coefficients_size(), 1U);

    uint8_t coefficients[1];

    // Create an encoding vector looking like this: 01000010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 6, 1U);

    // Create a dummy symbol
    std::vector<uint8_t> symbol = random_vector(d->symbol_size());

    d->decode_symbol(&symbol[0], coefficients);
    // d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 1U);
    EXPECT_TRUE(d->symbol_pivot(6));

    // Create an encoding vector looking like this: 10000010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 6, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    // d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 2U);
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(1));

    // Create an encoding vector looking like this: 11100010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);
    fifi::set_value<field_type>(coefficients, 6, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    // d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 3U);
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(1));
    EXPECT_TRUE(d->symbol_pivot(2));

    // Create an encoding vector looking like this: 11100010 (linear dept.)
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);
    fifi::set_value<field_type>(coefficients, 6, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    // d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 3U);
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(1));
    EXPECT_TRUE(d->symbol_pivot(2));


    // Create an encoding vector looking like this: 10111100
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);
    fifi::set_value<field_type>(coefficients, 3, 1U);
    fifi::set_value<field_type>(coefficients, 4, 1U);
    fifi::set_value<field_type>(coefficients, 5, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    // d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 4U);
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(5));
    EXPECT_TRUE(d->symbol_pivot(1));
    EXPECT_TRUE(d->symbol_pivot(2));


    // Create an encoding vector looking like this: 10000100
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 5, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    // d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 5U);
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(5));
    EXPECT_TRUE(d->symbol_pivot(4));
    EXPECT_TRUE(d->symbol_pivot(2));
    EXPECT_TRUE(d->symbol_pivot(1));

    // Create an encoding vector looking like this: 10010000
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 3, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    // d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 6U);
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(5));
    EXPECT_TRUE(d->symbol_pivot(4));
    EXPECT_TRUE(d->symbol_pivot(3));
    EXPECT_TRUE(d->symbol_pivot(2));
    EXPECT_TRUE(d->symbol_pivot(1));

    // Create an encoding vector looking like this: 00011001
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 3, 1U);
    fifi::set_value<field_type>(coefficients, 4, 1U);
    fifi::set_value<field_type>(coefficients, 7, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    // d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 7U);
    EXPECT_TRUE(d->symbol_pivot(7));
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(5));
    EXPECT_TRUE(d->symbol_pivot(4));
    EXPECT_TRUE(d->symbol_pivot(3));
    EXPECT_TRUE(d->symbol_pivot(2));
    EXPECT_TRUE(d->symbol_pivot(1));

    // Create an encoding vector looking like this: 00011001
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    // d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 8U);
    EXPECT_TRUE(d->symbol_pivot(7));
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(5));
    EXPECT_TRUE(d->symbol_pivot(4));
    EXPECT_TRUE(d->symbol_pivot(3));
    EXPECT_TRUE(d->symbol_pivot(2));
    EXPECT_TRUE(d->symbol_pivot(1));
    EXPECT_TRUE(d->symbol_pivot(0));

}


/// Run the tests typical coefficients stack
TEST(TestBackwardLinearBlockDecoder, test_decoder)
{
    test_backward_stack<kodo::test_backward_stack>();
}

/// Run the tests typical coefficients stack
TEST(TestBackwardLinearBlockDecoder, test_decoder_delayed)
{
    test_backward_stack<kodo::test_backward_delayed_stack>();
}



