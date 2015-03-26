// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <fifi/default_field.hpp>

#include "../final_coder_factory_pool.hpp"
#include "../finite_field_math.hpp"
#include "../finite_field_info.hpp"
#include "../zero_symbol_encoder.hpp"
#include "../systematic_encoder.hpp"
#include "../systematic_decoder.hpp"
#include "../storage_bytes_used.hpp"
#include "../storage_block_info.hpp"
#include "../deep_symbol_storage.hpp"
#include "../payload_encoder.hpp"
#include "../payload_decoder.hpp"
#include "../symbol_id_encoder.hpp"
#include "../symbol_id_decoder.hpp"
#include "../coefficient_storage.hpp"
#include "../coefficient_info.hpp"
#include "../storage_aware_encoder.hpp"
#include "../encode_symbol_tracker.hpp"
#include "../linear_block_encoder.hpp"
#include "../forward_linear_block_decoder.hpp"

#include "reed_solomon_symbol_id_writer.hpp"
#include "reed_solomon_symbol_id_reader.hpp"
#include "systematic_vandermonde_matrix.hpp"

namespace kodo
{

    /// @ingroup fec_stacks
    /// @brief Complete stack implementing a Reed-Solomon encoder.
    ///
    /// The key features of this configuration is the following:
    /// - Systematic encoding (uncoded symbols produced before switching
    ///   to coding)
    /// - Deep symbol storage which makes the encoder allocate its own
    ///   internal memory.
    template<class Field>
    class rs_encoder
        : public // Payload Codec API
                 payload_encoder<
                 // Codec Header API
                 systematic_encoder<
                 symbol_id_encoder<
                 // Symbol ID API
                 reed_solomon_symbol_id_writer<
                 systematic_vandermonde_matrix<
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
                 rs_encoder<Field>
                     > > > > > > > > > > > > > > > >
    { };

    /// @ingroup fec_stacks
    /// @brief Implementation of a complete RS decoder
    ///
    /// This configuration adds the following features (including those
    /// described for the encoder):
    /// - Linear block decoder using Gauss-Jordan elimination.
    template<class Field>
    class rs_decoder
        : public // Payload API
                 payload_decoder<
                 // Codec Header API
                 systematic_decoder<
                 symbol_id_decoder<
                 // Symbol ID API
                 reed_solomon_symbol_id_reader<
                 systematic_vandermonde_matrix<
                 // Codec API
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
                 rs_decoder<Field>
                     > > > > > > > > > > > > > >
    { };

}



