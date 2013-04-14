// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <sak/convert_endian.hpp>

#include "systematic_base_coder.hpp"

namespace kodo
{

    /// @ingroup codec_header_layers
    /// @brief Systematic decoding layer.
    template<class SuperCoder>
    class systematic_decoder : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

        /// The symbol count type
        typedef typename systematic_base_coder::counter_type
            counter_type;

        /// The flag type
        typedef typename systematic_base_coder::flag_type
            flag_type;

    public:

        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
            { }

            /// @copydoc layer::max_header_size() const
            uint32_t max_header_size() const
            {
                return SuperCoder::factory::max_header_size() +
                    sizeof(flag_type) + sizeof(counter_type);
            }
        };

    public:

        /// Looks for the systematic flag in the symbol_header. If the
        /// symbol is systematic directly pass it to the Codec Layers
        /// otherwise pass it to the next Codec Header Layer.
        ///
        /// @copydoc layer::decode(uint8_t*, uint8_t*)
        void decode(uint8_t *symbol_data, uint8_t *symbol_header)
        {
            assert(symbol_data != 0);
            assert(symbol_header != 0);

            flag_type flag =
                sak::big_endian::get<flag_type>(symbol_header);

            symbol_header += sizeof(flag_type);

            if(flag == systematic_base_coder::systematic_flag)
            {
                /// Get symbol index and copy the symbol
                counter_type symbol_index =
                    sak::big_endian::get<counter_type>(symbol_header);

                SuperCoder::decode_symbol(symbol_data, symbol_index);
            }
            else
            {
                SuperCoder::decode(symbol_data, symbol_header);
            }
        }

        /// @copydoc layer::header_size() const
        uint32_t header_size() const
        {
            return SuperCoder::header_size() +
                sizeof(flag_type) + sizeof(counter_type);
        }
    };

}


