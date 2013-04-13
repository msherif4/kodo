// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_matrix.cpp Unit tests for the kodo::matrix class

#include <cstdint>
#include <ctime>
#include <algorithm>

#include <gtest/gtest.h>

#include <kodo/matrix.hpp>

#include <fifi/fifi_utils.hpp>

TEST(TestMatrix, invoke_api)
{
    uint32_t rows = 16;
    uint32_t columns = 1500;

    typedef fifi::binary field_type;
    typedef field_type::value_type value_type;

    kodo::matrix<fifi::binary> m(rows, columns);

    ASSERT_EQ(rows, m.rows());
    ASSERT_EQ(columns, m.columns());

    uint32_t row_size = fifi::elements_to_size<field_type>(columns);
    uint32_t row_length = fifi::size_to_length<field_type>(row_size);

    ASSERT_EQ(row_size, m.row_size());
    ASSERT_EQ(row_length, m.row_length());

    // Set all the elements
    for(uint32_t i = 0; i < rows; ++i)
    {
        for(uint32_t j = 0; j < columns; ++j)
        {
            // Odd column index equal to 1
            value_type v = j % 2 ? 0U : 1U;
            m.set_element(i,j,v);
        }
    }

    // Get them back, but how using the row
    for(uint32_t i = 0; i < rows; ++i)
    {
        const value_type *row = m.row(i);

        for(uint32_t j = 0; j < columns; ++j)
        {
            value_type expect = j % 2 ? 0U : 1U;
            value_type actual = fifi::get_value<field_type>(row, j);

            ASSERT_EQ(expect, actual);

            // The element function should return the same result
            actual = m.element(i,j);

            ASSERT_EQ(expect, actual);
        }
    }

    // Transpose the matrix
    kodo::matrix<field_type> m2 = m.transpose();

    ASSERT_EQ(columns, m2.rows());
    ASSERT_EQ(rows, m2.columns());

    // Get them back, but how using the row
    for(uint32_t i = 0; i < m2.rows(); ++i)
    {
        const value_type *row = m2.row(i);

        for(uint32_t j = 0; j < m2.columns(); ++j)
        {
            value_type expect = i % 2 ? 0U : 1U;
            value_type actual = fifi::get_value<field_type>(row, j);

            ASSERT_EQ(expect, actual);

            // The element function should return the same result
            actual = m2.element(i,j);

            ASSERT_EQ(expect, actual);
        }
    }

}


