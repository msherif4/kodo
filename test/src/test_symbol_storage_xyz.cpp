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
#include <kodo/final_coder_factory_pool.hpp>
#include <kodo/finite_field_info.hpp>
#include <kodo/partial_shallow_symbol_storage.hpp>
#include <kodo/deep_symbol_storage.hpp>
#include <kodo/has_shallow_symbol_storage.hpp>
#include <kodo/has_deep_symbol_storage.hpp>
#include <kodo/fake_symbol_storage.hpp>

#include "basic_api_test_helper.hpp"

/// Defines a number test stacks which contains the layers we wish to
/// test.
/// The stacks we define below contain the expect layers used in a
/// typical storage stack. In addition to this we test both with and
/// without adding the factory pool layer. This layer will recycle the
/// allocated objects.
namespace kodo
{

    // Deep Symbol Storage
    template<class Field>
    class deep_storage_stack
        : public deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory<
                 deep_storage_stack<Field>
                     > > > > >
    {};

    template<class Field>
    class deep_storage_stack_pool
        : public deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory_pool<
                 deep_storage_stack_pool<Field>
                     > > > > >
    {};

    // Mutable Shallow Symbol Storage
    template<class Field>
    class mutable_shallow_stack
        : public mutable_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory<
                 mutable_shallow_stack<Field>
                     > > > > >
    {};

    template<class Field>
    class mutable_shallow_stack_pool
        : public mutable_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory_pool<
                 mutable_shallow_stack_pool<Field>
                     > > > > >
    {};

    // Const Shallow Symbol Storage
    template<class Field>
    class const_shallow_stack
        : public const_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory<
                 const_shallow_stack<Field>
                     > > > > >
    {};

    template<class Field>
    class const_shallow_stack_pool
        : public const_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory_pool<
                 const_shallow_stack_pool<Field>
                     > > > > >
    {};

    // Partial Shallow Symbol Storage
    template<class Field>
    class partial_shallow_stack
        : public partial_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory<
                 partial_shallow_stack<Field>
                     > > > > >
    {};

    template<class Field>
    class partial_shallow_stack_pool
        : public partial_shallow_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 finite_field_info<Field,
                 final_coder_factory_pool<
                 partial_shallow_stack_pool<Field>
                     > > > > >
    {};

}

/// Tests: Setting partial data on a storage object. Any unfilled or partial
///        symbols should be available but their memory zero'ed.
template<class Coder>
struct set_partial_data
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    set_partial_data(uint32_t max_symbols, uint32_t max_symbol_size)
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

            uint32_t vector_size = rand() % coder->block_size();

            // Avoid zero vector size
            vector_size = vector_size ? vector_size : 1;

            auto vector_in = random_vector(vector_size);

            sak::mutable_storage storage_in = sak::storage(vector_in);
            coder->set_symbols(storage_in);

            auto symbol_storage =
                sak::split_storage(storage_in, coder->symbol_size());

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                std::vector<uint8_t> symbol_a(coder->symbol_size(), '\0');
                std::vector<uint8_t> symbol_b(coder->symbol_size(), '\0');

                sak::mutable_storage storage_a = sak::storage(symbol_a);
                sak::mutable_storage storage_b = sak::storage(symbol_b);

                coder->copy_symbol(i, storage_a);

                // If we have symbol in the input data there are three
                // cases:
                // 1) The full symbol is available, so we just copy it to
                //    symbol_b.
                // 2) We had insufficient data to fill an entire symbol.
                //    In this case the storage layer
                //    should have stored a zero padded symbol. Since we
                //    copy symbols to the zero initialized symbol_b
                //    buffer we do not have to do any zero padding.
                // 3) The symbol was not available in the input data. This
                //    should result in a zero initialized symbol.
                //    So comparing directly with symbol_b is OK.
                if(i < symbol_storage.size())
                {
                    // Handles case 1,2
                    sak::copy_storage(storage_b, symbol_storage[i]);
                }

                EXPECT_TRUE(sak::equal(storage_a, storage_b));
            }
        }

