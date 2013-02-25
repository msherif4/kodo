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

// Creates a random value
template<class DataType>
DataType rand_value()
{
    return rand() % std::numeric_limits<DataType>::max();
}

// Set a std::vector as storage data
template<class CoderType, class DataType>
void test_std_vector(CoderType coder)
{

    // Make the vector the full size of a coder block
    uint32_t vector_size = coder->block_size() / sizeof(DataType);
    DataType fill_value  = rand_value<DataType>();

    // Set all symbols using set_symbols()
    {
        std::vector<DataType> v_in(vector_size, fill_value);

        coder->set_symbols(sak::storage(v_in));

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            EXPECT_TRUE( coder->symbol(i) != 0 );
        }

        std::vector<DataType> v_out(vector_size);
        coder->copy_symbols(sak::storage(v_out));

        EXPECT_TRUE( std::equal(v_in.begin(), v_in.end(), v_out.begin()));
    }

    // Set one symbol at-a-time with layer::set_symbol()
    {
        std::vector<DataType> v_in(vector_size);

        for(uint32_t i = 0; i < vector_size; ++i)
            v_in[i] = rand_value<DataType>();

        auto storage_sequence =
            sak::split_storage(sak::storage(v_in), coder->symbol_size());

        for(uint32_t i = 0; i < storage_sequence.size(); ++i)
        {
            coder->set_symbol(i, storage_sequence[i]);
        }

        std::vector<DataType> v_out(vector_size);

        coder->copy_symbols(sak::storage(v_out));

        EXPECT_TRUE( std::equal(v_in.begin(),
                                v_in.end(),
                                v_out.begin()));

    }

}

// Test helper, creates a storage objected based on a pointer and a size.
template<class CoderType, class DataType>
void test_pointer_to_data(CoderType coder)
{
    {
        // Make the vector the full size of a coder block
        uint32_t vector_size = coder->block_size() / sizeof(DataType);
        DataType fill_value  = rand_value<DataType>();

        boost::shared_array<DataType> d_in(new DataType[vector_size]);

        std::fill_n(d_in.get(), vector_size, fill_value);

        coder->set_symbols(
            sak::storage(d_in.get(), vector_size*sizeof(DataType)));

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            EXPECT_TRUE( coder->symbol(i) != 0 );
        }

        boost::shared_array<DataType> d_out(new DataType[vector_size]);
        coder->copy_symbols(
            sak::storage(d_out.get(), vector_size*sizeof(DataType)));

        EXPECT_TRUE( std::equal(d_in.get(),
                                d_in.get() + vector_size,
                                d_out.get()) );
    }
}

// Creates a coder and invokes the test helpers
template<class Coder>
void test_coder(uint32_t symbols, uint32_t symbol_size)
{

    typename Coder::factory f(symbols, symbol_size);
    typename Coder::pointer coder = f.build(symbols, symbol_size);

    test_std_vector<typename Coder::pointer, char>(coder);
    test_std_vector<typename Coder::pointer, int>(coder);
    test_std_vector<typename Coder::pointer, short>(coder);
    test_std_vector<typename Coder::pointer, uint8_t>(coder);
    test_std_vector<typename Coder::pointer, uint16_t>(coder);
    test_std_vector<typename Coder::pointer, uint32_t>(coder);
    test_std_vector<typename Coder::pointer, uint64_t>(coder);

    test_pointer_to_data<typename Coder::pointer, char>(coder);
    test_pointer_to_data<typename Coder::pointer, int>(coder);
    test_pointer_to_data<typename Coder::pointer, short>(coder);
    test_pointer_to_data<typename Coder::pointer, uint8_t>(coder);
    test_pointer_to_data<typename Coder::pointer, uint16_t>(coder);
    test_pointer_to_data<typename Coder::pointer, uint32_t>(coder);
    test_pointer_to_data<typename Coder::pointer, uint64_t>(coder);
}

/// Tests that the set_symbol works
TEST(TestSymbolStorage, test_set_storage_function)
{
    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = (rand() % 512) + 1;
    uint32_t symbol_size = ((rand() % 1000) + 1) * 16;

    {
        test_coder<kodo::shallow_const_coder<fifi::binary> >(
            symbols, symbol_size);

        test_coder<kodo::shallow_const_coder<fifi::binary8> >(
            symbols, symbol_size);

        test_coder<kodo::shallow_const_coder<fifi::binary16> >(
            symbols, symbol_size);

        test_coder<kodo::shallow_mutable_coder<fifi::binary> >(
            symbols, symbol_size);

        test_coder<kodo::shallow_mutable_coder<fifi::binary8> >(
            symbols, symbol_size);

        test_coder<kodo::shallow_mutable_coder<fifi::binary16> >(
            symbols, symbol_size);

        test_coder<kodo::deep_coder<fifi::binary> >(
            symbols, symbol_size);

        test_coder<kodo::deep_coder<fifi::binary8> >(
            symbols, symbol_size);

        test_coder<kodo::deep_coder<fifi::binary16> >(
            symbols, symbol_size);

        test_coder<kodo::shallow_partial_coder<fifi::binary> >(
            symbols, symbol_size);

        test_coder<kodo::shallow_partial_coder<fifi::binary8> >(
            symbols, symbol_size);

        test_coder<kodo::shallow_partial_coder<fifi::binary16> >(
            symbols, symbol_size);
    }
}



