// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <stdint.h>

#include <gtest/gtest.h>

#include <kodo/has_block_info.hpp>
#include <kodo/final_coder_factory.hpp>
#include <kodo/partial_shallow_symbol_storage.hpp>
#include <kodo/deep_symbol_storage.hpp>
#include <kodo/has_shallow_symbol_storage.hpp>
#include <kodo/has_deep_symbol_storage.hpp>

namespace kodo
{
    template<class Field>
    class shallow_const_coder
        : public const_shallow_symbol_storage<
                 has_block_info<
                 final_coder_factory<shallow_const_coder<Field>, Field>
                     > >
    {};

    template<class Field>
    class shallow_mutable_coder
        : public mutable_shallow_symbol_storage<
                 has_block_info<
                 final_coder_factory<shallow_mutable_coder<Field>, Field>
                     > >
    {};

    template<class Field>
    class deep_coder
        : public deep_symbol_storage<
                 has_block_info<
                 final_coder_factory<deep_coder<Field>, Field>
                     > >
    {};

    template<class Field>
    class shallow_partial_coder
        : public partial_shallow_symbol_storage<
                 has_block_info<
                 final_coder_factory<shallow_partial_coder<Field>, Field>
                     > >
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

// Probably stupid, but just to avoid a warning about comparing
// signed and unsigned values, using EXPECT_EQ(...) otherwise
// we get warnings about usigned and signed comparisons
template<bool is_signed, class DataType>
class expect_zero;

template<class DataType>
class expect_zero<true, DataType>
{
public:

    void operator()(DataType v)
        {
            EXPECT_EQ(v, 0);
        }
};

template<class DataType>
class expect_zero<false, DataType>
{
public:

    void operator()(DataType v)
        {
            EXPECT_EQ(v, 0U);
        }
};


// Set a std::vector as storage data
template<class CoderType, class DataType>
void test_partial_std_vector(CoderType coder)
{
    // Make the vector the full size of a coder block
    uint32_t vector_length = coder->block_size() / sizeof(DataType);

    // Make it partial
    uint32_t partial_vector_length = vector_length - (rand() % vector_length);

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

TEST(TestSymbolStorage, test_set_partial_storage_function)
{
    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = (rand() % 512) + 1;
    uint32_t symbol_size = ((rand() % 1000) + 1) * 16;

    {
        test_partial_coder<kodo::shallow_partial_coder<fifi::binary> >(
            symbols, symbol_size);

        test_partial_coder<kodo::shallow_partial_coder<fifi::binary8> >(
            symbols, symbol_size);

        test_partial_coder<kodo::shallow_partial_coder<fifi::binary16> >(
            symbols, symbol_size);
    }
}


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
void test_mutable(uint32_t symbols, uint32_t symbol_size)
{
    typename Coder::factory factory(symbols, symbol_size);

    auto coder = factory.build(symbols, symbol_size);

    std::vector<uint8_t> data(coder->block_size());
    coder->set_symbols(sak::storage(data));

    uint8_t *symbol_mutable = coder->symbol(0);
    const uint8_t *symbol_const = coder->symbol(0);

    (void)symbol_mutable;
    (void)symbol_const;

}

template<template <class> class Coder>
void test_mutable(uint32_t symbols, uint32_t symbol_size)
{
    test_mutable<Coder<fifi::binary> >(
        symbols, symbol_size);

    test_mutable<Coder<fifi::binary8> >(
        symbols, symbol_size);

    test_mutable<Coder<fifi::binary16> >(
        symbols, symbol_size);
}

template<class Coder>
void test_const(uint32_t symbols, uint32_t symbol_size)
{
    typename Coder::factory factory(symbols, symbol_size);

    auto coder = factory.build(symbols, symbol_size);

    std::vector<uint8_t> data(coder->block_size());
    coder->set_symbols(sak::storage(data));

    const uint8_t *symbol_const = coder->symbol(0);
    (void)symbol_const;
}

template<template <class> class Coder>
void test_const(uint32_t symbols, uint32_t symbol_size)
{
    test_const<Coder<fifi::binary> >(
        symbols, symbol_size);

    test_const<Coder<fifi::binary8> >(
        symbols, symbol_size);

    test_const<Coder<fifi::binary16> >(
        symbols, symbol_size);
}



// // Test that the layer::symbol(uint32_t index) function works
// TEST(TestSymbolStorage, test_shallow_symbol_storage_symbol_access)
// {
//     uint32_t symbols = 10;
//     uint32_t symbol_size = 10;

//     test_mutable<kodo::shallow_mutable_coder>(
//         symbols, symbol_size);

//     test_mutable<kodo::deep_coder>(
//         symbols, symbol_size);

//     test_const<kodo::shallow_const_coder>(
//         symbols, symbol_size);

//     test_const<kodo::shallow_partial_coder>(
//         symbols, symbol_size);
// }



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