private:

    // The factory
    factory_type m_factory;

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

    api_copy_symbols(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            auto vector_in = random_vector(coder->block_size());
            auto vector_out = random_vector(coder->block_size());

            sak::mutable_storage storage_in = sak::storage(vector_in);
            sak::mutable_storage storage_out = sak::storage(vector_out);

            coder->set_symbols(storage_in);
            coder->copy_symbols(storage_out);

            EXPECT_TRUE(sak::equal(sak::storage(vector_in),
                                   sak::storage(vector_out)));
        }

private:

    // The factory
    factory_type m_factory;

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

    api_copy_symbol(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

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

    api_symbol_const(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::symbol(uint32_t index)
///   - layer::set_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_symbol
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_symbol(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

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

    api_symbol_value_const(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }


    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

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

    api_symbol_value(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;


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

    api_set_symbols_const_storage(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

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

    api_set_symbols_mutable_storage(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

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

    api_set_symbol_const_storage(uint32_t max_symbols,
                                 uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

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

    api_set_symbol_mutable_storage(uint32_t max_symbols,
                                   uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::swap_symbols(std::vector<const uint8_t*>&)
///   - layer::copy_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_swap_symbols_const_pointer
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_swap_symbols_const_pointer(uint32_t max_symbols,
                                   uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::swap_symbols(std::vector<uint8_t*>&)
///   - layer::copy_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_swap_symbols_pointer
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_swap_symbols_pointer(uint32_t max_symbols,
                             uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::swap_symbols(std::vector<uint8_t>&)
///   - layer::copy_symbols(const sak::mutable_storage&)
template<class Coder>
struct api_swap_symbols_data
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_swap_symbols_data(uint32_t max_symbols,
                          uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

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

private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::factory_max_symbols()
template<class Coder>
struct api_factory_max_symbols
{
    typedef typename Coder::factory factory_type;

    api_factory_max_symbols(uint32_t max_symbols,
                            uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size), m_max_symbols(max_symbols)
        { }

    void run()
        {
            EXPECT_EQ(m_factory.max_symbols(), m_max_symbols);
        }

private:

    // The factory
    factory_type m_factory;

    // The maximum number of symbols
    uint32_t m_max_symbols;

};

/// Tests:
///   - layer::factory_max_symbol_size()
template<class Coder>
struct api_factory_max_symbol_size
{
    typedef typename Coder::factory factory_type;

    api_factory_max_symbol_size(uint32_t max_symbols,
                                uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size),
          m_max_symbol_size(max_symbol_size)
        { }

    void run()
        {
            EXPECT_EQ(m_factory.max_symbol_size(), m_max_symbol_size);
        }

private:

    // The factory
    factory_type m_factory;

    // The maximum number of symbols
    uint32_t m_max_symbol_size;

};

/// Tests:
///   - layer::factory_max_block_size()
template<class Coder>
struct api_factory_max_block_size
{
    typedef typename Coder::factory factory_type;

    api_factory_max_block_size(uint32_t max_symbols,
                               uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size),
          m_max_block_size(max_symbols*max_symbol_size)
        { }

    void run()
        {
            EXPECT_EQ(m_factory.max_block_size(), m_max_block_size);
        }

private:

    // The factory
    factory_type m_factory;

    // The maximum number of symbols
    uint32_t m_max_block_size;

};

/// Tests:
///   - layer::symbols()
template<class Coder>
struct api_symbols
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_symbols(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbols(), m_factory.max_symbols());
        }

private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::symbol_size()
template<class Coder>
struct api_symbol_size
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_symbol_size(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbol_size(), m_factory.max_symbol_size());
        }

private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::symbol_length()
template<class Coder>
struct api_symbol_length
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::field_type field_type;

    api_symbol_length(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            uint32_t length =
                fifi::elements_needed<field_type>(coder->symbol_size());

            EXPECT_EQ(coder->symbol_length(), length);
        }
private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::block_size()
template<class Coder>
struct api_block_size
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_block_size(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            EXPECT_EQ(coder->block_size(),
                      m_factory.max_symbols() * m_factory.max_symbol_size());
        }

private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::set_bytes_used(uint32_t)
///   - layer::bytes_used()
template<class Coder>
struct api_bytes_used
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_bytes_used(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            uint32_t used =
                m_factory.max_symbols() * m_factory.max_symbol_size();

            coder->set_bytes_used(used);
            EXPECT_EQ(coder->bytes_used(), used);

            coder->set_bytes_used(1U);
            EXPECT_EQ(coder->bytes_used(), 1U);

        }

private:

    // The factory
    factory_type m_factory;

};

/// Tests:
///   - layer::symbol_exists(uint32_t) const
///   - layer::symbol_count() const
///   - layer::is_storage_full() const
template<class Coder>
struct api_storage_status
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_storage_status(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size),
          m_factory_fixed(10, 100)
        { }

    void run()
        {
            set_symbol();
            set_symbols();
        }

    /// Using:
    ///   - layer::set_symbol(uint32_t, const sak::mutable_storage&)
    void set_symbol()
        {
            // Build with the max_symbols and max_symbol_size
            pointer_type coder =
                m_factory_fixed.build(10, 100);

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                EXPECT_FALSE(coder->symbol_exists(i));
            }

            EXPECT_EQ(coder->symbol_count(), 0U);
            EXPECT_FALSE(coder->is_storage_full());

            // Set some symbols
            auto vector_in = random_vector(coder->symbol_size());

            std::set<uint32_t> indexes;

            sak::mutable_storage s = sak::storage(vector_in);

            coder->set_symbol(2, s);
            coder->set_symbol(7, s);
            coder->set_symbol(1, s);
            coder->set_symbol(8, s);

            indexes.insert(2);
            indexes.insert(7);
            indexes.insert(1);
            indexes.insert(8);

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                auto it = indexes.find(i);
                if(it != indexes.end())
                {
                    EXPECT_TRUE(coder->symbol_exists(i));
                }
                else
                {
                    EXPECT_FALSE(coder->symbol_exists(i));
                }
            }

            EXPECT_EQ(coder->symbol_count(), indexes.size());
            EXPECT_FALSE(coder->is_storage_full());


        }

    /// Using:
    ///   - layer::set_symbols(const sak::mutable_storage&)
    void set_symbols()
        {
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbol_count(), 0U);
            EXPECT_FALSE(coder->is_storage_full());

            std::vector<uint8_t> vector_data =
                random_vector(coder->block_size());

            sak::mutable_storage s = sak::storage(vector_data);

            coder->set_symbols(s);

            EXPECT_EQ(coder->symbol_count(), coder->symbols());
            EXPECT_TRUE(coder->is_storage_full());

            coder = m_factory.build(m_factory.max_symbols(),
                                    m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbol_count(), 0U);
            EXPECT_FALSE(coder->is_storage_full());

            coder->set_symbols(s);

            EXPECT_EQ(coder->symbol_count(), coder->symbols());
            EXPECT_TRUE(coder->is_storage_full());
        }

private:

    // The factory
    factory_type m_factory;

    // Factory with fixed max_symbol_size and max_symbols
    factory_type m_factory_fixed;

};

/// Tests:
///   - layer::symbol_exists(uint32_t) const
///   - layer::symbol_count() const
///   - layer::is_storage_full() const
template<class Coder>
struct api_deep_swap_storage_status
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_deep_swap_storage_status(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            swap_symbols();
        }

    /// Using:
    ///   - layer::swap_symbols(std::vector<uint8_t>&)
    void swap_symbols()
        {
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbol_count(), 0U);
            EXPECT_FALSE(coder->is_storage_full());

            std::vector<uint8_t> vector_data =
                random_vector(coder->block_size());

            coder->swap_symbols(vector_data);

            EXPECT_EQ(coder->symbol_count(), coder->symbols());
            EXPECT_TRUE(coder->is_storage_full());

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                EXPECT_TRUE(coder->symbol_exists(i));
            }

            coder = m_factory.build(m_factory.max_symbols(),
                                    m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbol_count(), 0U);
            EXPECT_FALSE(coder->is_storage_full());

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                EXPECT_FALSE(coder->symbol_exists(i));
            }


        }

private:

    // The factory
    factory_type m_factory;

};


