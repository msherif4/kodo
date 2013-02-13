// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_SYSTEMATIC_DECODER_HPP
#define KODO_SYSTEMATIC_DECODER_HPP

#include <stdint.h>
#include <sak/convert_endian.hpp>

#include "systematic_base_coder.hpp"

namespace kodo
{
    /// Systematic encoder layer
    template<class SuperCoder>
    class systematic_decoder : public SuperCoder
    {
    public:

        /// The field type
        typedef typename SuperCoder::field_type field_type;

        /// The value type
        typedef typename field_type::value_type value_type;

        /// The symbol count type
        typedef typename systematic_base_coder::counter_type
            counter_type;

        /// The flag type
        typedef typename systematic_base_coder::flag_type
            flag_type;

        using SuperCoder::decode_symbol;

    public:

        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc final_coder_factory::factory::factory()
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @return the required payload buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    return SuperCoder::factory::max_symbol_id_size() +
                        sizeof(flag_type) + sizeof(counter_type);
                }
        };

    public:

        /// Iterates over the symbols stored in the encoding symbol id part
        /// of the payload id, and calls the encode_symbol function.
        /// @copydoc linear_block_decoder::decode()
        void decode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                flag_type flag =
                    sak::big_endian::get<flag_type>(symbol_id);

                symbol_id += sizeof(flag_type);

                if(flag == systematic_base_coder::systematic_flag)
                {
                    /// Get symbol index and copy the symbol
                    counter_type symbol_index =
                        sak::big_endian::get<counter_type>(symbol_id);

                    SuperCoder::decode_symbol(symbol_data, symbol_index);
                }
                else
                {
                    /// @todo call decode(..) here not decode_symbol()
                    SuperCoder::decode_symbol(symbol_data, symbol_id);
                }
            }

        /// @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return SuperCoder::symbol_id_size() +
                    sizeof(flag_type) + sizeof(counter_type);
            }
    };
}

#endif

