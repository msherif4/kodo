// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_copy_payload Unit test for copy_payload layer

/// Tests:
///   - layer::construct(uint32_t,uint32_t)
///   - layer::decode(const uint8_t*)

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include <fifi/default_field.hpp>

#include <kodo/copy_payload.hpp>
#include <kodo/payload_decoder.hpp>
#include <kodo/systematic_decoder.hpp>
#include <kodo/symbol_id_decoder.hpp>
#include <kodo/plain_symbol_id_reader.hpp>
#include <kodo/linear_block_decoder.hpp>
#include <kodo/coefficient_storage.hpp>
#include <kodo/coefficient_info.hpp>
#include <kodo/finite_field_math.hpp>
#include <kodo/symbol_storage_tracker.hpp>
#include <kodo/deep_symbol_storage.hpp>
#include <kodo/storage_bytes_used.hpp>
#include <kodo/storage_block_info.hpp>
#include <kodo/final_coder_factory_pool.hpp>

/// Here we define the stacks which should be tested.
namespace kodo
{
    // Test layer against real api to ensure that we get an error if the layer
    // doesn't complies with the api.
    template<class Field>
    class copy_payload_stack
        : public // Payload API
                 copy_payload<
                 payload_decoder<
                 // Codec Header API
                 systematic_decoder<
                 symbol_id_decoder<
                 // Symbol ID API
                 plain_symbol_id_reader<
                 // Codec API
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Finite Field Math API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 // Storage API
                 symbol_storage_tracker<
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 copy_payload_stack<Field>, Field>
                     > > > > > > > > > > > > >
    {};

    // A dummi api to replace the real stack
    class dummy_api
    {
    public:

        /// @copydoc layer::construct()
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
               (void)max_symbols;
               m_symbol_size = max_symbol_size;
            }

        /// @copydoc layer::decode(uint8_t*)
        void decode(uint8_t *payload)
            {
                assert(payload != 0);

                // Clear payload
                memset(payload, 0, payload_size());
            }

        /// @copydoc layer::payload_size() const
        uint32_t payload_size() const
            {
                return m_symbol_size;
            }

    private:

        /// Number of symbols
        uint32_t m_symbol_size;
    };

    // Test functionality of the individual layer 
    typedef copy_payload<dummy_api> copy_payload_coder; 
}

// Test the layer itself - confirms that it acts as expected
void test_layer(uint32_t symbols, uint32_t symbol_size)
{
    // Create and initialize coder
    kodo::copy_payload_coder coder;
    coder.construct(symbols, symbol_size);

    std::vector<uint8_t> payload(coder.payload_size(), 'a');
    std::vector<uint8_t> payload_copy(payload);

    ASSERT_EQ(payload.size(), payload_copy.size());

    // Ensure that payload and payload_copy are equal
    EXPECT_TRUE(
        std::equal(payload.begin(), payload.end(), payload_copy.begin()) );

    coder.decode(&payload[0]);

    // Test that payload isn't changed during decoding
    EXPECT_TRUE(
        std::equal(payload.begin(), payload.end(), payload_copy.begin()) );

}

// Ensure that the layer complies to the API
void test_stack(uint32_t symbols, uint32_t symbol_size)
{
    typedef kodo::copy_payload_stack<fifi::binary8> copy_paylaod_coder_t;
    
    copy_paylaod_coder_t::factory coder_factory(symbols, symbol_size);
    copy_paylaod_coder_t::pointer coder = coder_factory.build(symbols, symbol_size);

    std::vector<uint8_t> payload(coder->payload_size(), 'a');

    coder->decode(&payload[0]);
}

/// Run the tests
TEST(TestCopyPayload, test_copy_payload_stack)
{
    uint32_t symbols = 8;
    uint32_t symbol_size = 8;

    test_stack(symbols, symbol_size);
    test_layer(symbols, symbol_size);
}