/// Tests:
///   - layer::symbol_exists(uint32_t) const
///   - layer::symbol_count() const
///   - layer::is_storage_full() const
template<class Coder>
struct api_const_shallow_swap_storage_status
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_const_shallow_swap_storage_status(uint32_t max_symbols,
                                          uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            swap_symbols();
        }

    /// Using:
    ///   - layer::swap_symbols(std::vector<const uint8_t*>&)
    void swap_symbols()
        {
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbol_count(), 0U);
            EXPECT_FALSE(coder->is_storage_full());

            // Make a temp. vector with some dummy pointers
            std::vector<const uint8_t*> vector_const_ptr(
                m_factory.max_symbols(), (const uint8_t*)1U);

            coder->swap_symbols(vector_const_ptr);

            EXPECT_EQ(coder->symbol_count(), coder->symbols());
            EXPECT_TRUE(coder->is_storage_full());

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                EXPECT_TRUE(coder->symbol_exists(i));
            }

            coder = m_factory.build(m_factory.max_symbols(),
                                    m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbol_count(), 0U);
            EXPECT_FALSE(coder->is_storage_full());

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                EXPECT_FALSE(coder->symbol_exists(i));
            }
        }

private:

    // The factory
    factory_type m_factory;

};


/// Tests:
///   - layer::symbol_exists(uint32_t) const
///   - layer::symbol_count() const
///   - layer::is_storage_full() const
template<class Coder>
struct api_mutable_shallow_swap_storage_status
{
    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;

