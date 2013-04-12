// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_rank_callback_decoder Unit test for rank_callback_decoder layer

/// Tests:
///   - layer::initialize(uint32_t,uint32_t)
///   - layer::decode_symbol(uint8_t*,uint8_t*)
///   - layer::decode_symbol(uint8_t*,uint32_t)
///   - layer::set_rank_changed_callback()
///   - layer::reset_rank_changed_callback()

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include <fifi/default_field.hpp>

#include <kodo/rank_callback_decoder.hpp>
#include <kodo/linear_block_decoder.hpp>
#include <kodo/coefficient_storage.hpp>
#include <kodo/coefficient_info.hpp>
#include <kodo/finite_field_math.hpp>
#include <kodo/finite_field_info.hpp>
#include <kodo/deep_symbol_storage.hpp>
#include <kodo/storage_bytes_used.hpp>
#include <kodo/storage_block_info.hpp>
#include <kodo/final_coder_factory_pool.hpp>

/// Here we define the stacks which should be tested.
namespace kodo
{
    // Test layer against real api to ensure that we get an error if the
    // layer doesn't complies with the api.
    template<class Field>
    class rank_callback_decoder_stack
        : public // Codec API
                 rank_callback_decoder<
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Storage api
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field Math API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 rank_callback_decoder_stack<Field>
                     > > > > > > > > > >
    {};

    // A dummi api to replace the real stack
    class dummy_codec_api
    {
    public:

        /// Reset rank changed callback function
        /// @copydoc layer::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                (void)symbol_size;
                m_symbols = symbols;
                m_rank = 0;
            }

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data,
                           uint8_t *symbol_coefficients)
            {
                (void) symbol_data;
                (void) symbol_coefficients;
                ++m_rank;
            }

        /// @copydoc layer::decode_symbol(const uint8_t*, uint32_t)
        void decode_symbol(const uint8_t *symbol_data,
                           uint32_t symbol_index)
            {
                (void) symbol_data;
                (void) symbol_index;
                ++m_rank;
            }

        /// @copydoc layer::rank() const
        uint32_t rank() const
            {
                return m_rank;
            }

    private:

        /// The current rank of the decoder
        uint32_t m_rank;

        /// Number of symbols
        uint32_t m_symbols;
    };

    // Test functionality of the individual layer
    typedef rank_callback_decoder<dummy_codec_api> rank_coder;
}

// callback handler
void rank_changed_event(kodo::rank_coder& coder, uint32_t& callback_count, uint32_t rank)
{
    ++callback_count;
    EXPECT_EQ(coder.rank(), rank);
}

// Test the layer itself - confirms that it acts as expected
void test_rank_callback_decoder_layer(uint32_t symbols, uint32_t symbol_size)
{
    uint32_t callback_count = 0;
    uint32_t expected_callback_count = 0;
    uint32_t expected_rank = 0;

    std::vector<uint8_t> symbol_coefficients(symbols, 0);
    std::vector<uint8_t> symbol_data(symbol_size, 0);
    uint32_t symbol_id = 0;


    // Create and initialize coder
    kodo::rank_coder coder;
    coder.initialize(symbols, symbol_size);


    // Expect rank initialized to zero
    EXPECT_EQ(coder.rank(), expected_rank);

    // Test decoding with symbol coefficients before setting callback
    coder.decode_symbol(&symbol_data[0], &symbol_coefficients[0]);

    EXPECT_EQ(coder.rank(), ++expected_rank);
    EXPECT_EQ(callback_count, expected_callback_count);


    // Test decoding with symbol id before setting callback
    coder.decode_symbol(&symbol_data[0], symbol_id);

    EXPECT_EQ(coder.rank(), ++expected_rank);
    EXPECT_EQ(callback_count, expected_callback_count);


    // Set callback handler
    coder.set_rank_changed_callback(
        std::bind(&rank_changed_event, std::ref(coder),
            std::ref(callback_count), std::placeholders::_1)
    );


    // Test decoding with symbol coefficients
    coder.decode_symbol(&symbol_data[0], &symbol_coefficients[0]);

    EXPECT_EQ(coder.rank(), ++expected_rank);
    EXPECT_EQ(callback_count, ++expected_callback_count);

    // Test decoding with symbol id
    coder.decode_symbol(&symbol_data[0], symbol_id);

    EXPECT_EQ(coder.rank(), ++expected_rank);
    EXPECT_EQ(callback_count, ++expected_callback_count);


    // reset callback handler
    coder.reset_rank_changed_callback();


    // Test that callback handler isn't invoked after it has been reset
    coder.decode_symbol(&symbol_data[0], &symbol_coefficients[0]);

    EXPECT_EQ(coder.rank(), ++expected_rank);
    EXPECT_EQ(callback_count, expected_callback_count);
}

// Ensure that the layer complies to the API
void test_rank_callback_decoder_stack(uint32_t symbols, uint32_t symbol_size)
{
    std::vector<uint8_t> symbol_coefficients(symbols, 0);
    std::vector<uint8_t> symbol_data(symbol_size, 0);
    uint32_t symbol_id = 0;

    typedef kodo::rank_callback_decoder_stack<fifi::binary8> rank_coder_t;

    rank_coder_t::factory coder_factory(symbols, symbol_size);

    coder_factory.set_symbols(symbols);
    coder_factory.set_symbol_size(symbol_size);

    auto coder = coder_factory.build();

    coder->decode_symbol(&symbol_data[0], &symbol_coefficients[0]);
    coder->decode_symbol(&symbol_data[0], symbol_id);
}

/// Run the tests
TEST(TestRankCallbackDecoder, test_rank_callback_decoder_stack)
{
    uint32_t symbols = 8;
    uint32_t symbol_size = 8;

    test_rank_callback_decoder_stack(symbols, symbol_size);
    test_rank_callback_decoder_layer(symbols, symbol_size);
}
