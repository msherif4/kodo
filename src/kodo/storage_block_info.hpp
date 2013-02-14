// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_STORAGE_BLOCK_INFO_HPP
#define KODO_STORAGE_BLOCK_INFO_HPP

#include <fifi/fifi_utils.hpp>

namespace kodo
{
    /// @ingroup storage_layers
    /// @brief Defines several functions to retrieve info about the block size
    template<class SuperCoder>
    class storage_block_info : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

    public:

        /// Constructor
        storage_block_info()
            : m_symbols(0),
              m_symbol_size(0),
              m_symbol_length(0)
            {}

        /// @copydoc layer::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                m_symbols = symbols;
                m_symbol_size = symbol_size;

                assert(m_symbols > 0);
                assert(m_symbol_size > 0);

                m_symbol_length =
                    fifi::elements_needed<field_type>(symbol_size);
            }

        /// @copydoc layer::symbols()
        uint32_t symbols() const
            {
                return m_symbols;
            }

        /// @copydoc layer::symbol_size()
        uint32_t symbol_size() const
            {
                return m_symbol_size;
            }

        /// @copydoc layer::symbol_length()
        uint32_t symbol_length() const
            {
                return m_symbol_length;
            }

        /// @copydoc layer::block_size()
        uint32_t block_size() const
            {
                return m_symbols * m_symbol_size;
            }

    protected:

        /// The number of symbols store
        uint32_t m_symbols;

        /// The size of a symbol in bytes
        uint32_t m_symbol_size;

        /// The length of a symbol in value_type elements
        uint32_t m_symbol_length;
    };
}

#endif

