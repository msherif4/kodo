// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_LINEAR_BLOCK_ENCODER_HPP
#define KODO_LINEAR_BLOCK_ENCODER_HPP

#include <cstdint>

#include <fifi/is_binary.hpp>
#include <fifi/fifi_utils.hpp>

#include <sak/storage.hpp>

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

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

    public:

        /// @copydoc layer::encode_symbol(uint8_t*,uint32_t)
        void encode_symbol(uint8_t *symbol_data, uint32_t symbol_index)
            {
                // Copy the symbol
                assert(symbol_index < SuperCoder::symbols());

                sak::mutable_storage dest =
                    sak::storage(symbol_data, SuperCoder::symbol_size());

                SuperCoder::copy_symbol(symbol_index, dest);
            }


        /// @copydoc layer::encode_symbol(uint8_t*,uint8_t*)
        void encode_symbol(uint8_t *symbol_data, const uint8_t *coefficients)
            {
                assert(symbol_data != 0);
                assert(coefficients != 0);

                value_type *symbol =
                    reinterpret_cast<value_type*>(symbol_data);

                const value_type *c =
                    reinterpret_cast<const value_type*>(coefficients);

                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    value_type value = fifi::get_value<field_type>(c, i);

                    if(!value)
                    {
                        continue;
                    }

                    const value_type *symbol_i =
                        SuperCoder::symbol_value( i );

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
                            symbol, symbol_i, value,
                            SuperCoder::symbol_length());
                    }
                }
            }
    };

}

#endif

