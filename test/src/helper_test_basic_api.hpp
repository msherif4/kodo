// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "basic_api_test_helper.hpp"

template<class Encoder, class Decoder>
inline void test_basic_api(uint32_t symbols, uint32_t symbol_size)
{

    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build();

    typename Decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build();

    EXPECT_TRUE(symbols == encoder_factory.max_symbols());
    EXPECT_TRUE(symbol_size == encoder_factory.max_symbol_size());
    EXPECT_TRUE(symbols == encoder->symbols());
    EXPECT_TRUE(symbol_size == encoder->symbol_size());

    EXPECT_TRUE(symbols == decoder_factory.max_symbols());
    EXPECT_TRUE(symbol_size == decoder_factory.max_symbol_size());
    EXPECT_TRUE(symbols == decoder->symbols());
    EXPECT_TRUE(symbol_size == decoder->symbol_size());

    EXPECT_TRUE(encoder->symbol_length() > 0);
    EXPECT_TRUE(decoder->symbol_length() > 0);

    EXPECT_TRUE(encoder->block_size() == symbols * symbol_size);
    EXPECT_TRUE(decoder->block_size() == symbols * symbol_size);

    EXPECT_TRUE(encoder_factory.max_payload_size() >=
                encoder->payload_size());

    EXPECT_TRUE(decoder_factory.max_payload_size() >=
                decoder->payload_size());

    EXPECT_EQ(encoder_factory.max_payload_size(),
              decoder_factory.max_payload_size());

    // Encode/decode operations
    EXPECT_EQ(encoder->payload_size(), decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());

    std::vector<uint8_t> data_in = random_vector(encoder->block_size());
    std::vector<uint8_t> data_in_copy(data_in);

    sak::mutable_storage storage_in = sak::storage(data_in);
    sak::mutable_storage storage_in_copy = sak::storage(data_in_copy);

    EXPECT_TRUE(sak::equal(storage_in, storage_in_copy));

    // Only used for prime fields, lets reconsider how we implement
    // this less intrusive
    uint32_t prefix = 0;

    if(fifi::is_prime2325<typename Encoder::field_type>::value)
    {
        // This field only works for multiple of uint32_t
        assert((encoder->block_size() % 4) == 0);

        uint32_t block_length = encoder->block_size() / 4;

        fifi::prime2325_binary_search search(block_length);
        prefix = search.find_prefix(storage_in_copy);

        // Apply the negated prefix
        fifi::apply_prefix(storage_in_copy, ~prefix);
    }

    encoder->set_symbols(storage_in_copy);

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

    if(fifi::is_prime2325<typename Encoder::field_type>::value)
    {
        // Now we have to apply the negated prefix to the decoded data
        fifi::apply_prefix(sak::storage(data_out), ~prefix);
    }

    EXPECT_TRUE(std::equal(data_out.begin(),
                           data_out.end(),
                           data_in.begin()));
}

template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_basic_api(uint32_t symbols, uint32_t symbol_size)
{
    test_basic_api
        <
            Encoder<fifi::binary>,
            Decoder<fifi::binary>
        >(symbols, symbol_size);

    test_basic_api
        <
            Encoder<fifi::binary8>,
            Decoder<fifi::binary8>
        >(symbols, symbol_size);

    test_basic_api
        <
            Encoder<fifi::binary16>,
            Decoder<fifi::binary16>
        >(symbols, symbol_size);

    test_basic_api
        <
            Encoder<fifi::prime2325>,
            Decoder<fifi::prime2325>
        >(symbols, symbol_size);
}

template
<
    template <class> class Encoder,
    template <class> class Decoder
>
inline void test_basic_api()
{

    test_basic_api<Encoder, Decoder>(32, 1600);
    test_basic_api<Encoder, Decoder>(1, 1600);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_basic_api<Encoder, Decoder>(symbols, symbol_size);
}

