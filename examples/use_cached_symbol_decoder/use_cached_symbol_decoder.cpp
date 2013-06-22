// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/cached_symbol_decoder.hpp>
#include <kodo/nada_decoder.hpp>

namespace kodo
{

    /// @brief The symbol info stack can be used to split an
    ///        incoming encoded payload into the encoded
    ///        symbol data and symbol id (in RLNC encoding vector).
    template<class Field>
    class symbol_info_decoder
        : public // Payload API
                 payload_decoder<
                 // Codec Header API
                 systematic_decoder<
                 symbol_id_decoder<
                 // Symbol ID API
                 plain_symbol_id_reader<
                 // Codec API
                 cached_symbol_decoder<
                 nada_decoder<
                 // Coefficient Storage API
                 coefficient_info<
                 // Storage API
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 symbol_info_decoder<Field>
                     > > > > > > > > > > >
    { };

}

/// @example use_cached_symbol_decoder.cpp
///
/// This example shows how to use the cached symbol decoder to "extract"
/// the symbol coding coefficients and the encoded symbol data from an
/// encoded symbol.


int main()
{
    // The finite field we will use in the example. You can try
    // with other fields by specifying e.g. fifi::binary8 for the
    // extension field 2^8
    typedef fifi::binary finite_field;

    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    uint32_t symbols = 8;
    uint32_t symbol_size = 160;

    // Typdefs for the encoder/decoder type we wish to use
    typedef kodo::full_rlnc_encoder<finite_field> rlnc_encoder;
    typedef kodo::full_rlnc_decoder<finite_field> rlnc_decoder;

    typedef kodo::symbol_info_decoder<finite_field> rlnc_info_decoder;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build();

    rlnc_decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build();

    rlnc_info_decoder::factory decoder_info_factory(symbols, symbol_size);
    auto decoder_info = decoder_info_factory.build();

    // Allocate some storage for a "payload" the payload is what we would
    // eventually send over a network
    std::vector<uint8_t> payload(encoder->payload_size());

    // Allocate some data to encode. In this case we make a buffer
    // with the same size as the encoder's block size (the max.
    // amount a single encoder can encode)
    std::vector<uint8_t> data_in(encoder->block_size());

    // Just for fun - fill the data with random data
    for(auto &e: data_in)
        e = rand() % 256;

    // Assign the data buffer to the encoder so that we may start
    // to produce encoded symbols from it
    encoder->set_symbols(sak::storage(data_in));

    while( !decoder->is_complete())
    {
        // Encode a packet into the payload buffer
        encoder->encode( &payload[0] );

        if((rand() % 2) == 0)
            continue;

        decoder_info->decode( &payload[0] );

        if(!decoder_info->cached_symbol_coded())
        {
            std::cout << "Symbol was uncoded, index = "
                      << decoder_info->cached_symbol_index() << std::endl;

            // Pass that packet to the decoder
            decoder->decode_symbol( decoder_info->cached_symbol_data(),
                                    decoder_info->cached_symbol_index());

        }
        else
        {
            std::cout << "Symbol was coded, encoding vector = ";

            const uint8_t* coefficients =
                decoder_info->cached_symbol_coefficients();

            for(uint32_t i = 0; i < decoder_info->symbols(); ++i)
            {
                std::cout << (uint32_t) fifi::get_value<finite_field>(coefficients, i)
                          << " ";
            }

            std::cout << std::endl;

            // Pass that packet to the decoder
            decoder->decode_symbol( decoder_info->cached_symbol_data(),
                                    decoder_info->cached_symbol_coefficients());

        }
    }

    // The decoder is complete, now copy the symbols from the decoder
    std::vector<uint8_t> data_out(decoder->block_size());
    decoder->copy_symbols(sak::storage(data_out));

    // Check we properly decoded the data
    if (std::equal(data_out.begin(), data_out.end(), data_in.begin()))
    {
        std::cout << "Data decoded correctly" << std::endl;
    }
    else
    {
        std::cout << "Unexpected failure to decode "
                  << "please file a bug report :)" << std::endl;
    }
}

