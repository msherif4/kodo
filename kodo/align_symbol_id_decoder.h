// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_ALIGN_SYMBOL_ID_DECODER_H
#define KODO_ALIGN_SYMBOL_ID_DECODER_H

#include <cstdint>

namespace kodo
{

    /// Implementes basic linear block decoder. The linear block decoder
    /// expects that an encoded symbol is described by a vector of coefficients.
    /// Using these coefficients the block decoder subtracts incomming symbols
    /// until the original data has been recreated.
    template<class SuperCoder>
    class align_symbol_id_decoder : public SuperCoder
    {
    public:

        /// @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_temp_id.resize(SuperCoder::symbol_id_size());

                assert((((uintptr_t)&m_temp_id[0]) & 15) == 0);
            }

        /// The decode function which consumes an incomming symbol and
        /// the corresponding symbol_id
        /// @param symbol_data the encoded symbol
        /// @param symbol_id the coefficients used to create the encoded symbol
        void decode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                if((((uintptr_t)symbol_id) & 15) != 0)
                {
                    std::copy(symbol_id,
                              symbol_id + SuperCoder::symbol_id_size(),
                              &m_temp_id[0]);

                    SuperCoder::decode(symbol_data, &m_temp_id[0]);
                }
                else
                {
                    SuperCoder::decode(symbol_data, symbol_id);
                }
            }

    protected:

        /// Temp symbol id (with aligned memory)
        std::vector<uint8_t> m_temp_id;

    };
}

#endif

