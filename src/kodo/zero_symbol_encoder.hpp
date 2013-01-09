// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_ZERO_SYMBOL_ENCODER_HPP
#define KODO_ZERO_SYMBOL_ENCODER_HPP

#include <stdint.h>

namespace kodo
{
    /// Zeros the incoming symbol buffers
    template<class SuperCoder>
    class zero_symbol_encoder : public SuperCoder
    {
    public:

        /// Zero the incoming symbol buffers and forward
        /// call.
        /// @param symbol_data symbol storage
        /// @param symbol_id symbol id storage
        /// @return the number of byes used by the underlying coding layers
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                std::fill_n(symbol_data, SuperCoder::symbol_size(), 0);
                std::fill_n(symbol_id, SuperCoder::symbol_id_size(), 0);

                return SuperCoder::encode(symbol_data, symbol_id);
            }
    };
}

#endif
