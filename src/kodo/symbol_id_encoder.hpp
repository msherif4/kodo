// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

namespace kodo
{

    /// @ingroup codec_header_layers
    ///
    /// @brief Writes the symbol id into the symbol header. Generates the
    ///        the symbol coding coefficients. and calls the
    ///        layer::encode_symbol() function.
    template<class SuperCoder>
    class symbol_id_encoder : public SuperCoder
    {
    public:

        /// @ingroup factory_layers
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

            /// @copydoc layer::factory::max_header_size() const
            uint32_t max_header_size() const
            {
                return SuperCoder::factory::max_id_size();
            }

        };

    public:

        /// @copydoc layer::encode(uint8_t*, uint8_t*)
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_header)
        {
            assert(symbol_data != 0);
            assert(symbol_header != 0);

            uint8_t *coefficients = 0;

            uint32_t bytes_used =
                SuperCoder::write_id(symbol_header, &coefficients);

            assert(coefficients != 0);

            SuperCoder::encode_symbol(symbol_data, coefficients);

            return bytes_used;
        }

        /// @copydoc layer::header_size() const
        uint32_t header_size() const
        {
            return SuperCoder::id_size();
        }

    };

}


