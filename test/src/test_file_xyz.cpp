// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <stdint.h>

#include <gtest/gtest.h>

#include <kodo/file_encoder.hpp>
#include <kodo/object_decoder.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>

#include <boost/filesystem.hpp>

TEST(TestFileEncoder, test_file_encoder)
{

    {
        // Get a filename
        auto temp_file = boost::filesystem::unique_path();

        std::string encode_filename = "encode-file";
        std::string decode_filename = "decode-file";

        std::cout << "Encode filename " << encode_filename << std::endl;
        
        // Write a test file
        std::ofstream encode_file;
        encode_file.open (encode_filename, std::ios::binary);

        uint32_t size = 500;
        for(uint32_t i = 0; i < size; ++i)
        {
            char c = rand() % 255;
            encode_file.write(&c, 1);
        }

        encode_file.close();

        typedef kodo::full_rlnc_encoder<fifi::binary>
            encoder_t;

        typedef kodo::full_rlnc_decoder<fifi::binary>
            decoder_t;

        typedef kodo::file_encoder<encoder_t>
            file_encoder_t;

        typedef kodo::object_decoder<decoder_t>
            object_decoder_t;

        uint32_t max_symbols = 10;
        uint32_t max_symbol_size = 10;

        file_encoder_t::factory encoder_factory(max_symbols, max_symbol_size);
        file_encoder_t file_encoder(encoder_factory, encode_filename);

        object_decoder_t::factory decoder_factory(max_symbols, max_symbol_size);
        object_decoder_t object_decoder(decoder_factory, size);

        EXPECT_EQ(object_decoder.decoders(), file_encoder.encoders());

        // Open the decode file
        std::ofstream decode_file;
        decode_file.open (decode_filename, std::ios::binary);

        for(uint32_t i = 0; i < file_encoder.encoders(); ++i)
        {
            auto encoder = file_encoder.build(i);
            auto decoder = object_decoder.build(i);

            EXPECT_EQ(encoder->symbols(), decoder->symbols());
            EXPECT_EQ(encoder->symbol_size(), decoder->symbol_size());
            EXPECT_EQ(encoder->bytes_used(), decoder->bytes_used());

            // Set the encoder non-systematic
            if(kodo::is_systematic_encoder(encoder))
                kodo::set_systematic_off(encoder);

            std::vector<uint8_t> payload(encoder->payload_size());

            while( !decoder->is_complete() )
            {
                // Encode a packet into the payload buffer
                encoder->encode( &payload[0] );

                // Pass that packet to the decoder
                decoder->decode( &payload[0] );
            }

            std::vector<uint8_t> data_out(decoder->block_size());
            decoder->copy_symbols(sak::storage(data_out));

            decode_file.write(reinterpret_cast<char*>(&data_out[0]), decoder->bytes_used());

        }

        decode_file.close();

    }

}