// Set a std::vector as storage data
template<class CoderType, class DataType>
void test_partial_std_vector(CoderType coder)
{
    // Make the vector the full size of a coder block
    uint32_t vector_length = coder->block_size() / sizeof(DataType);

    // Make it partial
    uint32_t partial_vector_length =
        vector_length - (rand() % vector_length);

    DataType fill_value  = rand_value<DataType>();

    {
        std::vector<DataType> v_in(partial_vector_length, fill_value);

        coder->set_symbols(sak::storage(v_in));

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            EXPECT_TRUE( coder->symbol(i) != 0 );
        }

        std::vector<DataType> v_out(vector_length, 0);

        auto ms = sak::storage(v_out);

        coder->copy_symbols(ms);

        // Check whether the beginning of the partial vector
        // is now in the beginning at the output vector
        EXPECT_TRUE( std::equal(v_in.begin(), v_in.end(), v_out.begin()));

        // And that the remaining part is zero
        for(uint32_t i = partial_vector_length; i < vector_length; ++i)
        {
            bool ok = v_out[i] == 0;
            EXPECT_TRUE(ok);
        }
    }

}


/// Creates a coder and invokes the test helpers
template<class Coder>
void test_partial_coder(uint32_t symbols, uint32_t symbol_size)
{
    typename Coder::factory f(symbols, symbol_size);

    {
        typename Coder::pointer coder = f.build(symbols, symbol_size);
        test_partial_std_vector<typename Coder::pointer, char>(coder);
    }
    {
        typename Coder::pointer coder = f.build(symbols, symbol_size);
        test_partial_std_vector<typename Coder::pointer, int>(coder);
    }
    {
        typename Coder::pointer coder = f.build(symbols, symbol_size);
        test_partial_std_vector<typename Coder::pointer, short>(coder);
    }
    {
        typename Coder::pointer coder = f.build(symbols, symbol_size);
        test_partial_std_vector<typename Coder::pointer, uint8_t>(coder);
    }
    {
        typename Coder::pointer coder = f.build(symbols, symbol_size);
        test_partial_std_vector<typename Coder::pointer, uint16_t>(coder);
    }
    {
        typename Coder::pointer coder = f.build(symbols, symbol_size);
        test_partial_std_vector<typename Coder::pointer, uint32_t>(coder);
    }
    {
        typename Coder::pointer coder = f.build(symbols, symbol_size);
        test_partial_std_vector<typename Coder::pointer, uint64_t>(coder);
    }
}

/// Test that setting partial data on a partial shallow storage
/// works
TEST(TestSymbolStorage, test_set_partial_storage_function)
{

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    {
        test_partial_coder<kodo::shallow_partial_coder<fifi::binary> >(
            symbols, symbol_size);

        test_partial_coder<kodo::shallow_partial_coder<fifi::binary8> >(
            symbols, symbol_size);

        test_partial_coder<kodo::shallow_partial_coder<fifi::binary16> >(
            symbols, symbol_size);
    }
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

template<class Coder>
void test_mutable_symbol_access(uint32_t symbols, uint32_t symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(symbols, symbol_size);
    pointer_type coder = factory.build(symbols, symbol_size);

    // We also want to test the call though a const pointer
    const pointer_type & const_coder = coder;

    std::vector<uint8_t> data(coder->block_size());
    coder->set_symbols(sak::storage(data));

    uint8_t *symbol_mutable = coder->symbol(0);
    const uint8_t *symbol_const = const_coder->symbol(0);

    value_type *symbol_value_mutable = coder->symbol_value(0);
    const value_type *symbol_value_const = const_coder->symbol_value(0);

    // Avoid compiler warnings about unused variables
    (void)symbol_mutable;
    (void)symbol_const;
    (void)symbol_value_mutable;
    (void)symbol_value_const;

}

template<template <class> class Coder>
void test_mutable_symbol_access(uint32_t symbols, uint32_t symbol_size)
{
    test_mutable_symbol_access<Coder<fifi::binary> >(
        symbols, symbol_size);

    test_mutable_symbol_access<Coder<fifi::binary8> >(
        symbols, symbol_size);

    test_mutable_symbol_access<Coder<fifi::binary16> >(
        symbols, symbol_size);
}

template<class Coder>
void test_const_symbol_access(uint32_t symbols, uint32_t symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(symbols, symbol_size);
    pointer_type coder = factory.build(symbols, symbol_size);

    // We also want to test the call though a const pointer
    const pointer_type & const_coder = coder;

    std::vector<uint8_t> data(coder->block_size());
    coder->set_symbols(sak::storage(data));

    const uint8_t *symbol_const = coder->symbol(0);
    symbol_const = const_coder->symbol(0);

    const value_type *symbol_value_const = coder->symbol_value(0);
    symbol_value_const = const_coder->symbol_value(0);

    (void)symbol_const;
    (void)symbol_value_const;
}

template<template <class> class Coder>
void test_const_symbol_access(uint32_t symbols, uint32_t symbol_size)
{
    test_const_symbol_access<Coder<fifi::binary> >(
        symbols, symbol_size);

    test_const_symbol_access<Coder<fifi::binary8> >(
        symbols, symbol_size);

    test_const_symbol_access<Coder<fifi::binary16> >(
        symbols, symbol_size);
}



/// Test that the layer::symbol() and layer::symbol_value() functions works
TEST(TestSymbolStorage, test_symbol_access)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_mutable_symbol_access<kodo::shallow_mutable_coder>(
        symbols, symbol_size);

    test_mutable_symbol_access<kodo::deep_coder>(
        symbols, symbol_size);

    test_const_symbol_access<kodo::shallow_const_coder>(
        symbols, symbol_size);

    test_const_symbol_access<kodo::shallow_partial_coder>(
        symbols, symbol_size);
}


