// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_PLAIN_SYMBOL_ID_WRITER_HPP
#define KODO_PLAIN_SYMBOL_ID_WRITER_HPP

#include <cstdint>

#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @brief Writes the symbol id and initializes the coding
    ///        coefficients pointer. This implementation is the most
    ///        basic one where the symbol id simply is the coding
    ///        coefficients.
    ///
    /// @ingroup symbol_id_layers
    /// @ingroup factory_layers
    template<class SuperCoder>
    class plain_symbol_id_writer : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type;
        typedef typename SuperCoder::field_type field_type;

    public:

        /// @ingroup factory_layers
        /// The factory layer associated with this coder.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory()
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @copydoc layer::factory::max_id_size()
            uint32_t max_id_size() const
                {
                    return SuperCoder::factory::max_coefficients_size();
                }
        };

    public:

        /// @copydoc layer::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                m_id_size = SuperCoder::coefficients_size();
                assert(m_id_size > 0);
            }

        /// @copydoc layer::write_id()
        uint32_t write_id(uint8_t *symbol_id, uint8_t **symbol_coefficients)
            {
                assert(symbol_id != 0);
                assert(symbol_coefficients != 0);

                SuperCoder::generate(symbol_id);
                *symbol_coefficients = symbol_id;

                return m_id_size;
            }

        /// @copydoc layer::id_size()
        uint32_t id_size() const
            {
                return m_id_size;
            }

    protected:

        /// The number of bytes needed to store the symbol id
        /// i.e. the coding coefficients
        uint32_t m_id_size;

    };

}

#endif

