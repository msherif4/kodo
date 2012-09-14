// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_ALIGN_SYMBOL_ID_ENCODER_H
#define KODO_ALIGN_SYMBOL_ID_ENCODER_H

#include <cstdint>

namespace kodo
{

    /// Looks at the symbol id buffer and checks whether it is aligned
    /// if not it makes a copy into an aligned buffer.
    template<class SuperCoder>
    class align_symbol_id_encoder : public SuperCoder
    {
    public:

        /// @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_temp_id.resize(SuperCoder::symbol_id_size());

                assert((((uintptr_t)&m_temp_id[0]) & 15) == 0);
            }

        /// The encode function which produces an incoming symbol and
        /// the corresponding symbol_id
        /// @param symbol_data the encoded symbol
        /// @param symbol_id the coefficients used to create the encoded symbol
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                if((((uintptr_t)symbol_id) & 15) != 0)
                {
                    uint32_t used =
                        SuperCoder::encode(symbol_data, &m_temp_id[0]);

                    std::copy(&m_temp_id[0],
                              &m_temp_id[0] + used,
                              symbol_id);

                    return used;
                }
                else
                {
                    return SuperCoder::encode(symbol_data, symbol_id);
                }
            }

    protected:

        /// Temp symbol id (with aligned memory)
        std::vector<uint8_t> m_temp_id;

    };
}

#endif

