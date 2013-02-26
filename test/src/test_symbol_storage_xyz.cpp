// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_symbol_storage_xyz.cpp Unit tests for the symbol storage

#include <cstdint>

#include <gtest/gtest.h>

#include <kodo/storage_bytes_used.hpp>
#include <kodo/storage_block_info.hpp>
#include <kodo/final_coder_factory.hpp>
#include <kodo/partial_shallow_symbol_storage.hpp>
#include <kodo/deep_symbol_storage.hpp>
#include <kodo/has_shallow_symbol_storage.hpp>
#include <kodo/has_deep_symbol_storage.hpp>

#include <boost/shared_array.hpp>

#include "basic_api_test_helper.hpp"

namespace kodo
{
    template<class Field>
    class shallow_const_coder
        : public const_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 final_coder_factory<shallow_const_coder<Field>, Field>
                     > > >
    {};

    template<class Field>
    class shallow_mutable_coder
        : public mutable_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 final_coder_factory<shallow_mutable_coder<Field>, Field>
                     > > >
    {};

    template<class Field>
    class deep_coder
        : public deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 final_coder_factory<deep_coder<Field>, Field>
                     > > >
    {};

    template<class Field>
    class shallow_partial_coder
        : public partial_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 final_coder_factory<shallow_partial_coder<Field>, Field>
                     > > >
    {};

}

/// Helper function used extensively in the tests below. This
/// function returns a std::vector<uint8_t> filled with random data.
std::vector<uint8_t> random_vector(uint32_t size)
{
    std::vector<uint8_t> v(size);
    for(uint32_t i = 0; i < v.size(); ++i)
    {
        v[i] = rand() % 255;
    }

    return v;
}

/// Helper function with for running tests with different fields.
/// The helper expects a template template class Coder which has an
/// unspecified template argument (the finite field or Field) and a
/// template template class Test which expects the final Coder<Field>
/// type.
template<template <class> class Coder, template <class> class Test>
void run_test(uint32_t symbols, uint32_t symbol_size)
{
    {
        Test<Coder<fifi::binary> > test;
        test.run(symbols, symbol_size);
    }

    {
        Test<Coder<fifi::binary8> > test;
        test.run(symbols, symbol_size);
    }

    {
        Test<Coder<fifi::binary16> > test;
        test.run(symbols, symbol_size);
    }
}

/// Tests: Setting partial data on a storage object. Any unfilled or partial
///        symbols should be available but their memory zero'ed.
template<class Coder>
struct set_partial_data
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            uint32_t vector_size = rand() % coder->block_size();

            auto vector_in = random_vector(vector_size);

            sak::mutable_storage storage_in = sak::storage(vector_in);
            coder->set_symbols(storage_in);

            auto symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                std::vector<uint8_t> symbol_a(coder->symbol_size(), '\0');
                std::vector<uint8_t> symbol_b(coder->symbol_size(), '\0');

                sak::mutable_storage storage_a = sak::storage(symbol_a);
                sak::mutable_storage storage_b = sak::storage(symbol_b);

                coder->copy_symbol(i, storage_a);
                sak::copy_storage(storage_b, symbols[i]);
                EXPECT_TRUE(sak::equal(storage_a, storage_b));
            }
        }
};

/// Tests:
///   - layer::set_symbols(const sak::mutable_storage&)
///   - layer::copy_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_copy_symbols
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);

            // Build with max_symbols and max_symbol_size
            {
                // Build with the max_symbols and max_symbol_size
                pointer_type coder =
                    factory.build(max_symbols, max_symbol_size);

                auto vector_in = random_vector(coder->block_size());
                auto vector_out = random_vector(coder->block_size());

                sak::mutable_storage storage_in = sak::storage(vector_in);
                sak::mutable_storage storage_out = sak::storage(vector_out);

                coder->set_symbols(storage_in);
                coder->copy_symbols(storage_out);

                EXPECT_TRUE(sak::equal(sak::storage(vector_in),
                                       sak::storage(vector_out)));
            }
        }
};