template<class Coder>
void test_mutable_swap(uint32_t symbols, uint32_t symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(symbols, symbol_size);
    pointer_type coder = factory.build(symbols, symbol_size);
    const pointer_type &const_coder = coder;

    std::vector<uint8_t> data(coder->block_size());
    coder->swap_symbols(data);

    uint8_t *symbol_mutable = coder->symbol(0);
    const uint8_t *symbol_const = const_coder->symbol(0);

    value_type *symbol_value_mutable = coder->symbol_value(0);
    const value_type *symbol_value_const = const_coder->symbol_value(0);

    // Avoid compiler warnings about unused variables
    (void)symbol_mutable;
    (void)symbol_const;
    (void)symbol_value_mutable;
    (void)symbol_value_const;

}

template<template <class> class Coder>
void test_mutable_swap(uint32_t symbols, uint32_t symbol_size)
{
    test_mutable_swap<Coder<fifi::binary> >(
        symbols, symbol_size);

    test_mutable_swap<Coder<fifi::binary8> >(
        symbols, symbol_size);

    test_mutable_swap<Coder<fifi::binary16> >(
        symbols, symbol_size);
}

template<class Coder>
void test_const_swap(uint32_t symbols, uint32_t symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(symbols, symbol_size);
    pointer_type coder = factory.build(symbols, symbol_size);

    std::vector<uint8_t> data(coder->block_size());
    coder->set_symbols(sak::storage(data));

    const uint8_t *symbol_const = coder->symbol(0);
    const value_type *symbol_value_const = coder->symbol_value(0);

    (void)symbol_const;
    (void)symbol_value_const;
}

template<template <class> class Coder>
void test_const_swap(uint32_t symbols, uint32_t symbol_size)
{
    test_const_swap<Coder<fifi::binary> >(
        symbols, symbol_size);

    test_const_swap<Coder<fifi::binary8> >(
        symbols, symbol_size);

    test_const_swap<Coder<fifi::binary16> >(
        symbols, symbol_size);
}

/// Tests that using layer::copy_symbol() function returns the
/// same data as is stored in the check_storage object.
/// @param coder A pointer to an encoder or decoder
/// @param check_storage Data used for comparison
template<class CoderPointer>
void check_copy_symbol(const CoderPointer &coder,
                       const sak::const_storage &check_storage)
{
    coder->set_symbols(check_storage);

    std::vector<uint8_t> data(coder->symbol_size());

    auto symbol_storage =
        sak::split_storage(check_storage, coder->symbol_size());

    for(uint32_t i = 0; i < coder->symbols(); ++i)
    {
        coder->copy_symbol(i, sak::storage(data));

        EXPECT_TRUE(sak::equal(symbol_storage[i], sak::storage(data)));

    }
}

/// Tests that using layer::copy_symbols() function returns the
/// same data as is stored in the check_storage object.
/// @param coder A pointer to an encoder or decoder
/// @param check_storage Data used for comparison
template<class CoderPointer>
void check_copy_symbols(const CoderPointer &coder,
                        const sak::const_storage &check_storage)
{
    coder->set_symbols(check_storage);

    std::vector<uint8_t> data(coder->block_size());
    coder->copy_symbols(sak::storage(data));

    EXPECT_TRUE(sak::equal(check_storage, sak::storage(data)));
}

