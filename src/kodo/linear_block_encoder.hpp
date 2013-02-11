// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_LINEAR_BLOCK_ENCODER_HPP
#define KODO_LINEAR_BLOCK_ENCODER_HPP

#include <cstdint>

#include <fifi/is_binary.hpp>

#include "linear_block_vector.hpp"

namespace kodo
{
    /// @ingroup codec_layers
    /// @brief A linear block encoder.
    ///
    /// This type of encoder iterates
    /// over a coefficient vector and combines symbols according
    /// to the coefficients selected.
    template<class SuperCoder>
    class linear_block_encoder : public SuperCoder
    {
    public:

        /// The field type
        typedef typename SuperCoder::field_type field_type;

        /// The value type
        typedef typename field_type::value_type value_type;

        /// The coefficient vector
        typedef linear_block_vector<field_type> vector_type;

    public:

        /// Encodes a symbol according to the encoding vector
        /// @param symbol_data the destination buffer for the encoded symbol
        /// @param symbol_id the encoding vector - note at this point the
        ///        coding vector should already be initialized with coding
        ///        coefficients
        void encode(uint8_t *symbol_data, const uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                value_type *symbol =
                    reinterpret_cast<value_type*>(symbol_data);

                const value_type *id =
                    reinterpret_cast<const value_type*>(symbol_id);

                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    auto coefficient =
                        vector_type::coefficient(i, id);

                    if(coefficient)
                    {
                        const value_type *symbol_i =
                            reinterpret_cast<const value_type*>(
                                SuperCoder::symbol( i ));

                        // Did you forget to set the data on the encoder?
                        assert(symbol_i != 0);

                        if(fifi::is_binary<field_type>::value)
                        {
                            SuperCoder::add(symbol, symbol_i,
                                            SuperCoder::symbol_length());
                        }
                        else
                        {
                            SuperCoder::multiply_add(
                                symbol, symbol_i,
                                coefficient,
                                SuperCoder::symbol_length());
                        }
                    }
                }
            }
    };
}

#endif

