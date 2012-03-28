// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_REED_SOLOMON_CODES_H
#define KODO_RS_REED_SOLOMON_CODES_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>

#include <fifi/fifi_utils.h>
#include <fifi/default_field_impl.h>
#include <fifi/is_binary.h>

#include "../final_coder_factory_pool.h"
#include "../final_coder_factory.h"
#include "../finite_field_math.h"
#include "../zero_payload_encoder.h"
#include "../systematic_encoder.h"
#include "../systematic_decoder.h"
#include "../has_bytes_used.h"
#include "../has_block_info.h"
#include "../storage.h"
#include "../symbol_storage_shallow.h"
#include "../symbol_storage_shallow_partial.h"
#include "../symbol_storage_deep.h"
#include "../generators/block.h"
#include "../generators/block_cache.h"
#include "../generators/block_cache_lookup.h"
#include "../payload_encoder.h"
#include "../payload_decoder.h"

#include "../rlnc/full_vector_storage.h"

#include "vandermonde_matrix.h"
#include "reed_solomon_encoder.h"
#include "reed_solomon_decoder.h"

namespace kodo
{

    template<class Field>
    class rs_encoder
        : public payload_encoder<
                 zero_payload_encoder<
                 reed_solomon_encoder<vandermonde_matrix,
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_shallow_partial<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<rs_encoder<Field>, Field>
                     > > > > > > >
    {};
    
    template<class Field>
    class rs_decoder
        : public payload_decoder<
                 reed_solomon_decoder<vandermonde_matrix,
                 full_vector_storage<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_deep<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<rs_decoder<Field>, Field>
                     > > > > > > >
    {};

    /// Common typedefs
    typedef rs_encoder<fifi::binary8> rs8_encoder;
    typedef rs_decoder<fifi::binary8> rs8_decoder;

    //typedef full_rlnc_decoder<fifi::binary8> full_rlnc8_decoder;
    //typedef full_rlnc_encoder<fifi::binary16> full_rlnc16_encoder;
    //typedef full_rlnc_decoder<fifi::binary16> full_rlnc16_decoder;
    
}

#endif