/// Tests that using const layer::symbol() layer::symbol_value() function
/// returns the same data as is stored in the check_storage object.
/// @param coder A pointer to an encoder or decoder
/// @param check_storage Data used for comparison
template<class CoderPointer>
void check_symbols(const CoderPointer &coder,
                   const sak::const_storage &check_storage)
{
    std::vector<uint8_t> data(coder->symbol_size());

    auto symbol_storage =
        sak::split_storage(check_storage, coder->symbol_size());

    for(uint32_t i = 0; i < coder->symbols(); ++i)
    {
        auto symbol =
            sak::storage(coder->symbol(i), coder->symbol_size());

        EXPECT_TRUE(sak::equal(symbol_storage[i], symbol));

        auto symbol_value =
            sak::storage(coder->symbol_value(i), coder->symbol_size());

        EXPECT_TRUE(sak::equal(symbol_storage[i], symbol_value));
    }
}

/// Tests that using the layer::set_symbol() function works
/// @param coder A pointer to an encoder or decoder
/// @param check_storage Data used for comparison
template<class CoderPointer>
void check_set_symbol(const CoderPointer &coder,
                      const sak::mutable_storage &check_storage)
{

    auto symbol_storage =
        sak::split_storage(check_storage, coder->symbol_size());

    for(uint32_t i = 0; i < coder->symbols(); ++i)
    {
        coder->set_symbol(i, symbol_storage[i]);
    }

    // Check that the symbols match
    check_symbols(coder, check_storage);
}

/// Tests that using the layer::set_symbols() function works
/// @param coder A pointer to an encoder or decoder
/// @param check_storage Data used for comparison
template<class CoderPointer>
void check_set_symbols(const CoderPointer &coder,
                       const sak::mutable_storage &check_storage)
{
    // Invoke the set_symbols() function
    coder->set_symbols(check_storage);

    // Check that the symbols match
    check_symbols(coder, check_storage);
}

/// Tests that using layer::copy_symbols() function returns the
/// same data as is stored in the check_storage object.
/// @param coder A pointer to an encoder or decoder
/// @param check_storage Data used for comparison
// template<class CoderPointer>
// void check_swap_symbols(const CoderPointer &coder,
//                         const sak::const_storage &check_storage)
// {
//     std::vector<uint8_t> data();
//     coder->swap_symbols(sak::storage(data));

//     EXPECT_TRUE(sak::equal(check_storage, sak::storage(data)));
// }

std::vector<uint8_t> random_vector(uint32_t size)
{
    std::vector<uint8_t> v(size);
    for(uint32_t i = 0; i < v.size(); ++i)
    {
        v[i] = rand() % 255;
    }
    return v;
}

template<class Coder>
void test_deep_swap(uint32_t symbols, uint32_t symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(symbols, symbol_size);
    pointer_type coder = factory.build(symbols, symbol_size);
    const pointer_type &const_coder = coder;

    std::vector<uint8_t> check_data =
        random_vector(coder->block_size());

    auto check_symbols = sak::split_storage(
        sak::storage(check_data), symbol_size);

    // Check that swap works
    std::vector<uint8_t> data = check_data;
    coder->swap_symbols(data);

    EXPECT_TRUE(data.size() == coder->block_size());

    // Check that the various symbol() and symbol_value() functions work
    for(uint32_t i = 0; i < symbols; ++i)
    {
        uint8_t *symbol_mutable = coder->symbol(i);
        EXPECT_TRUE(symbol_mutable != 0);

        const uint8_t *symbol_const = const_coder->symbol(i);
        EXPECT_TRUE(symbol_const != 0);

        value_type *symbol_value_mutable = coder->symbol_value(i);
        EXPECT_TRUE(symbol_value_mutable != 0);

        const value_type *symbol_value_const = const_coder->symbol_value(i);
        EXPECT_TRUE(symbol_value_const != 0);
    }

    // Check that the set_symbols work
    data = check_data;
    coder->set_symbols(sak::storage(data));

    // Check that the set_symbol work
    data = check_data;
    auto split_data = sak::split_storage(sak::storage(data), symbol_size);

    for(uint32_t i = 0; i < symbols; ++i)
    {
        coder->set_symbol(i, split_data[i]);
    }

    // Check that copy_symbols work
    std::vector<uint8_t> copy_symbols_check(coder->block_size());
    coder->copy_symbols(sak::storage(copy_symbols_check));

    // Check that copy_symbol work
    std::vector<uint8_t> copy_symbol_check(coder->block_size());

    split_data = sak::split_storage(
        sak::storage(copy_symbol_check), symbol_size);

    for(uint32_t i = 0; i < symbols; ++i)
    {
        coder->copy_symbol(i, split_data[i]);
    }

}

