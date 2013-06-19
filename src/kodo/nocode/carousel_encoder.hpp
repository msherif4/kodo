// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <sak/convert_endian.hpp>

#include "carousel_common.hpp"

namespace kodo
{

    /// @ingroup codec_header_layers
    /// @brief Simple un-coded scheme not meant for practical use but
    ///        for simulation purposes. As it simply produces un-coded symbols.
    ///
    /// Works like a systematic encoder only that it restarts the systematic
    /// transmission from the beginning after sending the last symbol.
    ///
    /// Example sequence of 4 symbols: 1,2,3,4, 1,2,3,4, 1,2,3,4 ... , 1,2,3,4
    ///
    /// Simply repeats the raw un-coded symbols.
    template<class SuperCoder>
    class carousel_encoder : public SuperCoder
    {
    public:

        /// The symbol id
        typedef carousel_common::id_type id_type;

    public:

        /// The factory layer associated with this coder. In this case only
        /// needed to provide the max_payload_size() function.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
            { }

            /// @copydoc layer::factory::max_header_size() const
            uint32_t max_header_size() const
            {
                return sizeof(id_type);
            }
        };

    public:

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            SuperCoder::initialize(the_factory);
            m_current_symbol = 0;
        }

        /// @copydoc layer::encode(uint8_t*, uint8_t*)
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_header)
        {
            assert(symbol_data != 0);
            assert(symbol_header != 0);

            // Write the symbol id in the header
            sak::big_endian::put<id_type>(
                m_current_symbol, symbol_header);

            assert(m_current_symbol < SuperCoder::symbols());

            SuperCoder::encode_symbol(symbol_data, m_current_symbol);

            m_current_symbol =
                (m_current_symbol + 1) % SuperCoder::symbols();

            return sizeof(id_type);
        }

        /// @copydoc layer::header_size() const
        uint32_t header_size() const
        {
            return sizeof(id_type);
        }

    private:

        /// Keeps track of the current symbol id
        uint32_t m_current_symbol;

    };

}


