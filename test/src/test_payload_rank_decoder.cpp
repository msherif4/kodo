// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_payload_rank_decoder.cpp Unit tests for the
///       kodo::payload_rank_decoder layer

#include <cstdint>
#include <gtest/gtest.h>

#include <kodo/payload_rank_decoder.hpp>

namespace kodo
{

    // Small helper struct which provides the API needed by the
    // payload_rank_decoder layer.
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


        template<class Factory>
        void initialize(Factory& the_factory)
        {
            (void) the_factory;
        }


        void decode(uint8_t *payload)
        {
            m_payload = payload;
        }

        uint32_t payload_size() const
        {
            return m_payload_size;
        }

        uint8_t *m_payload;
        uint32_t m_payload_size;
    };

    // Instantiate a stack containing the payload_rank_decoder
    class rank_decoder_stack
        : public // Payload API
                 payload_rank_decoder<
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
TEST(TestPayloadRankDecoder, test_payload_rank_decoder)
{

    kodo::rank_decoder_stack::factory factory(0U, 0U);

    uint32_t sizeof_rank_type = sizeof(kodo::dummy_final::rank_type);

    // The payload rank decoder uses sizeof(rank_type) to determine
    // how much to read
    factory.m_max_payload_size = 0U;
    EXPECT_EQ(factory.max_payload_size(), sizeof_rank_type);

    factory.m_max_payload_size = 1U;
    EXPECT_EQ(factory.max_payload_size(), sizeof_rank_type + 1U);

    kodo::rank_decoder_stack stack;

    // Needs a factory - not used so we can pass any type
    stack.initialize(factory);
    EXPECT_EQ(stack.encoder_rank(), 0U);

    stack.m_payload_size = 0U;
    EXPECT_EQ(stack.payload_size(), sizeof_rank_type);

    stack.m_payload_size = 1U;
    EXPECT_EQ(stack.payload_size(), sizeof_rank_type + 1U);

    std::vector<uint8_t> payload(sizeof_rank_type);

    sak::big_endian::put<kodo::dummy_final::rank_type>(0U, &payload[0]);
    stack.decode(&payload[0]);
    EXPECT_EQ(stack.encoder_rank(), 0U);

    // This is the payload buffer received in the dummy_final we expect
    // it to be incremented past the rank information
    EXPECT_EQ(stack.m_payload, &payload[sizeof_rank_type]);

    sak::big_endian::put<kodo::dummy_final::rank_type>(1U, &payload[0]);
    stack.decode(&payload[0]);
    EXPECT_EQ(stack.encoder_rank(), 1U);
    EXPECT_EQ(stack.m_payload, &payload[sizeof_rank_type]);

    sak::big_endian::put<kodo::dummy_final::rank_type>(10U, &payload[0]);
    stack.decode(&payload[0]);
    EXPECT_EQ(stack.encoder_rank(), 10U);
    EXPECT_EQ(stack.m_payload, &payload[sizeof_rank_type]);

    sak::big_endian::put<kodo::dummy_final::rank_type>(10U, &payload[0]);
    stack.decode(&payload[0]);
    EXPECT_EQ(stack.encoder_rank(), 10U);
    EXPECT_EQ(stack.m_payload, &payload[sizeof_rank_type]);

    stack.initialize(factory);
    EXPECT_EQ(stack.encoder_rank(), 0U);

}


