// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_ZERO_SYMBOL_ENCODER_HPP
#define KODO_ZERO_SYMBOL_ENCODER_HPP

#include <cstdint>

namespace kodo
{

    /// @ingroup codec_layers
    /// @brief Zeros the symbol data buffer
    template<class SuperCoder>
    class zero_symbol_encoder : public SuperCoder
    {
    public:

        /// Zero the incoming symbol data buffer and forward
        /// the encode_symbol() call.
        ///
        /// @copydoc layer::encode_symbol(uint8_t*,uint8_t*)
        void encode_symbol(uint8_t *symbol_data, uint8_t *coefficients)
            {
                assert(symbol_data != 0);
                assert(coefficients != 0);

                std::fill_n(symbol_data, SuperCoder::symbol_size(), 0);
                SuperCoder::encode_symbol(symbol_data, coefficients);
            }

        /// Not implemented in this layer - the systematic encode will
        /// typically copy directly into symbol_data buffer. Therefore
        /// we don't have to worry about junk bytes existing in the buffer
        /// they will be overwritten.
        ///
        /// @copydoc layer::encode_symbol(uint8_t*,uint8_t*)
        void encode_symbol(uint8_t *symbol_data, uint32_t symbol_index)
            {
                SuperCoder::encode_symbol(symbol_data, symbol_index);
            }

    };
}

#endif

