// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_TEST_AUTO_SRC_BASIC_API_TEST_HELPER
#define KODO_TEST_AUTO_SRC_BASIC_API_TEST_HELPER

#include <gtest/gtest.h>


template<class Encoder, class Decoder>
inline void invoke_basic_api(uint32_t symbols, uint32_t symbol_size)
{
    
    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    typename Encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);
        
    typename Decoder::factory decoder_factory(symbols, symbol_size);
    typename Decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);
    
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

    EXPECT_TRUE(encoder_factory.max_payload_size() >= encoder->payload_size());
    EXPECT_TRUE(decoder_factory.max_payload_size() >= decoder->payload_size());
    EXPECT_EQ(encoder_factory.max_payload_size(), decoder_factory.max_payload_size());
    
    // Encode/decode operations
    EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in(encoder->block_size(), 'a');

    kodo::random_uniform<uint8_t> fill_data;
    fill_data.generate(&data_in[0], data_in.size());
    
    kodo::set_symbols(kodo::storage(data_in), encoder);

    // Set the encoder non-systematic
    encoder->systematic_off();
    
    while( !decoder->is_complete() )
    {
        uint32_t payload_used = encoder->encode( &payload[0] );
        EXPECT_TRUE(payload_used <= encoder->payload_size());
        
        decoder->decode( &payload[0] );
    }
    
    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    kodo::copy_symbols(kodo::storage(data_out), decoder); 
    
    EXPECT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));
}

template<class Encoder, class Decoder>
inline void invoke_out_of_order_raw(uint32_t symbols, uint32_t symbol_size)
{
    
    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    typename Encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);
        
    typename Decoder::factory decoder_factory(symbols, symbol_size);
    typename Decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);

    // Encode/decode operations
    EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in(encoder->block_size());

    kodo::random_uniform<uint8_t> fill_data;
    fill_data.generate(&data_in[0], data_in.size());
    
    kodo::set_symbols(kodo::storage(data_in), encoder);

    encoder->systematic_off();
    
    while( !decoder->is_complete() )
    {

        if((rand() % 100) > 50)
        {
            encoder->encode( &payload[0] );
            decoder->decode( &payload[0] );
        }
        else
        {
            uint32_t symbol_id = rand() % encoder->symbols();
            
            encoder->encode_raw(symbol_id, &payload[0]);
            decoder->decode_raw(symbol_id, &payload[0]);
                
        }
    }

    EXPECT_EQ(encoder->block_size(), decoder->block_size());

    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    kodo::copy_symbols(kodo::storage(data_out), decoder);
        
    EXPECT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));

}




template<class Encoder, class Decoder>
inline void invoke_systematic(uint32_t symbols, uint32_t symbol_size)
{
    
    // Common setting
    typename Encoder::factory encoder_factory(symbols, symbol_size);
    typename Encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);
        
    typename Decoder::factory decoder_factory(symbols, symbol_size);
    typename Decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);

    // Encode/decode operations
    EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

    std::vector<uint8_t> payload(encoder->payload_size());
    std::vector<uint8_t> data_in(encoder->block_size(), 'a');

    kodo::random_uniform<uint8_t> fill_data;
    fill_data.generate(&data_in[0], data_in.size());
    
    kodo::set_symbols(kodo::storage(data_in), encoder);

    // Ensure encoder systematic
    encoder->systematic_on();

    uint32_t pkg_count = 0;
    
    while( !decoder->is_complete() )
    {
        encoder->encode( &payload[0] );
        decoder->decode( &payload[0] );

        ++pkg_count;
    }

    EXPECT_TRUE(pkg_count == encoder->symbols());
    
    std::vector<uint8_t> data_out(decoder->block_size(), '\0');
    kodo::copy_symbols(kodo::storage(data_out), decoder); 
    
    EXPECT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));

    
}




#endif



