// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <sak/is_aligned.hpp>
#include <sak/storage.hpp>

#include "aligned_coefficients_buffer.hpp"

namespace kodo
{

    /// @ingroup codec_layers
    /// @brief Aligns the symbol coefficient buffer if necessary
    template<class SuperCoder>
    class aligned_coefficients_decoder
        : public aligned_coefficients_buffer<SuperCoder>
    {
    public:

        /// The actual SuperCoder type
        typedef aligned_coefficients_buffer<SuperCoder> Super;

        /// Pull up the decode_symbol() functions
        using Super::decode_symbol;

    public:

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data, uint8_t *coefficients)
        {
            assert(symbol_data != 0);
            assert(coefficients != 0);

            if(sak::is_aligned(coefficients) == false)
            {
                uint32_t coefficients_size = Super::coefficients_size();

                auto src = sak::storage(coefficients, coefficients_size);
                auto dest = sak::storage(m_coefficients);

                sak::copy_storage(dest, src);

                Super::decode_symbol(symbol_data, &m_coefficients[0]);
            }
            else
            {
                Super::decode_symbol(symbol_data, coefficients);
            }
        }

    private:

        /// Access the coefficients buffer in the
        /// aligned_coefficients_buffer layer
        using Super::m_coefficients;

    };

}


