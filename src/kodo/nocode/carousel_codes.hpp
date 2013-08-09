// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "../payload_encoder.hpp"
#include "../payload_decoder.hpp"
#include "../zero_symbol_encoder.hpp"
#include "../linear_block_encoder.hpp"
#include "../forward_linear_block_decoder.hpp"
#include "../deep_symbol_storage.hpp"
#include "../final_coder_factory_pool.hpp"
#include "../finite_field_info.hpp"
#include "../coefficient_storage.hpp"
#include "../coefficient_info.hpp"

#include "../storage_bytes_used.hpp"
#include "../storage_block_info.hpp"

#include "carousel_encoder.hpp"
#include "carousel_decoder.hpp"
#include "nocode_encoder.hpp"
#include "nocode_decoder.hpp"


namespace kodo
{

    /// @ingroup fec_stacks
    /// @brief For testing purposes this encoder implements a simple repetition
    ///        code i.e. the symbols are the original source symbols produced
    ///        by the encoder in a round-robin fashion.
    class nocode_carousel_encoder
        : public // Payload Codec API
                 payload_encoder<
                 // Codec Header API
                 carousel_encoder<
                 // Codec API
                 nocode_encoder<
                 // Symbol Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_info<fifi::binary,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 nocode_carousel_encoder
                     > > > > > > > >
    { };

    /// @ingroup fec_stacks
    /// @brief For testing purposes this decoder will takes symbols produced by
    ///        the carousel encoder and put them in the right order until all
    ///        symbols have been received.
    class nocode_carousel_decoder
        : public // Payload Codec API
                 payload_decoder<
                 // Codec Header API
                 carousel_decoder<
                 // Codec API
                 nocode_decoder<
                 // Symbol Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_info<fifi::binary,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 nocode_carousel_decoder
                     > > > > > > > >
    { };

}