    api_mutable_shallow_swap_storage_status(uint32_t max_symbols,
                                          uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {
            swap_symbols();
        }

    /// Using:
    ///   - layer::swap_symbols(std::vector<uint8_t*>&)
    void swap_symbols()
        {
            pointer_type coder =
                m_factory.build(m_factory.max_symbols(),
                                m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbol_count(), 0U);
            EXPECT_FALSE(coder->is_storage_full());

            // Make a temp. vector with some dummy pointers
            std::vector<uint8_t*> vector_ptr(
                m_factory.max_symbols(), (uint8_t*)1U);

            coder->swap_symbols(vector_ptr);

            EXPECT_EQ(coder->symbol_count(), coder->symbols());
            EXPECT_TRUE(coder->is_storage_full());

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                EXPECT_TRUE(coder->symbol_exists(i));
            }

            coder = m_factory.build(m_factory.max_symbols(),
                                    m_factory.max_symbol_size());

            EXPECT_EQ(coder->symbol_count(), 0U);
            EXPECT_FALSE(coder->is_storage_full());

            for(uint32_t i = 0; i < coder->symbols(); ++i)
            {
                EXPECT_FALSE(coder->symbol_exists(i));
            }
        }

private:

    // The factory
    factory_type m_factory;

};



/// Helper function for running all the API and related tests
/// which are compatible with the deep stack.
template<template <class> class Stack>
void run_deep_stack_tests()
{

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // API tests:
    run_test<Stack, api_copy_symbols>(
        symbols, symbol_size);
    run_test<Stack, api_copy_symbol>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_const>(
        symbols, symbol_size);
    run_test<Stack, api_symbol>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_value_const>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_value>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbols_const_storage>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbols_mutable_storage>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbol_const_storage>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbol_mutable_storage>(
        symbols, symbol_size);
    run_test<Stack, api_swap_symbols_data>(
        symbols, symbol_size);
    run_test<Stack, api_factory_max_symbols>(
        symbols, symbol_size);
    run_test<Stack, api_factory_max_symbol_size>(
        symbols, symbol_size);
    run_test<Stack, api_factory_max_block_size>(
        symbols, symbol_size);
    run_test<Stack, api_symbols>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_length>(
        symbols, symbol_size);
    run_test<Stack, api_block_size>(
        symbols, symbol_size);
    run_test<Stack, api_bytes_used>(
        symbols, symbol_size);
    run_test<Stack, api_storage_status>(
        symbols, symbol_size);
    run_test<Stack, api_deep_swap_storage_status>(
        symbols, symbol_size);



    // Other tests
    run_test<Stack, set_partial_data>(
        symbols, symbol_size);

}

