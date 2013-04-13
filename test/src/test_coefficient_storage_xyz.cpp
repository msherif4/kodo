// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_coefficient_storage_xyz.cpp Unit tests for the Coefficient
///       Storage API and layers

#include <cstdint>

#include <gtest/gtest.h>


#include <kodo/final_coder_factory.hpp>
#include <kodo/final_coder_factory_pool.hpp>
#include <kodo/finite_field_info.hpp>
#include <kodo/coefficient_storage.hpp>
#include <kodo/coefficient_info.hpp>
#include <kodo/storage_block_info.hpp>

#include "basic_api_test_helper.hpp"

/// Defines a number test stacks which contains the layers we wish to
/// test.
/// The stacks we define below contain the expect layers used in a
/// typical coefficient storage stack. In addition to this we test both
/// with and without adding the factory pool layer. This layer will recycle the
/// allocated objects.
namespace kodo
{

    // Coefficient Storage
    template<class Field>
    class coefficient_storage_stack
        : public coefficient_storage<
                 coefficient_info<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory<
                 coefficient_storage_stack<Field>
                     > > > > >
    {};

    // Coefficient Storage
    template<class Field>
    class coefficient_storage_stack_pool

        : public coefficient_storage<
                 coefficient_info<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory<
                 coefficient_storage_stack_pool<Field>
                     > > > > >
    {};

}

/// Tests:
///   - layer::factory::max_coefficients_size() const
///   - layer::coefficients_size() const
///   - layer::coefficients_length() const
///   - layer::coefficients_value(uint32_t)
///   - layer::coefficients_value(uint32_t) const
///   - layer::coefficients(uint32_t)
///   - layer::coefficients(uint32_t) const
///   - layer::set_coefficients(uint32_t, const sak::const_storage&)
template<class Coder>
struct api_coefficients_storage
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::field_type field_type;

    api_coefficients_storage(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // We invoke the test three times to ensure that if the
            // factory recycles the objects they are safe to reuse
            run_once(m_factory.max_symbols(),
                     m_factory.max_symbol_size());

            run_once(m_factory.max_symbols(),
                     m_factory.max_symbol_size());

            // Build with different from max values
            uint32_t symbols =
                rand_symbols(m_factory.max_symbols());
            uint32_t symbol_size =
                rand_symbol_size(m_factory.max_symbol_size());

            run_once(symbols, symbol_size);
        }

    void run_once(uint32_t symbols, uint32_t symbol_size)
        {
            pointer_type coder = m_factory.build(symbols, symbol_size);

            // Make sure we call the const version of the function
            const pointer_type &const_coder = coder;

            uint32_t max_coefficients_size =
                fifi::elements_to_size<field_type>(m_factory.max_symbols());

            EXPECT_EQ(max_coefficients_size, m_factory.max_coefficients_size());

            uint32_t size =
                fifi::elements_to_size<field_type>(symbols);

            EXPECT_EQ(size, coder->coefficients_size());

            uint32_t length =
                fifi::size_to_length<field_type>(size);

            EXPECT_EQ(length, coder->coefficients_length());

            // Create a zero vector for comparisons
            std::vector<uint8_t> zero_vector(size, '\0');
            auto zero_storage = sak::storage(zero_vector);

            // Everything should be zero initially
            for(uint32_t i = 0; i < symbols; ++i)
            {
                sak::const_storage s;

                s = sak::storage(coder->coefficients(i), size);
                EXPECT_TRUE(sak::equal(zero_storage, s));

                s = sak::storage(const_coder->coefficients(i), size);
                EXPECT_TRUE(sak::equal(zero_storage, s));

                s = sak::storage(coder->coefficients_value(i), size);
                EXPECT_TRUE(sak::equal(zero_storage, s));

                s = sak::storage(const_coder->coefficients_value(i), size);
                EXPECT_TRUE(sak::equal(zero_storage, s));
            }

            // Create some random coefficients, one for every symbol
            uint32_t vector_size = symbols*size;

            auto vector_in = random_vector(vector_size);
            auto storage_in = sak::storage(vector_in);
            auto coefficient_storage =
                sak::split_storage(storage_in, size);

            // Set all the coefficients
            for(uint32_t i = 0; i < symbols; ++i)
            {
                coder->set_coefficients(i, coefficient_storage[i]);
            }

            // Everything should be initialized
            for(uint32_t i = 0; i < symbols; ++i)
            {
                sak::const_storage s1;
                sak::const_storage s2 = coefficient_storage[i];

                s1 = sak::storage(coder->coefficients(i), size);
                EXPECT_TRUE(sak::equal(s1, s2));

                s1 = sak::storage(const_coder->coefficients(i), size);
                EXPECT_TRUE(sak::equal(s1, s2));

                s1 = sak::storage(coder->coefficients_value(i), size);
                EXPECT_TRUE(sak::equal(s1, s2));

                s1 = sak::storage(const_coder->coefficients_value(i), size);
                EXPECT_TRUE(sak::equal(s1, s2));
            }

        }

private:

    // The factory
    factory_type m_factory;

};

/// Run the tests typical coefficients stack
TEST(TestSymbolStorage, test_coefficients_storage_stack)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // API tests:
    run_test<
        kodo::coefficient_storage_stack,
        api_coefficients_storage>(symbols, symbol_size);

    run_test<
        kodo::coefficient_storage_stack_pool,
        api_coefficients_storage>(symbols, symbol_size);

}

