// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>
#include <iostream>
#include <vector>

#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup debug
    /// @brief Debug layer which allows inspecting the state of a
    ///        linear block decoder.
    template<class SuperCoder>
    class nada_decoder : public SuperCoder
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

