// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_operations_counter.cpp Unit tests for the
///       kodo::operations_counter class

#include <cstdint>

#include <gtest/gtest.h>

#include <kodo/operations_counter.hpp>

#include "operations_counter_helper.hpp"


/// Run the tests typical coefficients stack
TEST(TestOperationsCounter, test_operations_counter)
{

    {
        kodo::operations_counter counter;
        test_values(counter, 0U);
    }

    {
        kodo::operations_counter a;
        set_values(a, 1U);

        kodo::operations_counter b;
        set_values(b, 2U);

        a = a + b;

        test_values(a, 3U);

        EXPECT_TRUE(a >= b);

        a = a - b;

        test_values(a, 1U);

        EXPECT_FALSE(a >= b);
    }

}



