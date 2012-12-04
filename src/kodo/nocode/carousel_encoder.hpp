// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_CAROUSEL_ENCODER_HPP
#define KODO_CAROUSEL_ENCODER_HPP

#include <sak/convert_endian.hpp>

#include "carousel_common.hpp"

namespace kodo
{
    /// Simple un-coded scheme not meant for practical use but
    /// for simulation purposes. As it simply produces un-coded
    /// symbols.
    ///
    /// Works like a systematic encoder only that it restarts
    /// the systematic transmission from the beginning after
    /// sending the last symbol.
    ///
    /// Example sending sequence 4 symbols:
    /// 1,2,3,4, 1,2,3,4, 1,2,3,4 ... , 1,2,3,4
    ///
    /// Simply repeats the raw un-coded symbols.
    template<class SuperCoder>
    class carousel_encoder : public SuperCoder
    {
    public:

        /// The symbol id
        typedef carousel_common::id_type id_type;

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

            /// @return the required symbol_id buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    return sizeof(id_type);
                }
        };


    public:
        /// @copydoc final_coder_factory::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_current_symbol = 0;
            }

        /// Pick a symbol one after another
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                /// Flag systematic packet
                sak::big_endian::put<id_type>(
                    m_current_symbol, symbol_id);

                /// Copy the symbol
                SuperCoder::encode_raw(symbol_data, m_current_symbol);

                m_current_symbol =
                    (m_current_symbol + 1) % SuperCoder::symbols();

                return sizeof(id_type);
            }

        /// @return the required symbol_id buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return sizeof(id_type);
            }

    private:

        /// Keeps track of the current symbol id
        uint32_t m_current_symbol;
    };
}

#endif

