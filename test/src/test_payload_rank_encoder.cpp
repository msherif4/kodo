// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_payload_rank_encoder.cpp Unit tests for the
///       kodo::payload_rank_encoder layer

#include <cstdint>
#include <gtest/gtest.h>

#include <kodo/payload_rank_encoder.hpp>

namespace kodo
{

    // Small helper struct which provides the API needed by the
    // payload_rank_encoder layer.
    struct dummy_final
    {
        typedef uint32_t rank_type;

        struct factory
        {
            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : m_max_payload_size(0)
            {
                (void) max_symbols;
                (void) max_symbol_size;
            }

            uint32_t max_payload_size() const
            {
                return m_max_payload_size;
            }

            uint32_t m_max_payload_size;
        };


        uint32_t encode(uint8_t *payload)
        {
            m_payload = payload;
            return 0;
        }

        rank_type rank() const
        {
            return m_rank;
        }

        uint32_t payload_size() const
        {
            return m_payload_size;
        }

        uint8_t *m_payload;
        uint32_t m_payload_size;
        rank_type m_rank;
    };

    // Instantiate a stack containing the payload_rank_encoder
    class rank_encoder_stack
        : public // Payload API
                 payload_rank_encoder<
                 // Codec Header API
                 // Symbol ID API
                 // Codec API
                 // Coefficient Storage API
                 // Storage API
                 // Finite Field API
                 // Factory API
                 // Final type
                 dummy_final>
    { };

}

/// Run the tests typical coefficients stack
TEST(TestPayloadRankEncoder, test_payload_rank_encoder)
{

    kodo::rank_encoder_stack::factory factory(0U, 0U);

    uint32_t sizeof_rank_type = sizeof(kodo::dummy_final::rank_type);

    // The payload rank decoder uses sizeof(rank_type) to determine
    // how much to read
    factory.m_max_payload_size = 0U;
    EXPECT_EQ(factory.max_payload_size(), sizeof_rank_type);

    factory.m_max_payload_size = 1U;
    EXPECT_EQ(factory.max_payload_size(), sizeof_rank_type + 1U);

    kodo::rank_encoder_stack stack;

    stack.m_payload_size = 0U;
    EXPECT_EQ(stack.payload_size(), sizeof_rank_type);

    stack.m_payload_size = 1U;
    EXPECT_EQ(stack.payload_size(), sizeof_rank_type + 1U);

    std::vector<uint8_t> payload(sizeof_rank_type);

    stack.m_rank = 0U;

    uint32_t used = stack.encode(&payload[0]);

    EXPECT_EQ(used, sizeof_rank_type);
    EXPECT_EQ(&payload[sizeof_rank_type], stack.m_payload);

    kodo::dummy_final::rank_type encoder_rank =
        sak::big_endian::get<kodo::dummy_final::rank_type>(&payload[0]);

    EXPECT_EQ(encoder_rank, stack.m_rank);


    stack.m_rank = 10U;

    used = stack.encode(&payload[0]);

    EXPECT_EQ(used, sizeof_rank_type);
    EXPECT_EQ(&payload[sizeof_rank_type], stack.m_payload);

    encoder_rank =
        sak::big_endian::get<kodo::dummy_final::rank_type>(&payload[0]);

    EXPECT_EQ(encoder_rank, stack.m_rank);

    stack.m_rank = 11U;

    used = stack.encode(&payload[0]);

    EXPECT_EQ(used, sizeof_rank_type);
    EXPECT_EQ(&payload[sizeof_rank_type], stack.m_payload);

    encoder_rank =
        sak::big_endian::get<kodo::dummy_final::rank_type>(&payload[0]);

    EXPECT_EQ(encoder_rank, stack.m_rank);
}


