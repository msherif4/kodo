// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include <sak/storage.hpp>

#include <fifi/is_binary.hpp>
#include <fifi/fifi_utils.hpp>

#include <kodo/directional_linear_block_decoder.hpp>
#include <kodo/backward_linear_block_decoder_policy.hpp>

namespace kodo
{
    /// @ingroup codec_layers
    /// @brief Implements basic linear block decoder.
    ///
    /// The linear block decoder
    /// expects that an encoded symbol is described by a vector of
    /// coefficients. Using these coefficients the block decoder subtracts
    /// incoming symbols until the original data has been recreated.
    template<class SuperCoder>
    class backward_linear_block_decoder : public directional_linear_block_decoder<
        backward_linear_block_decoder_policy, SuperCoder>
    { };

}

