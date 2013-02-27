// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_CAROUSEL_CODES_HPP
#define KODO_CAROUSEL_CODES_HPP

#include <fifi/default_field_impl.hpp>

#include "../payload_encoder.hpp"
#include "../payload_decoder.hpp"
#include "../zero_symbol_encoder.hpp"
#include "../linear_block_encoder.hpp"
#include "../linear_block_decoder.hpp"
#include "../partial_shallow_symbol_storage.hpp"
#include "../deep_symbol_storage.hpp"
#include "../has_block_info.hpp"
#include "../has_bytes_used.hpp"
#include "../final_coder_factory_pool.hpp"
#include "../finite_field_math.hpp"
#include "../linear_block_vector_storage.hpp"
#include "../align_coefficient_decoder.hpp"
#include "../coefficient_storage.hpp"
#include "../coefficient_info.hpp"

#include "../storage_bytes_used.hpp"
#include "../storage_block_info.hpp"

#include "carousel_encoder.hpp"
#include "carousel_decoder.hpp"

namespace kodo
{
    /// @todo Update this
    class nocode_carousel_encoder
        : public // Payload Codec API
                 payload_encoder<
                 // Codec Header API
                 carousel_encoder<
                 // Symbol Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Factory API
                 final_coder_factory_pool<
                 nocode_carousel_encoder, fifi::binary>
                     > > > > >
    {};

    /// @todo Update this
    class nocode_carousel_decoder
        : public // Payload Codec API
                 payload_decoder<
                 // Codec Header API
                 carousel_decoder<
                 // Codec API
                 align_coefficient_decoder<
                 linear_block_decoder<
                 linear_block_vector_storage<
                 coefficient_storage<
                 coefficient_info<
                 // Finite Field Math API
                 finite_field_math<fifi::default_field_impl,
                 // Symbol Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Factory API
                 final_coder_factory_pool<
                 nocode_carousel_decoder, fifi::binary>
                     > > > > > > > > > > >
    {};

}

#endif

