// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <fifi/default_field.hpp>

#include "../aligned_coefficients_decoder.hpp"
#include "../final_coder_factory_pool.hpp"
#include "../final_coder_factory.hpp"
#include "../finite_field_math.hpp"
#include "../finite_field_info.hpp"
#include "../zero_symbol_encoder.hpp"
#include "../systematic_encoder.hpp"
#include "../systematic_decoder.hpp"
#include "../storage_bytes_used.hpp"
#include "../storage_block_info.hpp"
#include "../deep_symbol_storage.hpp"
#include "../payload_encoder.hpp"
#include "../payload_recoder.hpp"
#include "../payload_decoder.hpp"
#include "../symbol_id_encoder.hpp"
#include "../symbol_id_decoder.hpp"
#include "../coefficient_storage.hpp"
#include "../coefficient_info.hpp"
#include "../plain_symbol_id_reader.hpp"
#include "../plain_symbol_id_writer.hpp"
#include "../uniform_generator.hpp"
#include "../recoding_symbol_id.hpp"
#include "../proxy_layer.hpp"
#include "../storage_aware_encoder.hpp"
#include "../encode_symbol_tracker.hpp"
#include "../cached_symbol_decoder.hpp"
#include "../debug_cached_symbol_decoder.hpp"
#include "../debug_linear_block_decoder.hpp"

#include "../linear_block_encoder.hpp"
#include "../forward_linear_block_decoder.hpp"
#include "../linear_block_decoder_delayed.hpp"

namespace kodo
{

    /// @ingroup fec_stacks
    /// @brief Complete stack implementing a RLNC encoder.
    ///
    /// The key features of this configuration is the following:
    /// - Systematic encoding (uncoded symbols produced before switching
    ///   to coding)
    /// - Full encoding vectors, this stack uses the plain_symbol_id_writer
    ///   which sends the full encoding vector with every encoded symbol.
    ///   Encoding vectors are generated using a random uniform generator.
    /// - Deep symbol storage which makes the encoder allocate its own
    ///   internal memory.
    template<class Field>
    class full_rlnc_encoder :
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
               encode_symbol_tracker<
               zero_symbol_encoder<
               linear_block_encoder<
               storage_aware_encoder<
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
               full_rlnc_encoder<Field
                   > > > > > > > > > > > > > > > > >
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
                 encode_symbol_tracker<
                 zero_symbol_encoder<
                 linear_block_encoder<
                 // Proxy
                 proxy_layer<
                 recoding_stack<MainStack>, MainStack> > > > > > > > >
    { };

    /// @ingroup fec_stacks
    /// @brief Implementation of a complete RLNC decoder
    ///
    /// This configuration adds the following features (including those
    /// described for the encoder):
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
                 aligned_coefficients_decoder<
                 forward_linear_block_decoder<
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
                 full_rlnc_decoder<Field>
                     > > > > > > > > > > > > > > >
    { };

    /// @ingroup fec_stacks
    /// @brief Implementation of a full_rlnc_decoder, but with the debug
    ///        layers added.
    ///
    /// @copydoc full_rlnc_decoder
    template<class Field>
    class debug_full_rlnc_decoder
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
                 debug_linear_block_decoder<  // <-- Debug layer
                 debug_cached_symbol_decoder< // <-- Debug layer
                 cached_symbol_decoder<       // <-- Access to decoding symbols
                 forward_linear_block_decoder<
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
                 debug_full_rlnc_decoder<Field>
                     > > > > > > > > > > > > > > > > > >
    { };

}


