// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

namespace kodo
{

    /// @ingroup codec_layers
    /// @ingroup empty
    /// @brief Empty implementation of the decode_symbol(uint8_t*,uint8_t*) and
    ///        decode_symbol(uint8_t*, uint32_t) functions.
    template<class SuperCoder>
    class empty_decoder : public SuperCoder
    {
    public:

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data,
                           uint8_t *symbol_coefficients)
        {
            (void) symbol_data;
            (void) symbol_coefficients;
        }

        /// @copydoc layer::decode_symbol(uint8_t*,uint32_t)
        void decode_symbol(uint8_t *symbol_data, uint32_t symbol_index)
        {
            (void) symbol_data;
            (void) symbol_index;
        }

    };

}

