// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_SEED_CODES_H
#define KODO_RLNC_SEED_CODES_H

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
#include "../payload_encoder.h"
#include "../payload_decoder.h"

#include "full_vector_storage.h"
#include "seed_encoder.h"
#include "seed_decoder.h"

namespace kodo
{

    template<class Field>
    class seed_rlnc_encoder
        : public payload_encoder<
                 systematic_encoder<
                 zero_payload_encoder<
                 seed_encoder<block_uniform,
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_shallow_partial<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<seed_rlnc_encoder<Field>, Field>
                     > > > > > > > >
    {};
    
    template<class Field>
    class seed_rlnc_decoder
        : public payload_decoder<
                 systematic_decoder<
                 seed_decoder<block_uniform,
                 full_vector_storage<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_deep<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<seed_rlnc_decoder<Field>, Field>
                     > > > > > > > >
    {};

    // Common typedefs
    typedef seed_rlnc_encoder<fifi::binary> seed_rlnc2_encoder;
    typedef seed_rlnc_decoder<fifi::binary> seed_rlnc2_decoder;

    typedef seed_rlnc_encoder<fifi::binary8> seed_rlnc8_encoder;
    typedef seed_rlnc_decoder<fifi::binary8> seed_rlnc8_decoder;

    typedef seed_rlnc_encoder<fifi::binary16> seed_rlnc16_encoder;
    typedef seed_rlnc_decoder<fifi::binary16> seed_rlnc16_decoder;
    
}


#endif

