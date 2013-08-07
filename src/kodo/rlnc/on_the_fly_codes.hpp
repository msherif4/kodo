// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include "full_vector_codes.hpp"
#include "../storage_aware_generator.hpp"
#include "../partial_decoding_tracker.hpp"
#include "../rank_info.hpp"
#include "../payload_rank_encoder.hpp"
#include "../payload_rank_decoder.hpp"

namespace kodo
{

    /// @ingroup fec_stacks
    /// @brief Complete stack implementing a full RLNC on-the-fly encoder.
    ///
    /// The on-the-fly encoder has the advantage that symbols can be specified
    /// as they arrive at the encoder. This breaks with a traditional block
    /// code where all the data has to be available before encoding can start.
    ///
    /// Implementation of on the fly RLNC encoder uses a storage aware generator
    /// and storage aware encoder. The storage aware generator makes sure that
    /// we do not generate non-zero coefficients for the missing symbols, the
    /// storage aware encoder provides the generator with information about how
    /// many symbols have been specified.
    template<class Field>
    class on_the_fly_encoder :
        public // Payload Codec API
               payload_rank_encoder<
               payload_encoder<
               // Codec Header API
               systematic_encoder<
               symbol_id_encoder<
               // Symbol ID API
               plain_symbol_id_writer<
               // Coefficient Generator API
               storage_aware_generator<
               uniform_generator<
               // Codec API
               encode_symbol_tracker<
               zero_symbol_encoder<
               linear_block_encoder<
               storage_aware_encoder<
               rank_info<
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
               on_the_fly_encoder<Field>
               > > > > > > > > > > > > > > > > > > >
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
    class on_the_fly_recoding_stack
        : public // Payload API
                 payload_rank_encoder<
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
                 rank_info<
                 // Proxy
                 proxy_layer<
                 on_the_fly_recoding_stack<MainStack>,
                 MainStack> > > > > > > > > > >
    { };


    /// @ingroup fec_stacks
    /// @brief Implementation of a complete RLNC decoder
    ///
    /// This configuration adds the following features (including those
    /// described for the encoder):
    /// - Recoding using the recoding_stack
    /// - Linear block decoder using Gauss-Jordan elimination.
    template<class Field>
    class on_the_fly_decoder :
        public // Payload API
               partial_decoding_tracker<
               payload_recoder<on_the_fly_recoding_stack,
               payload_rank_decoder<
               payload_decoder<
               // Codec Header API
               systematic_decoder<
               symbol_id_decoder<
               // Symbol ID API
               plain_symbol_id_reader<
               // Codec API
               aligned_coefficients_decoder<
               linear_block_decoder<
               rank_info<
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
               on_the_fly_decoder<Field>
               > > > > > > > > > > > > > > > > > >
    { };

}



