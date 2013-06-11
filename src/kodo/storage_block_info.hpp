// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup symbol_storage_layers
    ///
    /// @brief Defines several functions to retrieve info about the
    ///        symbol storage layers.
    template<class SuperCoder>
    class storage_block_info : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

    public:

        /// @ingroup factory_layers
        /// @brief Provides access to the maximum symbol and symbol size
        ///        information.
        class factory : public SuperCoder::factory
        {
        public:

            /// Constructor
            /// @param max_symbols the maximum symbols this coder can expect
            /// @param max_symbol_size the maximum size of a symbol in bytes
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size),
                  m_max_symbols(max_symbols),
                  m_max_symbol_size(max_symbol_size),
                  m_symbols(max_symbols),
                  m_symbol_size(max_symbol_size)
            {
                assert(m_max_symbols > 0);
                assert(m_max_symbol_size > 0);
                assert(m_symbols > 0);
                assert(m_symbol_size > 0);
            }

            /// @copydoc layer::factory::max_symbols() const
            uint32_t max_symbols() const
            {
                return m_max_symbols;
            }

            /// @copydoc layer::factory::max_symbol_size() const
            uint32_t max_symbol_size() const
            {
                return m_max_symbol_size;
            }

            /// @copydoc layer::factory::max_block_size() const
            uint32_t max_block_size() const
            {
                return m_max_symbols*m_max_symbol_size;
            }

            /// @copydoc layer::factory::symbols() const;
            uint32_t symbols() const
            {
                return m_symbols;
            }

            /// @copydoc layer::factory::symbol_size() const;
            uint32_t symbol_size() const
            {
                return m_symbol_size;
            }

            /// @copydoc layer::factory::set_symbols(uint32_t)
            void set_symbols(uint32_t symbols)
            {
                assert(symbols > 0);
                assert(symbols <= m_max_symbols);

                m_symbols = symbols;
            }

            /// @copydoc layer::factory::set_symbol_size(uint32_t)
            void set_symbol_size(uint32_t symbol_size)
            {
                assert(symbol_size > 0);
                assert(symbol_size <= m_max_symbol_size);

                m_symbol_size = symbol_size;
            }

        private:

            /// The maximum number of symbols
            uint32_t m_max_symbols;

            /// The maximum symbol size
            uint32_t m_max_symbol_size;

            /// The number of symbols used
            uint32_t m_symbols;

            /// The symbol size used
            uint32_t m_symbol_size;

        };

    public:

        /// Constructor
        storage_block_info()
            : m_symbols(0),
              m_symbol_size(0),
              m_symbol_length(0)
        { }

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory &the_factory)
        {
            SuperCoder::initialize(the_factory);

            m_symbols = the_factory.symbols();
            m_symbol_size = the_factory.symbol_size();

            assert(m_symbols > 0);
            assert(m_symbol_size > 0);

            m_symbol_length =
                fifi::size_to_length<field_type>(m_symbol_size);
        }

        /// @copydoc layer::symbols() const
        uint32_t symbols() const
        {
            return m_symbols;
        }

        /// @copydoc layer::symbol_size() const
        uint32_t symbol_size() const
        {
            return m_symbol_size;
        }

        /// @copydoc layer::symbol_length() const
        uint32_t symbol_length() const
        {
            return m_symbol_length;
        }

        /// @copydoc layer::block_size() const
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


