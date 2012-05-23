// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_CODES_H
#define KODO_RLNC_FULL_VECTOR_CODES_H

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
#include "../zero_symbol_encoder.h"
#include "../systematic_encoder.h"
#include "../non_systematic_encoder.h"
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

#include "full_vector_encoder.h"
#include "full_vector_decoder.h"
#include "full_vector_recoder.h"
#include "full_vector_payload_recoder.h"
#include "full_vector_systematic_recoder.h"

#include "../linear_block_encoder.h"
#include "../linear_block_decoder.h"
#include "../linear_block_decoder_delayed.h"
#include "../linear_block_vector_storage.h"
#include "../linear_block_vector_generator.h"

#include "../../kodo_debug/full_vector_decoder_debug.h"

namespace kodo
{

    /// A basic RLNC encoder. This type of RLNC encoder
    /// transmits the entire encoding vector as part of the
    /// encoded payload. It therefore allows recoding at
    /// intermediate nodes in a network.
    template<class Field>
    class full_rlnc_encoder
        : public payload_encoder<
                 systematic_encoder<
                 zero_symbol_encoder<
                 full_vector_encoder<
                 linear_block_vector_generator<block_uniform_no_position,
                 linear_block_encoder<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_shallow_partial<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<full_rlnc_encoder<Field>, Field>
                     > > > > > > > > > >
    {};


    /// Intermediate layer utilized by the re-coding functionality
    /// of a RLNC decoder. This allows us to re-use layers from the
    /// RLNC encoder to build a RLNC recoder
    template<class SuperCoder>
    class recode_proxy
        : public payload_encoder<
                 non_systematic_encoder<
                 zero_symbol_encoder<SuperCoder
                     > > >
    {};

    /// A RLNC decoder. The decoder decodes according to a
    /// full encoding vector.
    template<class Field>
    class full_rlnc_decoder
        : public full_vector_recoder<recode_proxy, random_uniform,
                 payload_decoder<
                 systematic_decoder<
                 full_vector_decoder<
                 linear_block_decoder<
                 linear_block_vector_storage<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_deep<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<full_rlnc_decoder<Field>, Field>
                     > > > > > > > > > >
    {};

    /// Common typedefs
    typedef full_rlnc_encoder<fifi::binary> full_rlnc2_encoder;
    typedef full_rlnc_decoder<fifi::binary> full_rlnc2_decoder;

    typedef full_rlnc_encoder<fifi::binary8> full_rlnc8_encoder;
    typedef full_rlnc_decoder<fifi::binary8> full_rlnc8_decoder;

    typedef full_rlnc_encoder<fifi::binary16> full_rlnc16_encoder;
    typedef full_rlnc_decoder<fifi::binary16> full_rlnc16_decoder;

    /// A RLNC decoder with delayed backwards substitute. The decoder decodes
    /// according to a full encoding vector.
    template<class Field>
    class full_rlnc_decoder_delayed
        : public full_vector_recoder<recode_proxy, random_uniform,
                 payload_decoder<
                 systematic_decoder<
                 full_vector_decoder<
                 linear_block_decoder_delayed<
                 linear_block_decoder<
                 linear_block_vector_storage<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_deep<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<full_rlnc_decoder_delayed<Field>, Field>
                     > > > > > > > > > > >
    {};


}

#endif

