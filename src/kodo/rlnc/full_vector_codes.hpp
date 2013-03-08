// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_CODES_HPP
#define KODO_RLNC_FULL_VECTOR_CODES_HPP

#include <cstdint>

#include <fifi/default_field_impl.hpp>

#include "../final_coder_factory_pool.hpp"
#include "../final_coder_factory.hpp"
#include "../finite_field_math.hpp"
#include "../zero_symbol_encoder.hpp"
#include "../systematic_encoder.hpp"
#include "../systematic_decoder.hpp"
#include "../storage_bytes_used.hpp"
#include "../storage_block_info.hpp"
#include "../deep_symbol_storage.hpp"
#include "../payload_encoder.hpp"
#include "../payload_recoder.hpp"
#include "../payload_decoder.hpp"
#include "../align_coefficient_decoder.hpp"
#include "../symbol_id_encoder.hpp"
#include "../symbol_id_decoder.hpp"
#include "../coefficient_storage.hpp"
#include "../coefficient_info.hpp"
#include "../plain_symbol_id_reader.hpp"
#include "../plain_symbol_id_writer.hpp"
#include "../uniform_generator.hpp"
#include "../recoding_symbol_id.hpp"
#include "../proxy_layer.hpp"
#include "../symbol_storage_tracker.hpp"

#include "../linear_block_encoder.hpp"
#include "../linear_block_decoder.hpp"
#include "../linear_block_decoder_delayed.hpp"

namespace kodo
{

    /// Complete stack implementing a RLNC encoder. The key features of
    /// this configuration is the following:
    /// - Systematic encoding (uncoded symbols produced before switching
    ///   to coding)
    /// - Full encoding vectors, this stack uses the plain_symbol_id_writer
    ///   which sends the full encoding vector with every encoded symbol.
    ///   Encoding vectors are generated using a random uniform generator.
    /// - Deep symbol storage which makes the encoder allocate its own
    ///   internal memory.
    template<class Field>
    class full_rlnc_encoder
        : public // Payload Codec API
                 payload_encoder<
                 // Codec Header API
                 systematic_encoder<
                 symbol_id_encoder<
                 // Symbol ID API
                 plain_symbol_id_writer<
                 // Coefficient Generator API
                 uniform_generator<
                 // Codec API
                 zero_symbol_encoder<
                 linear_block_encoder<
                 // Coefficient Storage API
                 coefficient_info<
                 // Finite Field Math API
                 finite_field_math<fifi::default_field_impl,
                 // Symbol Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 full_rlnc_encoder<Field>, Field>
                     > > > > > > > > > > > >
    { };

    /// Intermediate stack implementing the recoding functionality of a
    /// RLNC code. As can be seen we are able to reuse a great deal of
    /// layers from the encode stack. It is important that the symbols
    /// produced by the recoder are compatible with the decoder. This
    /// means we have to use compatible Payload, Codec Header Symbol ID
    /// layers, between the encoder, recoder and decoder.
    /// The only layer specific to recoding is the recoding_symbol_id
    /// layer. Finally the recoder uses a proxy_layer which forwards
    /// any calls not implemented in the recoding stack to the MainStack.
    template<class MainStack>
    class recoding_stack
        : public // Payload API
                 payload_encoder<
                 // Codec Header API
                 non_systematic_encoder<
                 symbol_id_encoder<
                 // Symbol ID API
                 recoding_symbol_id<
                 // Coefficient Generator API
                 uniform_generator<
                 // Codec API
                 zero_symbol_encoder<
                 linear_block_encoder<
                 // Proxy
                 proxy_layer<
                 recoding_stack<MainStack>, MainStack> > > > > > > >
    { };

    /// Implementation of a complete RLNC decoder this configuration
    /// adds the following features (including those described for
    /// the encoder):
    /// - Recoding using the recoding_stack
    /// - Linear block decoder using Gauss-Jordan elimination.
    template<class Field>
    class full_rlnc_decoder
        : public // Payload API
                 payload_recoder<recoding_stack,
                 payload_decoder<
                 // Codec Header API
                 systematic_decoder<
                 symbol_id_decoder<
                 // Symbol ID API
                 plain_symbol_id_reader<
                 // Codec API
                 align_coefficient_decoder<
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Finite Field Math API
                 finite_field_math<fifi::default_field_impl,
                 // Storage API
                 symbol_storage_tracker<
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 full_rlnc_decoder<Field>, Field>
                     > > > > > > > > > > > > > >
    {};


    /// A RLNC decoder with delayed backwards substitute. The decoder decodes
    /// according to a full encoding vector.
    // template<class Field>
    // class full_rlnc_decoder_delayed
    //     : public full_vector_recoder<recode_proxy, random_uniform,
    //              payload_decoder<
    //              systematic_decoder<
    //              align_symbol_id_decoder<
    //              full_vector_decoder<
    //              linear_block_decoder_delayed<
    //              linear_block_decoder<
    //              linear_block_vector_storage<
    //              finite_field_math<fifi::default_field_impl,
    //              deep_symbol_storage<
    //              storage_bytes_used<
    //              storage_block_info<
    //              final_coder_factory_pool<full_rlnc_decoder_delayed<Field>, Field>
    //                  > > > > > > > > > > > >
    // {};
}

#endif