template<template <class> class Coder>
void test_deep_swap(uint32_t symbols, uint32_t symbol_size)
{
    test_deep_swap<Coder<fifi::binary> >(
        symbols, symbol_size);

    test_deep_swap<Coder<fifi::binary8> >(
        symbols, symbol_size);

    test_deep_swap<Coder<fifi::binary16> >(
        symbols, symbol_size);
}

/// Helper function with for running tests with different fields.
/// The helper expects a template template class Coder which has an
/// unspecified template argument (the finite field or Field) and a
/// template template class Test which expects the final Coder<Field>
/// type.
template<template <class> class Coder, template <class> class Test>
void run_api_test(uint32_t symbols, uint32_t symbol_size)
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


/// Tests:
///   - layer::set_symbols(const sak::mutable_storage&)
///   - layer::copy_symbols(const sak::mutable_storage&)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
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

/// Run the api_copy_symbols() test
TEST(TestSymbolStorage, test_api_copy_symbols)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    run_api_test<kodo::deep_coder, api_copy_symbols>(symbols, symbol_size);
}

/// Tests:
///   - layer::set_symbols(const sak::mutable_storage&)
///   - layer::copy_symbol(uint32_t, const sak::mutable_storage&)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
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

/// Run the api_copy_symbol() test
TEST(TestSymbolStorage, test_api_copy_symbol)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    run_api_test<kodo::deep_coder, api_copy_symbol>(symbols, symbol_size);
}

/// Tests:
///   - layer::symbol(uint32_t index) const
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_symbol_const(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {

        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        // Make sure we call the const version of the function
        const pointer_type &const_coder = coder;

        auto symbols_in = random_vector(coder->block_size());
        coder->set_symbols(sak::storage(symbols_in));

        auto symbols_in_vector =
            sak::split_storage(sak::storage(symbols_in),
                               coder->symbol_size());

        // Check that we correctly copy out the symbols
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            const uint8_t* symbol = const_coder->symbol(i);

            // Compare the storage
            auto s1 = symbols_in_vector[i];
            auto s2 = sak::storage(symbol, coder->symbol_size);
            EXPECT_TRUE(sak::equal(s1, s2));
        }
    }
}


/// Tests:
///   - layer::symbol(uint32_t index)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_symbol(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {

        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        auto symbols_in = random_vector(coder->block_size());
        coder->set_symbols(sak::storage(symbols_in));

        auto symbols_in_vector =
            sak::split_storage(sak::storage(symbols_in),
                               coder->symbol_size());

        // Check that we correctly copy out the symbols
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            uint8_t* symbol = coder->symbol(i);

            // Compare the storage
            auto s1 = symbols_in_vector[i];
            auto s2 = sak::storage(symbol, coder->symbol_size);
            EXPECT_TRUE(sak::equal(s1, s2));
        }
    }
}

/// Tests:
///   - layer::symbol_value(uint32_t index) const
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_symbol_value_const(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {

        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        // Make sure we call the const version of the function
        const pointer_type &const_coder = coder;

        auto symbols_in = random_vector(coder->block_size());
        coder->set_symbols(sak::storage(symbols_in));

        auto symbols_in_vector =
            sak::split_storage(sak::storage(symbols_in),
                               coder->symbol_size());

        // Check that we correctly copy out the symbols
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            const value_type* symbol = const_coder->symbol_value(i);

            // Compare the storage
            auto s1 = symbols_in_vector[i];
            auto s2 = sak::storage(symbol, coder->symbol_size);
            EXPECT_TRUE(sak::equal(s1, s2));
        }
    }
}


/// Tests:
///   - layer::symbol_value(uint32_t index)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_symbol_value(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {

        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        auto symbols_in = random_vector(coder->block_size());
        coder->set_symbols(sak::storage(symbols_in));

        auto symbols_in_vector =
            sak::split_storage(sak::storage(symbols_in),
                               coder->symbol_size());

        // Check that we correctly copy out the symbols
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            value_type* symbol = coder->symbol_value(i);

            // Compare the storage
            auto s1 = symbols_in_vector[i];
            auto s2 = sak::storage(symbol, coder->symbol_size());
            EXPECT_TRUE(sak::equal(s1, s2));
        }
    }
}


/// Tests:
///   - layer::set_symbols(const sak::const_storage)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_set_symbols_const_storage(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        auto vector_in = random_vector(coder->block_size());

        sak::const_storage storage_in = sak::storage(vector_in);
        coder->set_symbols(storage_in);

        auto symbols_in =
            sak::split_storage(storage_in, coder->symbol_size());

        EXPECT_EQ(symbols_in.size(), coder->symbols());

        // Check that we correctly can access the symbols
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            const uint8_t* symbol = coder->symbol(i);

            // Compare the storage
            auto s1 = symbols_in[i];
            auto s2 = sak::storage(symbol, coder->symbol_size());
            EXPECT_TRUE(sak::equal(s1, s2));
        }

    }

}


