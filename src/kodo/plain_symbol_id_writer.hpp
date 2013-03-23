// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_PLAIN_SYMBOL_ID_WRITER_HPP
#define KODO_PLAIN_SYMBOL_ID_WRITER_HPP

#include <cstdint>

#include "plain_symbol_id.hpp"

namespace kodo
{

    /// @brief Writes the symbol id and initializes the coding
    ///        coefficients pointer. This implementation is the most
    ///        basic one where the symbol id simply is the coding
    ///        coefficients.
    ///
    /// @ingroup symbol_id_layers
    template<class SuperCoder>
    class plain_symbol_id_writer_base : public SuperCoder
    {
    public:

        /// @copydoc layer::write_id(uint8_t*, uint8_t**)
        uint32_t write_id(uint8_t *symbol_id, uint8_t **symbol_coefficients)
            {
                assert(symbol_id != 0);
                assert(symbol_coefficients != 0);

                SuperCoder::generate(symbol_id);
                *symbol_coefficients = symbol_id;

                return SuperCoder::id_size();
            }

    };

    /// @copydoc plain_symbol_id_writer_base
    template<class SuperCoder>
    class plain_symbol_id_writer
        : public plain_symbol_id_writer_base<
                 plain_symbol_id<SuperCoder> >
    {};

}

#endif

