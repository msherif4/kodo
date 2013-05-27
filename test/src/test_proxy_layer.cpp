// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_proxy_layer.cpp Unit tests for the proxy layer class

#include <cstdint>

#include <gtest/gtest.h>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/proxy_layer.hpp>

#include "basic_api_test_helper.hpp"

/// Tests:
///  - That the proxy layer functions are correctly forwarded to the
///    main codec stack.
template<class Coder>
struct api_encoder
{

    typedef typename Coder::factory factory_type;
    typedef typename Coder::pointer pointer_type;
    typedef typename Coder::field_type field_type;
    typedef typename Coder::value_type value_type;

    api_encoder(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_factory(max_symbols, max_symbol_size)
        { }

    void run()
        {

        }

private:

    // The factory
    factory_type m_factory;

};


/// Run the tests typical coefficients stack
TEST(TestProxyLayer, encoder)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // API tests:
    typedef api_encoder<kodo::full_rlnc_encoder<fifi::binary> > test;

    test t(symbols, symbol_size);
    t.run();
}


// /// Run the tests typical coefficients stack
// TEST(TestSymbolId, test_rs_stack)
// {
//     uint32_t symbols = rand_symbols(255);
//     uint32_t symbol_size = rand_symbol_size();

//     // API tests:
//     api_symbol_id
//     <
//         kodo::rs_vandermond_nonsystematic_stack<fifi::binary8>
//     > test(symbols, symbol_size);

//     test.run();

// }