/// Tests:
///   - layer::set_symbols(const sak::mutable_storage&)
///   - layer::copy_symbol(uint32_t, const sak::mutable_storage&)
template<class Coder>
struct api_copy_symbol
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {

            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());

            sak::mutable_storage storage_in = sak::storage(vector_in);
            coder->set_symbols(storage_in);

            auto symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            // Prepare buffer for a single symbol
            auto vector_out = random_vector(coder->symbol_size());

            // Check that we correctly copy out the symbols
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                sak::mutable_storage symbol_out = sak::storage(vector_out);
                coder->copy_symbol(i, symbol_out);

                EXPECT_TRUE(sak::equal(symbols[i], symbol_out));
            }
        }
};

/// Tests:
///   - layer::symbol(uint32_t) const
///   - layer::set_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_symbol_const
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                factory.build(max_symbols, max_symbol_size);

            // Make sure we call the const version of the function
            const pointer_type &const_coder = coder;

            auto vector_in = random_vector(coder->block_size());

            sak::mutable_storage storage_in = sak::storage(vector_in);
            coder->set_symbols(storage_in);

            auto symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            // Check that we correctly copy out the symbols
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                const uint8_t* symbol = const_coder->symbol(i);

                // Compare the storage
                auto s1 = symbols[i];
                auto s2 = sak::storage(symbol, coder->symbol_size());
                EXPECT_TRUE(sak::equal(s1, s2));
            }
        }
};

/// Tests:
///   - layer::symbol(uint32_t index)
///   - layer::set_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_symbol
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());

            sak::mutable_storage storage_in = sak::storage(vector_in);
            coder->set_symbols(storage_in);

            auto symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            // Check that we correctly copy out the symbols
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                uint8_t* symbol = coder->symbol(i);

                // Compare the storage
                auto s1 = symbols[i];
                auto s2 = sak::storage(symbol, coder->symbol_size());
                EXPECT_TRUE(sak::equal(s1, s2));
            }
        }
};

/// Tests:
///   - layer::symbol_value(uint32_t) const
///   - layer::set_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_symbol_value_const
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {

            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            // Make sure we call the const version of the function
            const pointer_type &const_coder = coder;

            auto vector_in = random_vector(coder->block_size());

            sak::mutable_storage storage_in = sak::storage(vector_in);
            coder->set_symbols(storage_in);

            auto symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            // Check that we correctly copy out the symbols
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                const value_type* symbol = const_coder->symbol_value(i);

                // Compare the storage
                auto s1 = symbols[i];
                auto s2 = sak::storage(symbol, coder->symbol_size());
                EXPECT_TRUE(sak::equal(s1, s2));
            }
        }
};


/// Tests:
///   - layer::symbol_value(uint32_t)
///   - layer::set_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_symbol_value
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());

            sak::mutable_storage storage_in = sak::storage(vector_in);
            coder->set_symbols(storage_in);

            auto symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            // Check that we correctly copy out the symbols
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                value_type* symbol = coder->symbol_value(i);

                // Compare the storage
                auto s1 = symbols[i];
                auto s2 = sak::storage(symbol, coder->symbol_size());
                EXPECT_TRUE(sak::equal(s1, s2));
            }
        }
};

/// Tests:
///   - layer::set_symbols(const sak::const_storage&)
///   - layer::symbol(uint32_t) const
template<class Coder>
struct api_set_symbols_const_storage
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {

            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());

            sak::const_storage storage_in = sak::storage(vector_in);
            coder->set_symbols(storage_in);

            auto symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            EXPECT_EQ(symbols.size(), coder->symbols());

            // Check that we correctly can access the symbols
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                const uint8_t* symbol = coder->symbol(i);

                // Compare the storage
                auto s1 = symbols[i];
                auto s2 = sak::storage(symbol, coder->symbol_size());
                EXPECT_TRUE(sak::equal(s1, s2));
            }
        }
};

/// Tests:
///   - layer::set_symbols(const sak::mutable_storage&)
///   - layer::symbol(uint32_t) const
template<class Coder>
struct api_set_symbols_mutable_storage
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());
            auto vector_out = random_vector(coder->block_size());

            sak::mutable_storage storage_in = sak::storage(vector_in);
            coder->set_symbols(storage_in);

            auto symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            EXPECT_EQ(symbols.size(), coder->symbols());

            // Check that we correctly can access the symbols
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                const uint8_t* symbol = coder->symbol(i);

                // Compare the storage
                auto s1 = symbols[i];
                auto s2 = sak::storage(symbol, coder->symbol_size());
                EXPECT_TRUE(sak::equal(s1, s2));
            }
        }
};

