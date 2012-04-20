// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_LINEAR_BLOCK_ENCODER_H
#define KODO_LINEAR_BLOCK_ENCODER_H

#include <stdint.h>

#include "full_vector.h"

namespace kodo
{

    /// A linear block encoder. This type of encoder iterates
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
        typedef full_vector<field_type> vector_type;

    public:

        /// Iterates over the symbols stored in the encoding symbol id part
        /// of the payload id, and calls the encode_symbol function.
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                // Get the data for the encoding vector
                value_type *vector
                    = reinterpret_cast<value_type*>(symbol_id);

                // Cast the symbol to the correct field value_type
                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                encode_with_vector(symbol, vector);

                return m_vector_size;
            }

    protected:

        /// Encodes a symbol according to the encoding vector
        /// @param symbol_data the destination buffer for the encoded symbol
        /// @param vector_data the encoding vector - note at this point the
        ///        encoding vector should already be initialized with coding
        ///        coefficients.
        void encode_with_vector(value_type *symbol_data, value_type *vector_data)
            {
                assert(symbol_data != 0);
                assert(vector_data != 0);

                vector_type encoding_vector(vector_data, SuperCoder::symbols());

                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    if(value_type coefficient = encoding_vector.coefficient(i))
                    {
                        const value_type *symbol_i = SuperCoder::symbol( i );

                        // Did you forget to set the data on the encoder?
                        assert(symbol_i != 0);

                        if(fifi::is_binary<field_type>::value)
                        {
                            SuperCoder::add(symbol_data, symbol_i,
                                            SuperCoder::symbol_length());
                        }
                        else
                        {
                            SuperCoder::multiply_add(symbol_data, symbol_i,
                                                     coefficient,
                                                     SuperCoder::symbol_length());
                        }
                    }
                }
            }

    };
}

#endif

