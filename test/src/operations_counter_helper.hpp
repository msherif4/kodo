// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <kodo/operations_counter.hpp>

/// Helper function which sets all values in the counter
/// @param counter The counter to be initialized
/// @param value The value to use for initialization
inline void set_values(kodo::operations_counter &counter, uint32_t value)
{
    counter.m_multiply = value;
    counter.m_multiply_add = value;
    counter.m_add = value;
    counter.m_multiply_subtract = value;
    counter.m_subtract = value;
    counter.m_invert = value;
}

/// Helper function which tests all values in the counter
/// @param counter The counter to be tested
/// @param value The value to use for testing
inline void test_values(kodo::operations_counter &counter, uint32_t value)
{
    EXPECT_EQ(counter.m_multiply, value);
    EXPECT_EQ(counter.m_multiply_add, value);
    EXPECT_EQ(counter.m_add, value);
    EXPECT_EQ(counter.m_multiply_subtract, value);
    EXPECT_EQ(counter.m_subtract, value);
    EXPECT_EQ(counter.m_invert, value);
}