/// Tests:
///   - layer::set_symbol(uint32_t, const sak::const_storage&)
///   - layer::symbol(uint32_t)
template<class Coder>
struct api_set_symbol_const_storage
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());

            sak::const_storage storage_in = sak::storage(vector_in);

            std::vector<sak::const_storage> symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            EXPECT_EQ(symbols.size(), coder->symbols());

            // Check that we correctly can access the symbols
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                coder->set_symbol(i, symbols[i]);
                const uint8_t* symbol = coder->symbol(i);

                // Compare the storage
                auto s1 = symbols[i];
                auto s2 = sak::storage(symbol, coder->symbol_size());
                EXPECT_TRUE(sak::equal(s1, s2));
            }

        }

};

/// Tests:
///   - layer::set_symbols(uint32_t, const sak::mutable_storage&)
///   - layer::symbol(uint32_t)
template<class Coder>
struct api_set_symbol_mutable_storage
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {

            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());

            sak::mutable_storage storage_in = sak::storage(vector_in);

            std::vector<sak::mutable_storage> symbols =
                sak::split_storage(storage_in, coder->symbol_size());

            EXPECT_EQ(symbols.size(), coder->symbols());

            // Check that we correctly can access the symbols
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                coder->set_symbol(i, symbols[i]);
                const uint8_t* symbol = coder->symbol(i);

                // Compare the storage
                auto s1 = symbols[i];
                auto s2 = sak::storage(symbol, coder->symbol_size());
                EXPECT_TRUE(sak::equal(s1, s2));
            }
        }
};

/// Tests:
///   - layer::swap_symbols(std::vector<const uint8_t*>&)
///   - layer::copy_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_swap_symbols_const_pointer
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {

            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());
            auto vector_out = random_vector(coder->block_size());

            sak::mutable_storage storage_out = sak::storage(vector_out);

            std::vector<const uint8_t*> symbols;
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                symbols.push_back(&vector_in[i*coder->symbol_size()]);
            }

            coder->swap_symbols(symbols);
            coder->copy_symbols(storage_out);

            EXPECT_TRUE(sak::equal(sak::storage(vector_in),
                                   sak::storage(vector_out)));
        }
};

/// Tests:
///   - layer::swap_symbols(std::vector<uint8_t*>&)
///   - layer::copy_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_swap_symbols_pointer
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {

            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());
            auto vector_out = random_vector(coder->block_size());

            sak::mutable_storage storage_out = sak::storage(vector_out);

            std::vector<uint8_t*> symbols;
            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                symbols.push_back(&vector_in[i*coder->symbol_size()]);
            }

            coder->swap_symbols(symbols);
            coder->copy_symbols(storage_out);

            EXPECT_TRUE(sak::equal(sak::storage(vector_in),
                                   sak::storage(vector_out)));
        }

};

/// Tests:
///   - layer::swap_symbols(std::vector<uint8_t>&)
///   - layer::copy_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_swap_symbols_data
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);

            // Build with the max_symbols and max_symbol_size
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            auto vector_in = random_vector(coder->block_size());
            auto vector_out = random_vector(coder->block_size());

            sak::mutable_storage storage_out = sak::storage(vector_out);

            // Make vector_swap a copy of vector in
            auto vector_swap = vector_in;

            coder->swap_symbols(vector_swap);
            coder->copy_symbols(storage_out);

            EXPECT_TRUE(sak::equal(sak::storage(vector_in),
                                   sak::storage(vector_out)));
        }

};

/// Tests:
///   - layer::factory_max_symbols()
template<class Coder>
struct api_factory_max_symbols
{
    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            typedef typename Coder::factory factory_type;

            factory_type factory(max_symbols, max_symbol_size);
            EXPECT_EQ(factory.max_symbols(), max_symbols);
        }
};

/// Tests:
///   - layer::factory_max_symbol_size()
template<class Coder>
struct api_factory_max_symbol_size
{
    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            typedef typename Coder::factory factory_type;

            factory_type factory(max_symbols, max_symbol_size);
            EXPECT_EQ(factory.max_symbol_size(), max_symbol_size);
        }
};

/// Tests:
///   - layer::factory_max_block_size()
template<class Coder>
struct api_factory_max_block_size
{
    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            typedef typename Coder::factory factory_type;

