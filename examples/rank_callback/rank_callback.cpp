// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @example rank_callback.cpp
/// 
/// It may be that we want some code to be executed on some event that happens
/// within the decoder. This can be done using callback functions.
/// This example shows how this can be done by adding the rank_callback layer
/// to the decoder stack and how the rank changed event can be handled in three
/// different ways.

#include <functional>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/rank_callback.hpp>

namespace kodo
{
    // Added rank_callback layer to decoder stack
    template<class Field>
    class full_rlnc_callback_decoder
        : public // Payload API
                 payload_decoder<
                 // Codec Header API
                 systematic_decoder<
                 symbol_id_decoder<
                 // Symbol ID API
                 plain_symbol_id_reader<
                 // Codec API
                 rank_callback<
                 align_coefficient_decoder<
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Finite Field Math API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 // Storage API
                 symbol_storage_tracker<
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 full_rlnc_callback_decoder<Field>, Field>
                     > > > > > > > > > > > > > >
    {};
}

// Typdefs for the encoder/decoder type we wish to use
typedef kodo::full_rlnc_encoder<fifi::binary8> rlnc_encoder;
typedef kodo::full_rlnc_callback_decoder<fifi::binary8> rlnc_decoder;

// Global function as callback handler
void rank_changed_event(uint32_t rank)
{
    std::cout << "Rank changed to " << rank << std::endl;
}

// Global function as callback handler with pointer to the calling decoder
// as parameter
void rank_changed_event2(boost::weak_ptr<rlnc_decoder> w_decoder, uint32_t rank)
{
    /// Lock decoder pointer so that it cannot be freed until we are done
    if ( boost::shared_ptr<rlnc_decoder> decoder = w_decoder.lock() )
    {
        std::cout << "Rank changed to " << rank <<
            " (" << decoder << ")" << std::endl;
    }
}

// Some class
class callback_handler
{
    public:

        // Member function as callback handler
        void rank_changed_event3(uint32_t rank)
        {
            std::cout << "Rank changed to " << rank << std::endl;
        }
};

int main()
{
    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    uint32_t symbols = 8;
    uint32_t symbol_size = 160;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_encoder::factory encoder_factory(symbols, symbol_size);
    rlnc_encoder::pointer encoder = encoder_factory.build(symbols, symbol_size);

    rlnc_decoder::factory decoder_factory(symbols, symbol_size);
    rlnc_decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);


    // Specify which callback function that should be used:
    //  0: rank_changed_event()
    //  1: rank_changed_event2()
    //  2: rank_changed_event3()
    #define CALLBACK_FUNC 0

    #if CALLBACK_FUNC == 0
        // Set callback for decoder to be a global function
        decoder->set_rank_changed_callback( rank_changed_event );

    #elif CALLBACK_FUNC == 1
        // Gets a weak pointer to decoder to ensure that our callback doesn't
        // prevent kodo from freeing memory
        boost::weak_ptr<rlnc_decoder> w_ptr(decoder);

        // Set callback for decoder to be a global function that takes a
        // pointer to the calling decoder as an additional argument
        decoder->set_rank_changed_callback (
            std::bind( &rank_changed_event2, w_ptr, std::placeholders::_1 )
        );

    #elif CALLBACK_FUNC == 2
        // Declare a class to handle callback
        callback_handler handler;

        // Set callback for decoder using lambda expression
        decoder->set_rank_changed_callback (
            [&] (uint32_t rank) { handler.rank_changed_event3( rank ); }
        );

    #else
        std::cout << "Decoder will not do any callback on rank changed."
                  << std::endl;
    #endif


    // Allocate some storage for a "payload" the payload is what we would
    // eventually send over a network
    std::vector<uint8_t> payload(encoder->payload_size());

    // Allocate some data to encode. In this case we make a buffer with the
    // same size as the encoder's block size (the max. amount a single encoder
    // can encode)
    std::vector<uint8_t> data_in(encoder->block_size(), 'x');


    // Assign the data buffer to the encoder so that we may start
    // to produce encoded symbols from it
    encoder->set_symbols(sak::storage(data_in));

    while( !decoder->is_complete() )
    {
        // Encode a packet into the payload buffer
        encoder->encode( &payload[0] );

        // Pass that packet to the decoder
        decoder->decode( &payload[0] );
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