/// Tests:
///   - layer::set_symbols(const sak::mutable_storage&)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_set_symbols_mutable_storage(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        auto vector_in = random_vector(coder->block_size());
        auto vector_out = random_vector(coder->block_size());

        sak::mutable_storage storage_in = sak::storage(vector_in);
        coder->set_symbols(storage_in);

        auto symbols_in =
            sak::split_storage(storage_in, coder->symbol_size());

        EXPECT_EQ(symbols_in.size(), coder->symbols());

        // Check that we correctly can access the symbols
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            const uint8_t* symbol = coder->symbol(i);

            // Compare the storage
            auto s1 = symbols_in[i];
            auto s2 = sak::storage(symbol, coder->symbol_size());
            EXPECT_TRUE(sak::equal(s1, s2));
        }
    }
}


/// Tests:
///   - layer::set_symbol(uin32_t, const sak::const_storage)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_set_symbol_const_storage(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        auto vector_in = random_vector(coder->block_size());

        sak::const_storage storage_in = sak::storage(vector_in);

        std::vector<sak::const_storage> symbols_in =
            sak::split_storage(storage_in, coder->symbol_size());

        EXPECT_EQ(symbols_in.size(), coder->symbols());

        // Check that we correctly can access the symbols
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            coder->set_symbol(i, symbols_in[i]);
            const uint8_t* symbol = coder->symbol(i);

            // Compare the storage
            auto s1 = symbols_in[i];
            auto s2 = sak::storage(symbol, coder->symbol_size());
            EXPECT_TRUE(sak::equal(s1, s2));
        }

    }

}


/// Tests:
///   - layer::set_symbols(uint32_t, const sak::mutable_storage&)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_set_symbol_mutable_storage(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        auto vector_in = random_vector(coder->block_size());

        sak::mutable_storage storage_in = sak::storage(vector_in);

        std::vector<sak::mutable_storage> symbols_in =
            sak::split_storage(storage_in, coder->symbol_size());

        EXPECT_EQ(symbols_in.size(), coder->symbols());

        // Check that we correctly can access the symbols
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            coder->set_symbol(i, symbols_in[i]);
            const uint8_t* symbol = coder->symbol(i);

            // Compare the storage
            auto s1 = symbols_in[i];
            auto s2 = sak::storage(symbol, coder->symbol_size());
            EXPECT_TRUE(sak::equal(s1, s2));
        }
    }
}

/// Tests:
///   - layer::swap_symbols(std::vector<const uint8_t*>&)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_swap_symbols_const_pointer(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        auto vector_in = random_vector(coder->block_size());
        auto vector_out = random_vector(coder->block_size());

        std::vector<const uint8_t*> symbols;
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            symbols.push_back(&vector_in[i*coder->symbol_size()]);
        }

        coder->swap_symbols(symbols);
        coder->copy_symbols(sak::storage(vector_out));

        EXPECT_TRUE(sak::equal(sak::storage(vector_in),
                               sak::storage(vector_out)));
    }

}

/// Tests:
///   - layer::swap_symbols(std::vector<uint8_t*>&)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_swap_symbols_pointer(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        auto vector_in = random_vector(coder->block_size());
        auto vector_out = random_vector(coder->block_size());

        std::vector<uint8_t*> symbols;
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            symbols.push_back(&vector_in[i*coder->symbol_size()]);
        }

        coder->swap_symbols(symbols);
        coder->copy_symbols(sak::storage(vector_out));

        EXPECT_TRUE(sak::equal(sak::storage(vector_in),
                               sak::storage(vector_out)));
    }

}

/// Tests:
///   - layer::swap_symbols(std::vector<uint8_t>&)
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_swap_symbols_data(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Build with max_symbols and max_symbol_size
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        auto vector_in = random_vector(coder->block_size());
        auto vector_out = random_vector(coder->block_size());

        // Make vector_swap a copy of vector in
        auto vector_swap = vector_in;

        coder->swap_symbols(vector_swap);
        coder->copy_symbols(sak::storage(vector_out));

        EXPECT_TRUE(sak::equal(sak::storage(vector_in),
                               sak::storage(vector_out)));
    }

}

/// Tests:
///   - layer::factory_max_symbols()
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_factory_max_symbols(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);
    EXPECT_EQ(factory.max_symbols(), max_symbols);
}

/// Tests: layer::factory_max_symbol_size()
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_factory_max_symbol_size(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);
    EXPECT_EQ(factory.max_symbol_size(), max_symbol_size);
}

/// Tests:
///   - layer::factory_max_block_size()
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_factory_max_block_size(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);
    EXPECT_EQ(factory.max_block_size(), max_symbols*max_symbol_size);
}

/// Tests:
///   - layer::symbols()
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_symbols(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);

    {
        pointer_type coder = factory.build(max_symbols, max_symbol_size);
        EXPECT_EQ(coder->symbol(), max_symbols);
    }
}