            factory_type factory(max_symbols, max_symbol_size);
            EXPECT_EQ(factory.max_block_size(), max_symbols*max_symbol_size);
        }
};

/// Tests:
///   - layer::symbols()
template<class Coder>
struct api_symbols
{
    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            typedef typename Coder::factory factory_type;
            typedef typename Coder::pointer pointer_type;

            factory_type factory(max_symbols, max_symbol_size);
            pointer_type coder = factory.build(max_symbols, max_symbol_size);
            EXPECT_EQ(coder->symbols(), max_symbols);
        }
};

/// Tests:
///   - layer::symbol_size()
template<class Coder>
struct api_symbol_size
{
    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            typedef typename Coder::factory factory_type;
            typedef typename Coder::pointer pointer_type;

            factory_type factory(max_symbols, max_symbol_size);
            pointer_type coder = factory.build(max_symbols, max_symbol_size);
            EXPECT_EQ(coder->symbol_size(), max_symbol_size);
        }
};

/// Tests:
///   - layer::symbol_length()
template<class Coder>
struct api_symbol_length
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::field_type field_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);
            pointer_type coder = factory.build(max_symbols, max_symbol_size);

            uint32_t length =
                fifi::elements_needed<field_type>(coder->symbol_size());

            EXPECT_EQ(coder->symbol_length(), length);
        }
};

/// Tests:
///   - layer::block_size()
template<class Coder>
struct api_block_size
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {
            factory_type factory(max_symbols, max_symbol_size);
            pointer_type coder = factory.build(max_symbols, max_symbol_size);
            EXPECT_EQ(coder->block_size(), max_symbols * max_symbol_size);
        }
};

/// Tests:
///   - layer::set_bytes_used(uint32_t)
///   - layer::bytes_used()
template<class Coder>
struct api_bytes_used
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    void run(uint32_t max_symbols, uint32_t max_symbol_size)
        {

            factory_type factory(max_symbols, max_symbol_size);
            pointer_type coder = factory.build(max_symbols, max_symbol_size);
            coder->set_bytes_used(max_symbols*max_symbol_size);

            EXPECT_EQ(coder->bytes_used(), max_symbols * max_symbol_size);
        }
};

/// Run the api_copy_symbols() test
TEST(TestSymbolStorage, test_api_deep_coder)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    run_test<kodo::deep_coder, api_copy_symbols>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_copy_symbol>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_symbol_const>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_symbol>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_symbol_value_const>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_symbol_value>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_set_symbols_const_storage>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_set_symbols_mutable_storage>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_set_symbol_const_storage>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_set_symbol_mutable_storage>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_swap_symbols_data>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_factory_max_symbols>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_factory_max_symbol_size>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_factory_max_block_size>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_symbols>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_symbol_length>(
        symbols, symbol_size);
    run_test<kodo::deep_coder, api_bytes_used>(
        symbols, symbol_size);


    // run_test<kodo::deep_coder, set_partial_data>(
    //     symbols, symbol_size);

}

/// Test the has_shallow_symbol_storage template
TEST(TestSymbolStorage, test_has_shallow_symbol_storage)
{

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::shallow_partial_coder<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::shallow_partial_coder<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::shallow_partial_coder<fifi::binary16> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::shallow_const_coder<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::shallow_const_coder<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::shallow_const_coder<fifi::binary16> >::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<
                    kodo::deep_coder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<
                    kodo::deep_coder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<
                    kodo::deep_coder<fifi::binary16> >::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<int>::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<fifi::binary8>::value);
}

/// Test the has_deep_symbol_storage template
TEST(TestSymbolStorage, test_has_deep_symbol_storage)
{

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                    kodo::shallow_partial_coder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                    kodo::shallow_partial_coder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                    kodo::shallow_partial_coder<fifi::binary16> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                    kodo::shallow_const_coder<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                    kodo::shallow_const_coder<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                    kodo::shallow_const_coder<fifi::binary16> >::value);

    EXPECT_TRUE(kodo::has_deep_symbol_storage<
                    kodo::deep_coder<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_deep_symbol_storage<
                    kodo::deep_coder<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_deep_symbol_storage<
                    kodo::deep_coder<fifi::binary16> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<int>::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<fifi::binary8>::value);

}


