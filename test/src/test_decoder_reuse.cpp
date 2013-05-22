#include <gtest/gtest.h>
#include <kodo/rlnc/full_vector_codes.hpp>

using namespace kodo;

template<class Field>
class rlnc_encoder :
    public // Payload Codec API
           payload_encoder<
           // Codec Header API
           systematic_encoder<
           symbol_id_encoder<
           // Symbol ID API
           plain_symbol_id_writer<
           // Coefficient Generator API
           uniform_generator<
           // Codec API
           storage_aware_encoder<
           encode_symbol_tracker<
           zero_symbol_encoder<
           linear_block_encoder<
           // Coefficient Storage API
           coefficient_info<
           // Symbol Storage API
           deep_symbol_storage<
           storage_bytes_used<
           storage_block_info<
           // Finite Field API
           finite_field_math<typename fifi::default_field<Field>::type,
           finite_field_info<Field,
           // Factory API
           final_coder_factory_pool<
           // Final type
           rlnc_encoder<Field
               > > > > > > > > > > > > > > > > >
{};


template<class Field>
class rlnc_decoder
    : public // Payload API
             payload_recoder<recoding_stack,
             payload_decoder<
             // Codec Header API
             systematic_decoder<
             symbol_id_decoder<
             // Symbol ID API
             plain_symbol_id_reader<
             // Codec API
             aligned_coefficients_decoder<
             linear_block_decoder<
             // Coefficient Storage API
             coefficient_storage<
             coefficient_info<
             // Storage API
             deep_symbol_storage<
             storage_bytes_used<
             storage_block_info<
             // Finite Field API
             finite_field_math<typename fifi::default_field<Field>::type,
             finite_field_info<Field,
             // Factory API
             final_coder_factory_pool<
             // Final type
             rlnc_decoder<Field>
                 > > > > > > > > > > > > > > >
{};

TEST(TestDecoder, test_decoder_reuse) {
    uint32_t symbols = 32;
    uint32_t symbol_size = 1500;
    bool do_complete;

    rlnc_encoder<fifi::binary8>::factory encoder_factory(symbols, symbol_size);
    rlnc_decoder<fifi::binary8>::factory decoder_factory(symbols, symbol_size);

    srand(time(0));

    // Use factory a lot of times
    for (size_t i = 0; i < 100; i++) {
        // Build coders
        auto encoder = encoder_factory.build();
        auto decoder = decoder_factory.build();

        // Prepare buffers
        std::vector<uint8_t> payload(encoder->payload_size());
        std::vector<uint8_t> data_in(encoder->block_size());

        // Fill with random data
        for (auto &e: data_in)
            e = rand() % 256;

        // Put data in encoder
        encoder->set_symbols(sak::storage(data_in));
    
        if (rand() % 100 > 90)
            do_complete = false;
        else
            do_complete = true;

        // Start encoding/decoding
        while (!decoder->is_complete()) {
            encoder->encode(&payload[0]);

            // Loose a packet with probability
            if (rand() % 100 > 90)
                continue;

            decoder->decode(&payload[0]);

            // Stop decoding after a while with probability
            if (!do_complete && decoder->rank() == symbols - 2)
                break;
        }

        // Check if completed decoders are correct
        if (decoder->is_complete()) {
            std::vector<uint8_t> data_out(decoder->block_size());
            decoder->copy_symbols(sak::storage(data_out));

            ASSERT_TRUE(std::equal(data_out.begin(), data_out.end(), data_in.begin()));
        }
    }
}
