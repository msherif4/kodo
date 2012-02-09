// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#include <gtest/gtest.h>

#include <stdint.h>

#include <kodo/has_block_info.h>
#include <kodo/final_coder_factory.h>
#include <kodo/storage.h>
#include <kodo/symbol_storage_shallow_partial.h>
#include <kodo/symbol_storage_shallow.h>
#include <kodo/symbol_storage_deep.h>

namespace kodo
{

    template<class Field>
    class shallow_const_coder
        : public symbol_storage_shallow_const<
                 has_block_info<
                 final_coder_factory<shallow_const_coder<Field>, Field>
                     > >
    {};

    template<class Field>
    class shallow_mutable_coder
        : public symbol_storage_shallow_mutable<
                 has_block_info<
                 final_coder_factory<shallow_mutable_coder<Field>, Field>
                     > >
    {};

    template<class Field>
    class deep_coder
        : public symbol_storage_deep<
                 has_block_info<
                 final_coder_factory<deep_coder<Field>, Field>
                     > >
    {};

    template<class Field>
    class shallow_partial_coder
        : public symbol_storage_shallow_partial<
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
                
        kodo::set_symbols(kodo::storage(v_in), coder);

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            EXPECT_TRUE( coder->symbol(i) != 0 );
        }

        std::vector<DataType> v_out(vector_size);
        kodo::copy_symbols(kodo::storage(v_out), coder);

        EXPECT_TRUE( std::equal(v_in.begin(), v_in.end(), v_out.begin()));
    }

    {
        std::vector<DataType> v_in(vector_size);

        for(uint32_t i = 0; i < vector_size; ++i)
            v_in[i] = rand_value<DataType>();

        kodo::mutable_storage_sequence storage_sequence =
            kodo::split_storage(kodo::storage(v_in), coder->symbol_size());

        for(uint32_t i = 0; i < storage_sequence.size(); ++i)
        {
            coder->set_symbol(i, storage_sequence[i]);
        }
        
        std::vector<DataType> v_out(vector_size);

        kodo::copy_symbols(kodo::storage(v_out), coder);

        EXPECT_TRUE( std::equal(v_in.begin(), v_in.end(), v_out.begin()));
        
    }
        
}

// Test helper, creates a storage objected based on a pointer
// and a size.
template<class CoderType, class DataType>
void test_pointer_to_data(CoderType coder)
{
    {
        // Make the vector the full size of a coder block
        uint32_t vector_size = coder->block_size() / sizeof(DataType);
        DataType fill_value  = rand_value<DataType>(); 
        
        boost::shared_array<DataType> d_in(new DataType[vector_size]);

        std::fill_n(d_in.get(), vector_size, fill_value);
                
        coder->set_symbols(kodo::storage(d_in.get(), vector_size*sizeof(DataType)));

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            EXPECT_TRUE( coder->symbol(i) != 0 );
        }

        boost::shared_array<DataType> d_out(new DataType[vector_size]);
        coder->copy_symbols(kodo::storage(d_out.get(), vector_size*sizeof(DataType)));

        EXPECT_TRUE( std::equal(d_in.get(), d_in.get() + vector_size, d_out.get()) );
        
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
    srand(time(0));
    
    uint32_t symbols = (rand() % 512) + 1;
    uint32_t symbol_size = ((rand() % 1000) + 1) * 16;

//    uint32_t symbols = 10;
//    uint32_t symbol_size = 32;

    {
        test_coder<kodo::shallow_const_coder<fifi::binary> >(symbols, symbol_size);
        test_coder<kodo::shallow_const_coder<fifi::binary8> >(symbols, symbol_size);
        test_coder<kodo::shallow_const_coder<fifi::binary16> >(symbols, symbol_size);
        
        test_coder<kodo::shallow_mutable_coder<fifi::binary> >(symbols, symbol_size);
        test_coder<kodo::shallow_mutable_coder<fifi::binary8> >(symbols, symbol_size);
        test_coder<kodo::shallow_mutable_coder<fifi::binary16> >(symbols, symbol_size);

        test_coder<kodo::deep_coder<fifi::binary> >(symbols, symbol_size);
        test_coder<kodo::deep_coder<fifi::binary8> >(symbols, symbol_size);
        test_coder<kodo::deep_coder<fifi::binary16> >(symbols, symbol_size);

        test_coder<kodo::shallow_partial_coder<fifi::binary> >(symbols, symbol_size);
        test_coder<kodo::shallow_partial_coder<fifi::binary8> >(symbols, symbol_size);
        test_coder<kodo::shallow_partial_coder<fifi::binary16> >(symbols, symbol_size);

        
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
        
        kodo::set_symbols(kodo::storage(v_in), coder);

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            EXPECT_TRUE( coder->symbol(i) != 0 );
        }

        std::vector<DataType> v_out(vector_length, 0);

        kodo::mutable_storage ms = kodo::storage(v_out);
        
        kodo::copy_symbols(ms, coder);
        
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


// Creates a coder and invokes the test helpers 
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
    srand(time(0));
    
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

