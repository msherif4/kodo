// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include "plain_symbol_id.hpp"

namespace kodo
{

    /// @ingroup symbol_id_layers
    /// @brief Writes the symbol id and initializes the coding
    ///        coefficients pointer. This implementation is the most
    ///        basic one where the symbol id simply is the coding
    ///        coefficients.
    template<class SuperCoder>
    class base_plain_symbol_id_writer : public SuperCoder
    {
    public:

        /// @copydoc layer::write_id(uint8_t*, uint8_t**)
        uint32_t write_id(uint8_t *symbol_id, uint8_t **coefficients)
        {
            assert(symbol_id != 0);
            assert(coefficients != 0);

            SuperCoder::generate(symbol_id);
            *coefficients = symbol_id;

            return SuperCoder::id_size();
        }

    };

    /// @copydoc base_plain_symbol_id_writer
    template<class SuperCoder>
    class plain_symbol_id_writer
        : public base_plain_symbol_id_writer<
                 plain_symbol_id<SuperCoder> >
    { };

}