/// Run the tests typical deep_storage stack
TEST(TestSymbolStorage, test_deep_stack)
{
    run_deep_stack_tests<kodo::deep_storage_stack>();
    run_deep_stack_tests<kodo::deep_storage_stack_pool>();
}

/// Helper function for running all the API and related tests
/// which are compatible with the shallow const stack.
template<template <class> class Stack>
void run_const_shallow_stack_tests()
{

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // API tests:
    run_test<Stack, api_copy_symbols>(
        symbols, symbol_size);
    run_test<Stack, api_copy_symbol>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_const>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_value_const>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbols_const_storage>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbols_mutable_storage>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbol_const_storage>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbol_mutable_storage>(
        symbols, symbol_size);
    run_test<Stack, api_swap_symbols_const_pointer>(
        symbols, symbol_size);
    run_test<Stack, api_factory_max_symbols>(
        symbols, symbol_size);
    run_test<Stack, api_factory_max_symbol_size>(
        symbols, symbol_size);
    run_test<Stack, api_factory_max_block_size>(
        symbols, symbol_size);
    run_test<Stack, api_symbols>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_length>(
        symbols, symbol_size);
    run_test<Stack, api_block_size>(
        symbols, symbol_size);
    run_test<Stack, api_bytes_used>(
        symbols, symbol_size);
    run_test<Stack, api_storage_status>(
        symbols, symbol_size);
    run_test<Stack, api_const_shallow_swap_storage_status>(
        symbols, symbol_size);

}

/// Run the tests typical const shallow stack
TEST(TestSymbolStorage, test_const_shallow_stack)
{
    run_const_shallow_stack_tests<kodo::const_shallow_stack>();
    run_const_shallow_stack_tests<kodo::const_shallow_stack_pool>();
}

/// Run the tests typical partial shallow stack
TEST(TestSymbolStorage, test_partial_shallow_stack)
{
    // The partial shallow symbol stack is API compatible with the
    // const shallow stack
    run_const_shallow_stack_tests<kodo::partial_shallow_stack>();
    run_const_shallow_stack_tests<kodo::partial_shallow_stack_pool>();

    // Run the partial data tests
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    run_test<kodo::partial_shallow_stack, set_partial_data>(
        symbols, symbol_size);

    run_test<kodo::partial_shallow_stack_pool, set_partial_data>(
        symbols, symbol_size);
}

/// Helper function for running all the API and related tests
/// which are compatible with the shallow const stack.
template<template <class> class Stack>
void run_mutable_shallow_stack_tests()
{

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // API tests:
    run_test<Stack, api_copy_symbols>(
        symbols, symbol_size);
    run_test<Stack, api_copy_symbol>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_const>(
        symbols, symbol_size);
    run_test<Stack, api_symbol>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_value_const>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_value>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbols_mutable_storage>(
        symbols, symbol_size);
    run_test<Stack, api_set_symbol_mutable_storage>(
        symbols, symbol_size);
    run_test<Stack, api_swap_symbols_pointer>(
        symbols, symbol_size);
    run_test<Stack, api_factory_max_symbols>(
        symbols, symbol_size);
    run_test<Stack, api_factory_max_symbol_size>(
        symbols, symbol_size);
    run_test<Stack, api_factory_max_block_size>(
        symbols, symbol_size);
    run_test<Stack, api_symbols>(
        symbols, symbol_size);
    run_test<Stack, api_symbol_length>(
        symbols, symbol_size);
    run_test<Stack, api_block_size>(
        symbols, symbol_size);
    run_test<Stack, api_bytes_used>(
        symbols, symbol_size);
    run_test<Stack, api_storage_status>(
        symbols, symbol_size);
    run_test<Stack, api_mutable_shallow_swap_storage_status>(
        symbols, symbol_size);


}

