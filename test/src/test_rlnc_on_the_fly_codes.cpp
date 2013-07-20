// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing


/// @file test_rlnc_on_the_fly_codes.cpp Unit tests for the full
///       vector codes (i.e. Network Coding encoders and decoders).

#include <ctime>

#include <gtest/gtest.h>

#include <kodo/rlnc/on_the_fly_codes.hpp>

#include "basic_api_test_helper.hpp"

#include "helper_reuse_api.hpp"
#include "helper_recoding_api.hpp"
#include "helper_on_the_fly_api.hpp"

template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_coders(uint32_t symbols, uint32_t symbol_size)
{
    invoke_basic_api
        <
            Encoder<fifi::binary>,
            Decoder<fifi::binary>
        >(symbols, symbol_size);

    invoke_basic_api
        <
            Encoder<fifi::binary8>,
            Decoder<fifi::binary8>
        >(symbols, symbol_size);

    invoke_basic_api
        <
            Encoder<fifi::binary16>,
            Decoder<fifi::binary16>
        >(symbols, symbol_size);

    invoke_basic_api
        <
            Encoder<fifi::prime2325>,
            Decoder<fifi::prime2325>
        >(symbols, symbol_size);
}

inline void test_coders(uint32_t symbols, uint32_t symbol_size)
{
    test_coders<
        kodo::on_the_fly_encoder,
        kodo::on_the_fly_decoder
        >(symbols, symbol_size);
}

/// Tests the basic API functionality this mean basic encoding
/// and decoding
TEST(TestOnTheFlyCodes, basic_api)
{
    test_coders(32, 1600);
    test_coders(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_coders(symbols, symbol_size);
}

template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_initialize(uint32_t symbols, uint32_t symbol_size)
{

    invoke_initialize
        <
            Encoder<fifi::binary>,
            Decoder<fifi::binary>
            >(symbols, symbol_size);

    invoke_initialize
        <
            Encoder<fifi::binary8>,
            Decoder<fifi::binary8>
            >(symbols, symbol_size);

    invoke_initialize
        <
            Encoder<fifi::binary16>,
            Decoder<fifi::binary16>
            >(symbols, symbol_size);
}

inline void test_initialize(uint32_t symbols, uint32_t symbol_size)
{

    test_initialize<
        kodo::on_the_fly_encoder,
        kodo::on_the_fly_decoder>(symbols, symbol_size);

}

/// Test that the encoders and decoders initialize() function can be used
/// to reset the state of an encoder and decoder and that they therefore
/// can be safely reused.
TEST(TestOnTheFlyCodes, initialize_function)
{
    test_initialize(32, 1600);
    test_initialize(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_initialize(symbols, symbol_size);
}

template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_coders_systematic(uint32_t symbols, uint32_t symbol_size)
{
    invoke_systematic<
        Encoder<fifi::binary>,
        Decoder<fifi::binary>
        >(symbols, symbol_size);

    invoke_systematic<
        Encoder<fifi::binary8>,
        Decoder<fifi::binary8>
        >(symbols, symbol_size);

    invoke_systematic<
        Encoder<fifi::binary16>,
        Decoder<fifi::binary16>
        >(symbols, symbol_size);
}


inline void test_coders_systematic(uint32_t symbols, uint32_t symbol_size)
{
    test_coders_systematic
        <
        kodo::on_the_fly_encoder,
        kodo::on_the_fly_decoder
        >(symbols, symbol_size);
}

/// Tests that an encoder producing systematic packets is handled
/// correctly in the decoder.
TEST(TestOnTheFlyCodes, systematic)
{
    test_coders_systematic(32, 1600);
    test_coders_systematic(1, 1600);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_coders_systematic(symbols, symbol_size);
}

template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_coders_raw(uint32_t symbols, uint32_t symbol_size)
{
    invoke_out_of_order_raw<
        Encoder<fifi::binary>,
        Decoder<fifi::binary>
        >(symbols, symbol_size);

    invoke_out_of_order_raw<
        Encoder<fifi::binary8>,
        Decoder<fifi::binary8>
        >(symbols, symbol_size);

    invoke_out_of_order_raw<
        Encoder<fifi::binary16>,
        Decoder<fifi::binary16>
        >(symbols, symbol_size);
}

inline void test_coders_raw(uint32_t symbols, uint32_t symbol_size)
{
    test_coders_raw<
        kodo::on_the_fly_encoder,
        kodo::on_the_fly_decoder
        >(symbols, symbol_size);
}

/// Tests whether mixed un-coded and coded packets are correctly handled
/// in the encoder and decoder.
TEST(TestOnTheFlyCodes, raw)
{
    test_coders_raw(32, 1600);
    test_coders_raw(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_coders_raw(symbols, symbol_size);
}

/// Test the recoding functionality of the stack
TEST(TestOnTheFlyCodes, test_recoders)
{
    test_recoders<kodo::on_the_fly_encoder, kodo::on_the_fly_decoder>();
}


/// Tests that we can progressively set on symbol at-a-time on
/// encoder
TEST(TestOnTheFlyCodes, test_on_the_fly)
{
    test_on_the_fly<kodo::on_the_fly_encoder, kodo::on_the_fly_decoder>();
}

/// Tests that we can progressively set on symbol at-a-time on
/// encoder
TEST(TestOnTheFlyCodes, test_reuse)
{
    test_reuse<kodo::on_the_fly_encoder, kodo::on_the_fly_decoder>();
}

/// Tests that we can progressively set on symbol at-a-time on
/// encoder
TEST(TestOnTheFlyCodes, test_reuse_incomplete)
{
    test_reuse_incomplete<kodo::on_the_fly_encoder, kodo::on_the_fly_decoder>();
}