/// Tests:
///   - layer::symbol_size()
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_symbol_size(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);

    {
        pointer_type coder = factory.build(max_symbols, max_symbol_size);
        EXPECT_EQ(coder->symbol_size(), max_symbol_size);
    }
}

/// Tests:
///   - layer::symbol_length()
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_symbol_length(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::field_type field_type;

    factory_type factory(max_symbols, max_symbol_size);

    {
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        uint32_t length =
            fifi::bytes_needed<field_type>(coder->symbol_size());

        EXPECT_EQ(coder->symbol_length(), length);
    }
}

/// Tests:
///   - layer::block_size()
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_block_size(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);

    {
        pointer_type coder = factory.build(max_symbols, max_symbol_size);
        EXPECT_EQ(coder->block_size(), max_symbols * max_symbol_size);
    }
}

/// Tests:
///   - layer::set_bytes_used(uint32_t)
///   - layer::bytes_used()
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void api_test_bytes_used(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    factory_type factory(max_symbols, max_symbol_size);

    {
        pointer_type coder = factory.build(max_symbols, max_symbol_size);
        coder->set_bytes_used(max_symbols*max_symbol_size);

        EXPECT_EQ(coder->bytes_used(), max_symbols * max_symbol_size);
    }
}


/// Runs the unit test for the Storage Layer Shallow mutable API. We
/// try to invoke all APIs defined and assert that the result
/// is as expected.
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void run_test_shallow_mutable_api(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Check shallow mutable
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        // Get some test data
        auto vector = random_vector(coder->block_size());

        coder->set_symbols(sak::storage(vector));

        auto vector_symbols =
            sak::split_storage(sak::storage(vector), coder->symbol_size());

        // Loop over the symbols and check the non const get symbol
        // functions
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            uint8_t *symbol = coder->symbol(i);

            auto symbol_storage =
                sak::storage(symbol, coder->symbol_size());

            EXPECT_TRUE(
                sak::equal(symbol_storage[i], symbol_storage));

            value_type *symbol_value = coder->symbol_value(i);

            auto symbol_value_storage =
                sak::storage(symbol_value, coder->symbol_size());

            EXPECT_TRUE(
                sak::equal(symbol_storage[i], symbol_value_storage));
        }

    }

    // Check the swap function
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        uint8_t* check_ptr =
            reinterpret_cast<uint8_t*>(0xdeadbeefU);

        value_type* check_value_ptr =
            reinterpret_cast<value_type*>(check_ptr);

        std::vector<uint8_t*> swap_vector(max_symbols, check_ptr);

        coder->swap_symbols(swap_vector);

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            uint8_t *symbol = coder->symbol(i);
            EXPECT_EQ(symbol, check_ptr);

            value_type *symbol_value = coder->symbol_value(i);
            EXPECT_EQ(symbol_value, check_value_ptr);
        }
    }
}


/// Runs the unit test for the Storage Layer Shallow const API. We
/// try to invoke all APIs defined and assert that the result
/// is as expected.
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void run_test_shallow_const_api(
    uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    // Get some test data
    auto vector = random_vector(factory.max_block_size());

    sak::const_storage vector_storage = sak::storage(vector);

    auto vector_symbols =
        sak::split_storage(vector_storage, max_symbol_size);


    // Test the set_symbols() function
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);
        coder->set_symbols(vector_storage);

        // Loop over the symbols and check the non const get symbol
        // functions
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            auto symbol_storage =
                sak::storage(coder->symbol(i), coder->symbol_size());

            EXPECT_TRUE(sak::equal(vector_symbols[i], symbol_storage));
        }
    }

    // Test the set_symbol() function
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        // Loop over the symbols and check the non const get symbol
        // functions
        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            coder->set_symbol(i, vector_storage);

            auto symbol_storage =
                sak::storage(coder->symbol(i), coder->symbol_size());

            EXPECT_TRUE(sak::equal(vector_symbols[i], symbol_storage));
        }
    }

    // Test the swap function
    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);

        const uint8_t* check_ptr =
            reinterpret_cast<const uint8_t*>(0xdeadbeefU);

        const value_type* check_value_ptr =
            reinterpret_cast<const value_type*>(check_ptr);

        std::vector<const uint8_t*> swap_vector(max_symbols, check_ptr);

        coder->swap_symbols(swap_vector);

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            const uint8_t *symbol = coder->symbol(i);
            EXPECT_EQ(symbol, check_ptr);

            const value_type *symbol_value = coder->symbol_value(i);
            EXPECT_EQ(symbol_value, check_value_ptr);
        }
    }
}



