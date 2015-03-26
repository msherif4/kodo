// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_partial_decoding_tracker.cpp Unit tests for the
///       kodo::partial_decoding_tracker

#include <cstdint>
#include <gtest/gtest.h>

#include <kodo/partial_decoding_tracker.hpp>

namespace kodo
{

    // Small helper struct which provides the API needed by the
    // partial_decoding_tracker layer.
    struct dummy_rank
    {
        typedef uint32_t rank_type;

        uint32_t rank() const
        {
            return m_rank;
        }

        uint32_t encoder_rank() const
        {
            return m_encoder_rank;
        }

        uint32_t m_rank;
        uint32_t m_encoder_rank;
    };

    // Instantiate a stack containing the partial_decoding_tracker
    class test_partial_stack
        : public // Payload API
                 // Codec Header API
                 // Symbol ID API
                 // Codec API
                 partial_decoding_tracker<
                 // Coefficient Storage API
                 // Storage API
                 // Finite Field API
                 // Factory API
                 // Final type
                 dummy_rank>
    { };

}

/// Run the tests typical coefficients stack
TEST(TestPartialDecodingTracker, test_partial_decoding)
{
    kodo::test_partial_stack stack;
    stack.m_rank = 0;
    stack.m_encoder_rank = 0;

    EXPECT_FALSE(stack.is_partial_complete());

    stack.m_encoder_rank = 1;
    EXPECT_FALSE(stack.is_partial_complete());

    stack.m_rank = 1;
    EXPECT_TRUE(stack.is_partial_complete());
}


