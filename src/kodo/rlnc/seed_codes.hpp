// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_SEED_CODES_HPP
#define KODO_RLNC_SEED_CODES_HPP

#include <cstdint>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>

#include <fifi/fifi_utils.hpp>
#include <fifi/default_field_impl.hpp>
#include <fifi/is_binary.hpp>

#include <sak/storage.hpp>

#include "../final_coder_factory_pool.hpp"
#include "../final_coder_factory.hpp"
#include "../finite_field_math.hpp"
#include "../zero_symbol_encoder.hpp"
#include "../systematic_encoder.hpp"
#include "../systematic_decoder.hpp"
#include "../has_bytes_used.hpp"
#include "../has_block_info.hpp"
#include "../partial_shallow_symbol_storage.hpp"
#include "../deep_symbol_storage.hpp"
#include "../generators/block.hpp"
#include "../payload_encoder.hpp"
#include "../payload_decoder.hpp"
#include "../linear_block_encoder.hpp"
#include "../linear_block_decoder.hpp"
#include "../linear_block_vector_storage.hpp"
#include "../linear_block_vector_generator.hpp"

#include "seed_encoder.hpp"
#include "seed_decoder.hpp"

namespace kodo
{
    /// @copydoc seed_encoder
    template<class Field>
    class seed_rlnc_encoder
        : public payload_encoder<
                 systematic_encoder<
                 zero_symbol_encoder<
                 seed_encoder<
                 linear_block_vector_generator<block_uniform,
                 linear_block_encoder<
                 finite_field_math<fifi::default_field_impl,
                 partial_shallow_symbol_storage<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<seed_rlnc_encoder<Field>, Field>
                     > > > > > > > > > >
    {};

    /// @copydoc seed_decoder
    template<class Field>
    class seed_rlnc_decoder
        : public payload_decoder<
                 systematic_decoder<
                 seed_decoder<
                 linear_block_vector_generator<block_uniform,
                 linear_block_decoder<
                 linear_block_vector_storage<
                 finite_field_math<fifi::default_field_impl,
                 deep_symbol_storage<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<seed_rlnc_decoder<Field>, Field>
                     > > > > > > > > > >
    {};

    /// Common typedefs
    typedef seed_rlnc_encoder<fifi::binary> seed_rlnc2_encoder;
    typedef seed_rlnc_decoder<fifi::binary> seed_rlnc2_decoder;

    typedef seed_rlnc_encoder<fifi::binary8> seed_rlnc8_encoder;
    typedef seed_rlnc_decoder<fifi::binary8> seed_rlnc8_decoder;

    typedef seed_rlnc_encoder<fifi::binary16> seed_rlnc16_encoder;
    typedef seed_rlnc_decoder<fifi::binary16> seed_rlnc16_decoder;
}

#endif

