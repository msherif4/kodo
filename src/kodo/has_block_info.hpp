// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_HAS_BLOCK_INFO_HPP
#define KODO_HAS_BLOCK_INFO_HPP

#include <stdint.h>

#include <fifi/fifi_utils.hpp>

namespace kodo
{
    /// Defines several functions to retrieve info about the block size
    template<class SuperCoder>
    class has_block_info : public SuperCoder
    {
    public:

        /// Define the field type
        typedef typename SuperCoder::field_type field_type;

    public:

        /// Constructor
        has_block_info()
            : m_symbols(0),
              m_symbol_size(0),
              m_symbol_length(0)
            {}

        /// @layer{initialize}
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

        /// @layer{symbols}
        uint32_t symbols() const
            {
                return m_symbols;
            }

        /// @layer{symbol_size}
        uint32_t symbol_size() const
            {
                return m_symbol_size;
            }

        /// @layer{symbol_length}
        uint32_t symbol_length() const
            {
                return m_symbol_length;
            }

        /// @layer{block_size}
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

