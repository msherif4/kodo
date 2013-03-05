// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_coefficient_generator.cpp Unit tests for the coefficient
///       generators

#include <cstdint>

#include <gtest/gtest.h>

#include <fifi/fifi_utils.hpp>

#include <kodo/final_coder_factory.hpp>
#include <kodo/final_coder_factory_pool.hpp>
#include <kodo/coefficient_info.hpp>
#include <kodo/storage_block_info.hpp>
#include <kodo/symbol_storage_tracker.hpp>
#include <kodo/uniform_generator.hpp>
#include <kodo/fake_symbol_storage.hpp>

#include "basic_api_test_helper.hpp"




/// Defines a number test stacks which contains the layers we wish to
/// test.
/// The stacks we define below contain the expect layers used in a
/// typical coefficient storage stack. In addition to this we test both
/// with and without adding the factory pool layer. This layer will
/// recycle the allocated objects.
namespace kodo
{

    // Uniform generator
    template<class Field>
    class uniform_generator_stack
        : public uniform_generator<
                 coefficient_info<
                 symbol_storage_tracker<
                 fake_symbol_storage<
                 storage_block_info<
                 final_coder_factory<
                 uniform_generator_stack<Field>, Field>
                     > > > > >
    {};

    template<class Field>
    class uniform_generator_stack_pool
        : public uniform_generator<
                 coefficient_info<
                 symbol_storage_tracker<
                 fake_symbol_storage<
                 storage_block_info<
                 final_coder_factory_pool<
                 uniform_generator_stack_pool<Field>, Field>
                     > > > > >
    {};

}

/// Tests:
///   - layer::generate(uint8_t*)
///   - layer::generate_partial(uint8_t*)
///   - layer::seed(seed_type)
template<class Coder>
struct api_generate
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::field_type field_type;
    typedef typename Coder::value_type value_type;

    api_generate(uint32_t max_symbols,
                              uint32_t max_symbol_size)
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

            run_partial(symbols, symbol_size);
        }

    void run_once(uint32_t symbols, uint32_t symbol_size)
        {
            pointer_type coder = m_factory.build(symbols, symbol_size);

            std::vector<uint8_t> vector_a =
                random_vector(coder->coefficients_size());

            std::vector<uint8_t> vector_b =
                random_vector(coder->coefficients_size());

            std::vector<uint8_t> vector_c =
                random_vector(coder->coefficients_size());

            std::vector<uint8_t> vector_d =
                random_vector(coder->coefficients_size());

            coder->seed(0);
            coder->generate(&vector_a[0]);
            coder->generate(&vector_b[0]);

            coder->seed(0);
            coder->generate(&vector_c[0]);

            coder->seed(1);
            coder->generate(&vector_d[0]);

            auto storage_a = sak::storage(vector_a);
            auto storage_b = sak::storage(vector_b);
            auto storage_c = sak::storage(vector_c);
            auto storage_d = sak::storage(vector_d);

            EXPECT_FALSE(sak::equal(storage_a,storage_b));
            EXPECT_TRUE(sak::equal(storage_a,storage_c));
            EXPECT_FALSE(sak::equal(storage_a,storage_d));
            EXPECT_FALSE(sak::equal(storage_b,storage_c));
            EXPECT_FALSE(sak::equal(storage_b,storage_d));
            EXPECT_FALSE(sak::equal(storage_c,storage_d));
        }


    void run_partial(uint32_t symbols, uint32_t symbol_size)
        {
            pointer_type coder = m_factory.build(symbols, symbol_size);

            std::vector<uint8_t> vector_a =
                random_vector(coder->coefficients_size());

            std::vector<uint8_t> vector_b =
                random_vector(coder->coefficients_size());

            std::vector<uint8_t> vector_c =
                random_vector(coder->coefficients_size());

            std::vector<uint8_t> vector_d =
                random_vector(coder->coefficients_size());

            coder->seed(0);
            coder->generate_partial(&vector_a[0]);
            coder->generate_partial(&vector_b[0]);

            coder->seed(0);
            coder->generate_partial(&vector_c[0]);

            coder->seed(1);
            coder->generate_partial(&vector_d[0]);

            auto storage_a = sak::storage(vector_a);
            auto storage_b = sak::storage(vector_b);
            auto storage_c = sak::storage(vector_c);
            auto storage_d = sak::storage(vector_d);

            std::vector<uint8_t> zero_vector(coder->coefficients_size(), 0);

            auto zero_storage = sak::storage(zero_vector);

            EXPECT_TRUE(sak::equal(storage_a,zero_storage));
            EXPECT_TRUE(sak::equal(storage_b,zero_storage));
            EXPECT_TRUE(sak::equal(storage_c,zero_storage));
            EXPECT_TRUE(sak::equal(storage_d,zero_storage));

            // std::vector<uint8_t> symbol_a =
            //     random_vector(coder->symbol_size());

            // coder->set_symbol(1, sak::storage(symbol_a));

        }

private:

    // The factory
    factory_type m_factory;

};

/// Run the tests typical coefficients stack
TEST(TestCoefficientGenerator, test_uniform_generator_stack)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // API tests:
    run_test<
        kodo::uniform_generator_stack,
        api_generate>(symbols, symbol_size);

    run_test<
        kodo::uniform_generator_stack_pool,
        api_generate>(symbols, symbol_size);
}

