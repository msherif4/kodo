// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

namespace kodo
{

    /// @ingroup payload_codec_layers
    /// @brief The payload decoder splits the payload buffer into
    ///        symbol header and symbol.
    template<class SuperCoder>
    class payload_decoder : public SuperCoder
    {
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

            /// @copydoc layer::factory::max_payload_size() const
            uint32_t max_payload_size() const
            {
                return SuperCoder::factory::max_symbol_size() +
                    SuperCoder::factory::max_header_size();
            }
        };

    public:

        /// Unpacks the symbol data and symbol header from the payload
        /// buffer.
        /// @copydoc layer::decode(uint8_t*)
        void decode(uint8_t *payload)
        {
            assert(payload != 0);

            uint8_t *symbol_data = payload;
            uint8_t *symbol_id = payload + SuperCoder::symbol_size();

            SuperCoder::decode(symbol_data, symbol_id);
        }

        /// @copydoc layer::payload_size() const
        uint32_t payload_size() const
        {
            return SuperCoder::symbol_size() +
                SuperCoder::header_size();
        }
    };

}


