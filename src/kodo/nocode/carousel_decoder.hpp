// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_CAROUSEL_DECODER_HPP
#define KODO_CAROUSEL_DECODER_HPP

#include <sak/convert_endian.hpp>

#include "carousel_common.hpp"

namespace kodo
{
    /// Carousel decoder takes symbols produced by a carousel encoder
    /// and decodes them.
    /// @see carousel_encoder
    template<class SuperCoder>
    class carousel_decoder : public SuperCoder
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

        /// @copydoc linear_block_decoder::decode()
        void decode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                id_type id = sak::big_endian::get<id_type>(symbol_id);

                SuperCoder::decode_raw(symbol_data, id);
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

