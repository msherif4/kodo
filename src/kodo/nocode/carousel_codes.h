#ifndef KODO_CAROUSEL_CODES_H
#define KODO_CAROUSEL_CODES_H

#include <fifi/default_field_impl.h>

#include "carousel_encoder.h"
#include "carousel_decoder.h"

#include "../payload_encoder.h"
#include "../payload_decoder.h"
#include "../zero_symbol_encoder.h"
#include "../linear_block_encoder.h"
#include "../linear_block_decoder.h"
#include "../symbol_storage_shallow_partial.h"
#include "../symbol_storage_deep.h"
#include "../has_block_info.h"
#include "../has_bytes_used.h"
#include "../final_coder_factory_pool.h"
#include "../finite_field_math.h"
#include "../linear_block_vector_storage.h"

namespace kodo
{

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

