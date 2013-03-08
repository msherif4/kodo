// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_ENCODE_SYMBOL_TRACKER_HPP
#define KODO_ENCODE_SYMBOL_TRACKER_HPP

#include <cstdint>

namespace kodo
{

    /// @ingroup codec_layers
    /// @brief Counts the number of symbols which have been encoded
    template<class SuperCoder>
    class encode_symbol_tracker : public SuperCoder
    {
    public:

        /// Constructor
        encode_symbol_tracker()
            : m_counter(0)
            { }

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_counter = 0;
            }

        /// @copydoc layer::encode_symbol(uint8_t*,uint8_t*)
        void encode_symbol(uint8_t *symbol_data,
                           uint8_t *symbol_coefficients)
            {
                SuperCoder::encode_symbol(
                    symbol_data, symbol_coefficients);

                ++m_counter;
            }

        /// @return the symbol encoded counter
        uint32_t encode_symbol_count() const
            {
                return m_counter;
            }

    private:

        /// Counter tracking the number of symbols encoded
        uint32_t m_counter;

    };
}

#endif