/// Run the tests typical const shallow stack
TEST(TestSymbolStorage, test_mutable_shallow_stack)
{
    run_mutable_shallow_stack_tests<kodo::mutable_shallow_stack>();
    run_mutable_shallow_stack_tests<kodo::mutable_shallow_stack_pool>();
}

/// Tests the has_shallow_symbol_storage template
TEST(TestSymbolStorage, test_has_shallow_symbol_storage)
{
    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::partial_shallow_stack<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::partial_shallow_stack<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::partial_shallow_stack<fifi::binary16> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::const_shallow_stack<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::const_shallow_stack<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::const_shallow_stack<fifi::binary16> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::mutable_shallow_stack<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::mutable_shallow_stack<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_shallow_symbol_storage<
                    kodo::mutable_shallow_stack<fifi::binary16> >::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<
                     kodo::deep_storage_stack<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<
                     kodo::deep_storage_stack<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<
                     kodo::deep_storage_stack<fifi::binary16> >::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<int>::value);

    EXPECT_FALSE(kodo::has_shallow_symbol_storage<fifi::binary8>::value);

    /// has_mutable_shallow_symbol_storage

    EXPECT_FALSE(kodo::has_mutable_shallow_symbol_storage<
                    kodo::const_shallow_stack<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_mutable_shallow_symbol_storage<
                    kodo::const_shallow_stack<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_mutable_shallow_symbol_storage<
                    kodo::const_shallow_stack<fifi::binary16> >::value);

    EXPECT_TRUE(kodo::has_mutable_shallow_symbol_storage<
                    kodo::mutable_shallow_stack<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_mutable_shallow_symbol_storage<
                    kodo::mutable_shallow_stack<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_mutable_shallow_symbol_storage<
                    kodo::mutable_shallow_stack<fifi::binary16> >::value);

    // has_const_shallow_symbol_storage

    EXPECT_FALSE(kodo::has_const_shallow_symbol_storage<
                    kodo::mutable_shallow_stack<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_const_shallow_symbol_storage<
                    kodo::mutable_shallow_stack<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_const_shallow_symbol_storage<
                    kodo::mutable_shallow_stack<fifi::binary16> >::value);

    EXPECT_TRUE(kodo::has_const_shallow_symbol_storage<
                    kodo::const_shallow_stack<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_const_shallow_symbol_storage<
                    kodo::const_shallow_stack<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_const_shallow_symbol_storage<
                    kodo::const_shallow_stack<fifi::binary16> >::value);

}

/// Tests the has_deep_symbol_storage template
TEST(TestSymbolStorage, test_has_deep_symbol_storage)
{
    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                     kodo::partial_shallow_stack<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                     kodo::partial_shallow_stack<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                     kodo::partial_shallow_stack<fifi::binary16> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                     kodo::const_shallow_stack<fifi::binary> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                     kodo::const_shallow_stack<fifi::binary8> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<
                     kodo::const_shallow_stack<fifi::binary16> >::value);

    EXPECT_TRUE(kodo::has_deep_symbol_storage<
                    kodo::deep_storage_stack<fifi::binary> >::value);

    EXPECT_TRUE(kodo::has_deep_symbol_storage<
                    kodo::deep_storage_stack<fifi::binary8> >::value);

    EXPECT_TRUE(kodo::has_deep_symbol_storage<
                    kodo::deep_storage_stack<fifi::binary16> >::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<int>::value);

    EXPECT_FALSE(kodo::has_deep_symbol_storage<fifi::binary8>::value);
}



