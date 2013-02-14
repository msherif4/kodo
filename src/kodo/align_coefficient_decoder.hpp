// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_ALIGN_COEFFICIENT_DECODER_HPP
#define KODO_ALIGN_COEFFICIENT_DECODER_HPP

#include <cstdint>

#include <sak/aligned_allocator.hpp>
#include <sak/is_aligned.hpp>

namespace kodo
{
    /// @ingroup codec_layers
    /// @brief Aligns the symbol coefficient buffer if necessary
    template<class SuperCoder>
    class align_coefficient_decoder : public SuperCoder
    {
    public:

        // Make sure all overloads of the decode symbol function is available
        using SuperCoder::decode_symbol;

    public:

        /// @copydoc layer::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_aligned_coefficients.resize(SuperCoder::symbol_id_size());

                assert(sak::is_aligned(&m_aligned_coefficients[0]));
            }

        /// @copydoc layer::decode_symbol()
        void decode_symbol(uint8_t *symbol_data,
                           uint8_t *symbol_coefficients)
            {
                assert(symbol_data != 0);
                assert(symbol_coefficients != 0);

                if(sak::is_aligned(symbol_coefficients) == false)
                {
                    /// @todo chanage the symbol_id_size to coefficient_size
                    ///
                    std::copy(symbol_coefficients,
                              symbol_coefficients + SuperCoder::symbol_id_size(),
                              &m_aligned_coefficients[0]);

                    SuperCoder::decode_symbol(
                        symbol_data, &m_aligned_coefficients[0]);
                }
                else
                {
                    SuperCoder::decode_symbol(
                        symbol_data, symbol_coefficients);
                }
            }

    private:

        /// The storage type
        typedef std::vector<uint8_t, sak::aligned_allocator<uint8_t> >
            aligned_vector;

    protected:

        /// Temp symbol id (with aligned memory)
        aligned_vector m_aligned_coefficients;

    };
}

#endif

