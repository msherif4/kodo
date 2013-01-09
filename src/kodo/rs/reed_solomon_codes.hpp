// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_REED_SOLOMON_CODES_HPP
#define KODO_RS_REED_SOLOMON_CODES_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>

#include <fifi/fifi_utils.hpp>
#include <fifi/default_field_impl.hpp>
#include <fifi/is_binary.hpp>

#include "../final_coder_factory_pool.hpp"
#include "../final_coder_factory.hpp"
#include "../finite_field_math.hpp"
#include "../zero_symbol_encoder.hpp"
#include "../systematic_encoder.hpp"
#include "../systematic_decoder.hpp"
#include "../has_bytes_used.hpp"
#include "../has_block_info.hpp"
#include "../storage.hpp"
#include "../partial_shallow_symbol_storage.hpp"
#include "../symbol_storage_deep.hpp"
#include "../generators/block.hpp"
#include "../generators/block_cache.hpp"
#include "../generators/block_cache_lookup.hpp"
#include "../payload_encoder.hpp"
#include "../payload_decoder.hpp"
#include "../linear_block_encoder.hpp"
#include "../linear_block_decoder.hpp"
#include "../linear_block_vector_storage.hpp"

#include "vandermonde_matrix.hpp"
#include "reed_solomon_encoder.hpp"
#include "reed_solomon_decoder.hpp"

namespace kodo
{
    /// A reed-solomon encoder
    template<class Field>
    class rs_encoder
        : public payload_encoder<
                 zero_symbol_encoder<
                 reed_solomon_encoder<vandermonde_matrix,
                 linear_block_encoder<
                 finite_field_math<fifi::default_field_impl,
                 partial_shallow_symbol_storage<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<rs_encoder<Field>, Field>
                     > > > > > > > >
    {};

    /// A reed-solomon decoder
    template<class Field>
    class rs_decoder
        : public payload_decoder<
                 reed_solomon_decoder<vandermonde_matrix,
                 linear_block_decoder<
                 linear_block_vector_storage<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_deep<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<rs_decoder<Field>, Field>
                     > > > > > > > >
    {};

    /// Common typedefs
    typedef rs_encoder<fifi::binary8> rs8_encoder;
    typedef rs_decoder<fifi::binary8> rs8_decoder;
}

#endif

