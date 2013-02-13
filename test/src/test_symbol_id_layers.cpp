// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <gtest/gtest.h>

#include <kodo/final_coder_factory.hpp>
#include <kodo/random_uniform_symbol_id.hpp>

#include "basic_api_test_helper.hpp"

namespace kodo
{

    template<class Field>
    class test_random_uniform_symbol_id
        : public random_uniform_symbol_id<
                 final_coder_factory<
                     test_random_uniform_symbol_id<Field>, Field>
                 >
    { };

}

template<class SymbolIdLayer>
void test_symbol_id_layer(uint32_t symbols, uint32_t symbol_size)
{
    typename SymbolIdLayer::factory f(symbols, symbol_size);
    EXPECT_TRUE(f.max_id_size() > 0);

    auto layer = f.build(symbols, symbol_size);
    EXPECT_TRUE(layer->id_size());
    EXPECT_TRUE(f.max_id_size() >= layer->id_size());

    std::vector<uint8_t> symbol_id(layer->id_size());
    uint8_t *symbol_id_coefficients = 0;

    layer->write_id(&symbol_id[0], &symbol_id_coefficients);

    EXPECT_TRUE(symbol_id_coefficients != 0);

    typedef typename SymbolIdLayer::field_type field_type;
    typedef typename field_type::value_type value_type;

    value_type *c = reinterpret_cast<value_type*>(symbol_id_coefficients);

    for(uint32_t i = 0; i < symbols; ++i)
    {
        typename field_type::order_type v = fifi::get_value<field_type>(c, i);

        EXPECT_TRUE(v <= field_type::max_value);

        // Since values are unsigned and min_value of all field types are zero
        // v can never be less than - so we don't bother testing for it.
    }

}


template<template <class Field> class SymbolIdLayer>
void test_symbol_id_layer(uint32_t symbols, uint32_t symbol_size)
{
    test_symbol_id_layer<SymbolIdLayer<fifi::binary> >(symbols, symbol_size);
    test_symbol_id_layer<SymbolIdLayer<fifi::binary8> >(symbols, symbol_size);
    test_symbol_id_layer<SymbolIdLayer<fifi::binary16> >(symbols, symbol_size);
    test_symbol_id_layer<SymbolIdLayer<fifi::prime2325> >(symbols, symbol_size);
}


TEST(TestSymbolIdLayers, test_random_uniform_symbol_id)
{
    test_symbol_id_layer<kodo::test_random_uniform_symbol_id>(10,10);
    test_symbol_id_layer<kodo::test_random_uniform_symbol_id>(32,1600);
    test_symbol_id_layer<kodo::test_random_uniform_symbol_id>(1,1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_symbol_id_layer<kodo::test_random_uniform_symbol_id>(
        symbols, symbol_size);
}




