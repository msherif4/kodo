// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <gtest/gtest.h>

#include "basic_api_test_helper.hpp"

/// Helper for the reuse test, ensures that all encoders and decoders
/// produce valid data
template<class Encoder, class Decoder>
inline void
invoke_reuse_helper(Encoder encoder, Decoder decoder)
{
    std::vector<uint8_t> payload(encoder->payload_size());

    std::vector<uint8_t> data_in = random_vector(encoder->block_size());
    sak::mutable_storage storage_in = sak::storage(data_in);

    encoder->set_symbols(storage_in);

    // Set the encoder non-systematic
    if(kodo::is_systematic_encoder(encoder))
        kodo::set_systematic_off(encoder);

    while( !decoder->is_complete() )
    {
        uint32_t payload_used = encoder->encode( &payload[0] );
        EXPECT_TRUE(payload_used <= encoder->payload_size());

        decoder->decode( &payload[0] );
    }

    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    decoder->copy_symbols(sak::storage(data_out));

    EXPECT_TRUE(std::equal(data_out.begin(),
                           data_out.end(),
                           data_in.begin()));

}


/// Test that instantiates a number of encoders and decoders from
/// the same factories
template<class Encoder, class Decoder>
inline void
invoke_reuse(uint32_t symbols, uint32_t symbol_size)
{
    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    typename Decoder::factory decoder_factory(symbols, symbol_size);

    for(uint32_t i = 0; i < 4; ++i)
    {
        uint32_t coders = rand_nonzero(10);

        std::vector<typename Encoder::pointer> encoders;
        std::vector<typename Decoder::pointer> decoders;

        for(uint32_t j = 0; j < coders; ++j)
        {
            encoders.push_back(encoder_factory.build());
            decoders.push_back(decoder_factory.build());

        }

        for(uint32_t j = 0; j < coders; ++j)
        {
            invoke_reuse_helper(encoders[j], decoders[j]);
        }

    }

    return;

    // Test with differing symbols and symbol sizes
    for(uint32_t i = 0; i < 4; ++i)
    {
        uint32_t coders = rand_nonzero(10);

        std::vector<typename Encoder::pointer> encoders;
        std::vector<typename Decoder::pointer> decoders;

        for(uint32_t j = 0; j < coders; ++j)
        {
            uint32_t s = rand_symbols(encoder_factory.max_symbols());
            uint32_t l = rand_symbol_size(encoder_factory.max_symbol_size());

            encoder_factory.set_symbols(s);
            encoder_factory.set_symbol_size(l);
            decoder_factory.set_symbols(s);
            decoder_factory.set_symbol_size(l);

            encoders.push_back(encoder_factory.build());
            decoders.push_back(decoder_factory.build());
        }

        for(uint32_t j = 0; j < coders; ++j)
        {
            invoke_reuse_helper(encoders[j], decoders[j]);
        }

    }

}

/// Test reuse function which will invoke the reuse test for the common
/// field sizes
template
    <
    template <class> class Encoder,
    template <class> class Decoder
    >
inline void test_reuse(uint32_t symbols, uint32_t symbol_size)
{
    invoke_reuse
        <
            Encoder<fifi::binary>,
            Decoder<fifi::binary>
        >(symbols, symbol_size);

    invoke_reuse
        <
            Encoder<fifi::binary8>,
            Decoder<fifi::binary8>
        >(symbols, symbol_size);

    invoke_reuse
        <
            Encoder<fifi::binary16>,
            Decoder<fifi::binary16>
        >(symbols, symbol_size);
}








