// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include "full_vector_codes.hpp"
#include "../storage_aware_generator.hpp"
#include "../partial_decoding_tracker.hpp"

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
               > > > > > > > > > > > > > > > > >
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
               payload_recoder<recoding_stack,
               payload_decoder<
               // Codec Header API
               systematic_decoder<
               symbol_id_decoder<
               // Symbol ID API
               plain_symbol_id_reader<
               // Codec API
               aligned_coefficients_decoder<
               partial_decoding_tracker<
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
               on_the_fly_decoder<Field>
               > > > > > > > > > > > > > > > >
    { };

}



