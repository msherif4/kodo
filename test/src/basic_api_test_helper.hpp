// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <gtest/gtest.h>

#include <fifi/is_prime2325.hpp>
#include <fifi/prime2325_binary_search.hpp>
#include <fifi/prime2325_apply_prefix.hpp>

#include <kodo/systematic_encoder.hpp>

/// @param max_value The maximum value to return
/// @return a random number between 1 and max_value
inline uint32_t rand_nonzero(uint32_t max_value = 256)
{
    assert(max_value > 0);
    return (rand() % max_value) + 1;
}

/// @param max_symbols The maximum symbols an encoder or decoder should
///        support.
/// @return a random number up to max_symbols to use in the tests
inline uint32_t rand_symbols(uint32_t max_symbols = 256)
{
    return rand_nonzero(max_symbols);
}

/// Returns a random symbol size. The symbol size has to be chosen as a
/// multiple of finite field elements that we use. E.g using a field 2^16
/// the symbol size must be a multiple of two bytes.
/// Currently the biggest field we support is 2^32 so we just always make
/// sure that the symbol size is a multiple of 4 bytes.
///
/// @param max_symbol_size The maximum symbol size in bytes that we support.
/// @return a random symbol size up to max_symbol_size to use in the tests
inline uint32_t rand_symbol_size(uint32_t max_symbol_size = 1000)
{
    assert(max_symbol_size >= 4);

    uint32_t elements = max_symbol_size / 4;

    uint32_t symbol_size = ((rand() % elements) + 1) * 4;
    assert(symbol_size > 0);
    assert((symbol_size % 4) == 0);

    return symbol_size;
}

/// Helper function used extensively in the tests below. This
/// function returns a std::vector<uint8_t> filled with random data.
/// @param size The size of the vector in bytes
inline std::vector<uint8_t> random_vector(uint32_t size)
{
    std::vector<uint8_t> v(size);
    for(uint32_t i = 0; i < v.size(); ++i)
    {
        v[i] = rand() % 256;
    }

    if(size > 4)
    {
        // The 0xffffffff value is illegal in the prime2325 field
        // so we want to make sure we handle it correctly
        v[0] = 0xffU; v[1] = 0xffU;
        v[2] = 0xffU; v[3] = 0xffU;
    }

    return v;
}

/// Helper function with for running tests with different fields.
/// The helper expects a template template class Stack which has an
/// unspecified template argument (the finite field or Field) and a
/// template template class Test which expects the final Stack<Field>
/// type.
/// @param symbols The number of symbols used in the tests
/// @param symbol_size The size of a symbol in bytes used in the tests
template<template <class> class Stack, template <class> class Test>
inline void run_test(uint32_t symbols, uint32_t symbol_size)
{
    {
        Test<Stack<fifi::binary> > test(symbols, symbol_size);
        test.run();
    }

    {
        Test<Stack<fifi::binary8> > test(symbols, symbol_size);
        test.run();
    }

    {
        Test<Stack<fifi::binary16> > test(symbols, symbol_size);
        test.run();
    }
}


