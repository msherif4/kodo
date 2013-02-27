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

        /// @todo this should not be necessary right?
        // Make sure all overloads of the decode symbol function is available
        using SuperCoder::decode_symbol;

        /// @copydoc layer::pointer
        typedef typename SuperCoder::pointer pointer;

    public:

        /// @ingroup factory_layers
        /// The factory layer associated with this coder.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @copydoc layer::factory::build(uint32_t,uint32_t)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    auto coder =
                        SuperCoder::factory::build(symbols, symbol_size);

                    /// @todo what happens if multiple functions with this name
                    ///       is in the hierarchy
                    coder->initialize_storage(
                        SuperCoder::factory::max_coefficients_size());

                    return coder;
                }
        };

    public:

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data, uint8_t *symbol_coefficients)
            {
                assert(symbol_data != 0);
                assert(symbol_coefficients != 0);

                if(sak::is_aligned(symbol_coefficients) == false)
                {
                    std::copy(
                        symbol_coefficients,
                        symbol_coefficients + SuperCoder::coefficients_size(),
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

        /// Initialize the coefficient storage
        /// @param max_coefficients_size The maximum size of the coding
        ///        coefficients in bytes
        void initialize_storage(uint32_t max_coefficients_size)
            {
                assert(max_coefficients_size > 0);

                // Note, that resize will not re-allocate anything
                // as long as the sizes are not larger than
                // previously. So this call should only have an
                // effect the first time this function is called.
                m_aligned_coefficients.resize(max_coefficients_size);
            }

    private:

        /// The storage type
        typedef std::vector<uint8_t, sak::aligned_allocator<uint8_t> >
            aligned_vector;

        /// Temp symbol id (with aligned memory)
        aligned_vector m_aligned_coefficients;

    };
}

#endif

