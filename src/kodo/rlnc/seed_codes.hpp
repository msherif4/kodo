// Copyright Steinwurf ApS 2011-2012.
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
#include "../seed_symbol_id_writer.hpp"
#include "../seed_symbol_id_reader.hpp"
#include "../uniform_generator.hpp"
#include "../recoding_symbol_id.hpp"
#include "../proxy_layer.hpp"
#include "../storage_aware_encoder.hpp"
#include "../encode_symbol_tracker.hpp"

#include "../linear_block_encoder.hpp"
#include "../forward_linear_block_decoder.hpp"

namespace kodo
{

    /// @ingroup fec_stacks
    /// @brief Complete stack implementing a seed based RLNC encoder.
    ///
    /// The key features of this configuration is the following:
    /// - Systematic encoding (uncoded symbols produced before switching
    ///   to coding)
    /// - A seed is sent instead of a full encoding vectors, this reduces
    ///   the amount of overhead per symbol.
    /// - Encoding vectors are generated using a random uniform generator.
    /// - Deep symbol storage which makes the encoder allocate its own
    ///   internal memory.
    template<class Field>
    class seed_rlnc_encoder
        : public // Payload Codec API
                 payload_encoder<
                 // Codec Header API
                 systematic_encoder<
                 symbol_id_encoder<
                 // Symbol ID API
                 seed_symbol_id_writer<
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
                 seed_rlnc_encoder<Field>
                     > > > > > > > > > > > > > > > >
    { };

    /// @ingroup fec_stacks
    /// @brief Implementation of a seed based RLNC decoder this configuration.
    ///
    /// Adds the following features (including those described for
    /// the encoder):
    /// - Linear block decoder using Gauss-Jordan elimination.
    template<class Field>
    class seed_rlnc_decoder
        : public // Payload API
                 payload_decoder<
                 // Codec Header API
                 systematic_decoder<
                 symbol_id_decoder<
                 // Symbol ID API
                 seed_symbol_id_reader<
                 // Coefficient Generator API
                 uniform_generator<
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
                 // Finite Field Math API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 seed_rlnc_decoder<Field>
                     > > > > > > > > > > > > > > >
    { };

}