/// Runs the unit test for the Storage Layer Base API. We
/// try to invoke all APIs defined and assert that the result
/// is as expected.
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<class Coder>
void run_test_base_api(uint32_t max_symbols, uint32_t max_symbol_size)
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::value_type value_type;

    factory_type factory(max_symbols, max_symbol_size);

    EXPECT_EQ(factory.max_symbols(), max_symbols);
    EXPECT_EQ(factory.max_symbol_size(), max_symbol_size);

    {
        // Build with the max_symbols and max_symbol_size
        pointer_type coder = factory.build(max_symbols, max_symbol_size);
        const pointer_type &const_coder = coder;

        EXPECT_EQ(coder->symbols(), max_symbols);
        EXPECT_EQ(coder->symbol_size(), max_symbol_size);
        EXPECT_EQ(const_coder->symbols(), max_symbols);
        EXPECT_EQ(const_coder->symbol_size(), max_symbol_size);
        EXPECT_EQ(const_coder->block_size(), max_symbols*max_symbol_size);

        // Get some test data
        auto vector = random_vector(coder->block_size());
        EXPECT_EQ(coder->bytes_used(), 0U);
        coder->set_bytes_used(max_symbols * max_symbol_size);
        EXPECT_EQ(coder->bytes_used(), max_symbols * max_symbol_size);

        // Check we can set and get the symbols
        check_set_symbols(coder, sak::storage(vector));
        check_set_symbol(coder, sak::storage(vector));
        check_copy_symbols(coder, sak::storage(vector));
        check_copy_symbol(coder, sak::storage(vector));
    }

    {
        // Build with the random symbol and symbol_size
        uint32_t symbols = rand_symbols(max_symbols);
        uint32_t symbol_size = rand_symbol_size(max_symbol_size);

        pointer_type coder = factory.build(symbols, symbol_size);
        const pointer_type &const_coder = coder;

        EXPECT_EQ(coder->symbols(), symbols);
        EXPECT_EQ(coder->symbol_size(), symbol_size);
        EXPECT_EQ(const_coder->symbols(), symbols);
        EXPECT_EQ(const_coder->symbol_size(), symbol_size);
        EXPECT_EQ(const_coder->block_size(), symbols*symbol_size);

        // Get some test data
        auto vector = random_vector(coder->block_size());
        EXPECT_EQ(coder->bytes_used(), 0U);
        coder->set_bytes_used(symbols * symbol_size);
        EXPECT_EQ(coder->bytes_used(), symbols * symbol_size);

        // Check we can set and get the symbols
        check_set_symbols(coder, sak::storage(vector));
        check_set_symbol(coder, sak::storage(vector));
        check_copy_symbols(coder, sak::storage(vector));
        check_copy_symbol(coder, sak::storage(vector));
    }

}

/// Forwards calls to the test_basic_api with the changing the different
/// supported fields.
///
/// @param max_symbols The maximum number of symbols
/// @param max_symbol_size The maximum size of a symbol in bytes
template<template <class> class Coder>
void run_test_base_api(uint32_t symbols, uint32_t symbol_size)
{
    run_test_base_api<Coder<fifi::binary> >(
        symbols, symbol_size);

    run_test_base_api<Coder<fifi::binary8> >(
        symbols, symbol_size);

    run_test_base_api<Coder<fifi::binary16> >(
        symbols, symbol_size);
}

/// The test runner for the Storage Layer Base API
TEST(TestSymbolStorage, test_base_api)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    run_test_base_api<kodo::deep_coder>(
        symbols, symbol_size);

}


/// Test that the layer::swap_symbols() function works
TEST(TestSymbolStorage, test_swap_symbols)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // test_mutable_swap<kodo::shallow_mutable_coder>(
    //     symbols, symbol_size);

    test_deep_swap<kodo::deep_coder>(
        symbols, symbol_size);

    // test_const_swap<kodo::shallow_const_coder>(
    //     symbols, symbol_size);

    // test_const_swap<kodo::shallow_partial_coder>(
    //     symbols, symbol_size);
}



// template<class StorageType>
// void test_arithmetic_helper()
// {
//     uint32_t size = 10000;
//     boost::shared_array<uint8_t> data(new uint8_t[size]);

//     {
//         StorageType s = kodo::storage(data.get(), size);

//         StorageType s1000 = s + 1000;
//         ASSERT_TRUE(s1000.m_data == data.get() + 1000);
//         ASSERT_TRUE(s1000.m_size == 9000);

//         StorageType s9999 = s1000 + 8999;
//         ASSERT_TRUE(s9999.m_data == data.get() + 9999);
//         ASSERT_TRUE(s9999.m_size == 1);

//         StorageType s10000 = s9999 + 1;
//         ASSERT_TRUE(s10000.m_data == data.get() + 10000);
//         ASSERT_TRUE(s10000.m_size == 0);

//     }

// }

// TEST(TestSymbolStorage, test_set_storage_arithmetics)
// {
//     test_arithmetic_helper<kodo::mutable_storage>();
//     test_arithmetic_helper<kodo::const_storage>();
// }

