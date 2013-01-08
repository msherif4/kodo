// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_ALIGN_SYMBOL_ID_DECODER_HPP
#define KODO_ALIGN_SYMBOL_ID_DECODER_HPP

#include <cstdint>

#include <sak/aligned_allocator.hpp>
#include <sak/is_aligned.hpp>

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

        /// @copydoc final_coder_factory::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_temp_id.resize(SuperCoder::symbol_id_size());

                assert(sak::is_aligned(&m_temp_id[0]));
            }

        /// The decode function which consumes an incomming symbol and
        /// the corresponding symbol_id
        /// @param symbol_data the encoded symbol
        /// @param symbol_id the coefficients used to create the encoded symbol
        void decode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                if(sak::is_aligned(symbol_id) == false)
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

    private:

        /// The storage type
        typedef std::vector<uint8_t, sak::aligned_allocator<uint8_t> >
            aligned_vector;

    protected:

        /// Temp symbol id (with aligned memory)
        aligned_vector m_temp_id;

    };
}

#endif

