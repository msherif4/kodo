// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo/file_encoder.hpp>
#include <kodo/shallow_storage_decoder.hpp>
#include <kodo/storage_encoder.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/partial_shallow_symbol_storage.hpp>

/// @example encode_decode_storage.cpp
///
/// Often we want to encode / decode data that exceed a single
/// encoding/decoding block. In this case we need to "chop" up
/// the data into manageable chunks and then encode and decode
/// each chuck separately. This examples shows how to use the
/// storage encoder and decoder in Kodo.
///
/// Both the encoder and decoder uses a shallow storage which means
/// that they operate directly on the memory provided.

namespace kodo
{

    /// Encoder stack using a partial shallow storage class
    template<class Field>
    class shallow_rlnc_encoder :
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
               partial_shallow_symbol_storage<
               storage_bytes_used<
               storage_block_info<
               // Finite Field API
               finite_field_math<typename fifi::default_field<Field>::type,
               finite_field_info<Field,
               // Factory API
               final_coder_factory_pool<
               // Final type
               full_rlnc_encoder<Field>
               > > > > > > > > > > > > > > > >
    { };

    /// Decoder stack with shallow storage as required by the
    /// storage decoder.
    template<class Field>
    class shallow_rlnc_decoder :
        public // Payload API
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
               mutable_shallow_symbol_storage<
               storage_bytes_used<
               storage_block_info<
               // Finite Field API
               finite_field_math<typename fifi::default_field<Field>::type,
               finite_field_info<Field,
               // Factory API
               final_coder_factory_pool<
               // Final type
               shallow_rlnc_decoder<Field>
               > > > > > > > > > > > > > > >
    { };
}



int main()
{

    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    uint32_t max_symbols = 42;
    uint32_t max_symbol_size = 64;
    uint32_t object_size = 23456;

    typedef kodo::storage_encoder<
        kodo::shallow_rlnc_encoder<fifi::binary> >
           storage_encoder;

    typedef kodo::shallow_storage_decoder<
        kodo::shallow_rlnc_decoder<fifi::binary> >
           storage_decoder;

    storage_encoder::factory encoder_factory(max_symbols, max_symbol_size);
    storage_decoder::factory decoder_factory(max_symbols, max_symbol_size);

    // The storage needed for all decoders
    uint32_t total_block_size =
        decoder_factory.total_block_size(object_size);

    std::vector<uint8_t> data_out(total_block_size, '\0');
    std::vector<uint8_t> data_in(object_size, 'x');

    storage_encoder encoder(
        encoder_factory, sak::storage(data_in));

    storage_decoder decoder(
        decoder_factory, object_size, sak::storage(data_out));

    for(uint32_t i = 0; i < encoder.encoders(); ++i)
    {
        auto e = encoder.build(i);
        auto d = decoder.build(i);

        std::vector<uint8_t> payload(e->payload_size());

        while(!d->is_complete())
        {
            e->encode( &payload[0] );

            // Here we would send and receive the payload over a
            // network. Lets throw away some packet to simulate.
            if((rand() % 2) == 0)
                continue;

            d->decode( &payload[0] );

        }
    }

    // Resize the output buffer to contain only the object data
    data_out.resize(object_size);

    // Check we properly decoded the data
    if (std::equal(data_in.begin(),data_in.end(), data_out.begin()))
    {
        std::cout << "Data decoded correctly" << std::endl;
    }
    else
    {
        std::cout << "Unexpected failure to decode "
                  << "please file a bug report :)" << std::endl;
    }
}

