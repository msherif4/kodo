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
#include "../symbol_storage_shallow_partial.hpp"
#include "../symbol_storage_deep.hpp"
#include "../has_block_info.hpp"
#include "../has_bytes_used.hpp"
#include "../final_coder_factory_pool.hpp"
#include "../finite_field_math.hpp"
#include "../linear_block_vector_storage.hpp"

#include "carousel_encoder.hpp"
#include "carousel_decoder.hpp"

namespace kodo
{
    /// @copydoc carousel_encoder
    class nocode_carousel_encoder
        : public payload_encoder<
                 zero_symbol_encoder<
                 carousel_encoder<
                 linear_block_encoder<
                 symbol_storage_shallow_partial<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<nocode_carousel_encoder, fifi::binary>
                     > > > > > > >
    {};

    /// @copydoc carousel_decoder
    class nocode_carousel_decoder
        : public payload_decoder<
                 carousel_decoder<
                 linear_block_decoder<
                 linear_block_vector_storage<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_deep<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<nocode_carousel_decoder, fifi::binary>
                     > > > > > > > >
    {};

}

#endif

