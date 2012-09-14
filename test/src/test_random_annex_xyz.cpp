// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <gtest/gtest.h>

#include <ctime>

#include <kodo/random_annex_base.h>
#include <kodo/random_annex_encoder.h>
#include <kodo/random_annex_decoder.h>
#include <kodo/rfc5052_partitioning_scheme.h>
#include <kodo/generators/random_uniform.h>
#include <kodo/rlnc/full_vector_codes.h>

#include "basic_api_test_helper.h"

//
// Tests the build_annex functionality
//
template<class Partitioning>
inline void invoke_random_annex_base()
{
    Partitioning scheme(16, 1400, 342430);

    kodo::random_annex_base<Partitioning> annex_base;

    annex_base.build_annex(4, scheme);
}




TEST(TestRandomAnnexBase, build_annex)
{
    invoke_random_annex_base<kodo::rfc5052_partitioning_scheme>();
}

//
// Tests the results returned by the max_annex_size function
//
template<class Partitioning>
inline void invoke_max_annex_size(uint32_t max_symbols,
                                  uint32_t max_symbol_size,
                                  uint32_t object_size)
{
    Partitioning scheme(max_symbols, max_symbol_size, object_size);

    uint32_t max_annex = kodo::max_annex_size(
        max_symbols, max_symbol_size, object_size);

    for(uint32_t i = 0; i < scheme.blocks(); ++i)
    {
        uint32_t remaining_symbols = scheme.total_symbols() - scheme.symbols(i);
        ASSERT_TRUE(remaining_symbols >= max_annex);
    }
}


TEST(TestRandomAnnexBase, max_annex)
{
    invoke_max_annex_size<kodo::rfc5052_partitioning_scheme>(1, 1000, 1000);
    invoke_max_annex_size<kodo::rfc5052_partitioning_scheme>(1, 1000, 2000);
    invoke_max_annex_size<kodo::rfc5052_partitioning_scheme>(2, 1000, 1000);
    invoke_max_annex_size<kodo::rfc5052_partitioning_scheme>(1, 10000, 1000);
    invoke_max_annex_size<kodo::rfc5052_partitioning_scheme>(32, 1400, 423432);
    invoke_max_annex_size<kodo::rfc5052_partitioning_scheme>(48, 676, 24768);
}


//
// Tests encoding and decoding with a random annex wrapped
// encoder / decoder
//
template
<
    class Encoder,
    class Decoder,
    class Partitioning
>
void invoke_random_annex_partial(uint32_t max_symbols,
                                 uint32_t max_symbol_size,
                                 uint32_t multiplier)
{

    uint32_t object_size = max_symbols * max_symbol_size * multiplier;
    object_size -= (rand() % object_size);

    uint32_t annex_size = kodo::max_annex_size(max_symbols,
                                               max_symbol_size, object_size);

    if(annex_size > 0)
    {
        // Randomize the actual annex size
        annex_size -= (rand() % annex_size);
    }

    typedef kodo::random_annex_encoder<Encoder, Partitioning>
        random_annex_encoder;

    typedef kodo::random_annex_decoder<Decoder, Partitioning>
        random_annex_decoder;

    std::vector<char> data_in(object_size);
    std::vector<char> data_out(object_size, '\0');

    kodo::random_uniform<char> fill_data;
    fill_data.generate(&data_in[0], data_in.size());

    typename Encoder::factory encoder_factory(max_symbols, max_symbol_size);
    typename Decoder::factory decoder_factory(max_symbols, max_symbol_size);

    //std::cout << "object size " << data_in.size() << std::endl;
    //std::cout << "annex size " << annex_size << std::endl;

    random_annex_encoder obj_encoder(
        annex_size, encoder_factory, kodo::storage(data_in));

    //std::cout << "encoders " << obj_encoder.encoders() << std::endl;

    random_annex_decoder obj_decoder(
        annex_size, decoder_factory, obj_encoder.object_size());

    EXPECT_TRUE(obj_encoder.encoders() >= 1);
    EXPECT_TRUE(obj_decoder.decoders() >= 1);
    EXPECT_TRUE(obj_encoder.encoders() == obj_decoder.decoders());

    uint32_t bytes_used = 0;



    for(uint32_t i = 0; i < obj_encoder.encoders(); ++i)
    {
        typename Encoder::pointer encoder = obj_encoder.build(i);
        typename random_annex_decoder::pointer_type decoder =
            obj_decoder.build(i);

        if(kodo::is_systematic_encoder(encoder))
            kodo::set_systematic_off(encoder);

        EXPECT_TRUE(encoder->block_size() >= encoder->bytes_used());
        EXPECT_TRUE(decoder->block_size() >= decoder->bytes_used());

        EXPECT_TRUE(encoder->block_size() == decoder->block_size());
        EXPECT_TRUE(encoder->bytes_used() == decoder->bytes_used());
        EXPECT_TRUE(encoder->payload_size() == decoder->payload_size());

        std::vector<uint8_t> payload(encoder->payload_size());

        while(!decoder->is_complete())
        {
            encoder->encode( &payload[0] );
            decoder->decode( &payload[0] );
        }

        kodo::mutable_storage storage = kodo::storage(
            &data_out[0] + bytes_used, decoder->bytes_used());

        kodo::copy_symbols(storage, decoder.unwrap());

        bytes_used += decoder->bytes_used();
    }

    EXPECT_EQ(bytes_used, object_size);
    EXPECT_TRUE(std::equal(data_in.begin(), data_in.end(), data_out.begin()));

}



void test_random_annex_coders(uint32_t symbols, uint32_t symbol_size,
                              uint32_t multiplier)
{

    invoke_random_annex_partial<
        kodo::full_rlnc2_encoder,
        kodo::full_rlnc2_decoder,
            kodo::rfc5052_partitioning_scheme>(symbols, symbol_size, multiplier);

    invoke_random_annex_partial<
        kodo::full_rlnc8_encoder,
        kodo::full_rlnc8_decoder,
            kodo::rfc5052_partitioning_scheme>(symbols, symbol_size, multiplier);

    invoke_random_annex_partial<
        kodo::full_rlnc16_encoder,
        kodo::full_rlnc16_decoder,
            kodo::rfc5052_partitioning_scheme>(symbols, symbol_size, multiplier);

}


TEST(TestRandomAnnexCoder, construct_and_invoke_the_basic_api)
{
    test_random_annex_coders(32, 1600, 2);

    srand(static_cast<uint32_t>(time(0)));

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // Multiplies the data to be encoded so that the object encoder
    // is expected to contain multiplier encoders.
    uint32_t multiplier = (rand() % 10) + 1;

    test_random_annex_coders(symbols, symbol_size, multiplier);
}






