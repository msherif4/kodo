// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_SYMBOL_STORAGE_TRACKER_HPP
#define KODO_SYMBOL_STORAGE_TRACKER_HPP

#include <cstdint>
#include <vector>

#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup symbol_storage_layers
    /// The storage tracker layer intercepts the calls used to
    /// initialize the symbol storage and allow us to track
    /// which symbols have been specified.
    template<class SuperCoder>
    class symbol_storage_tracker : public SuperCoder
    {
    public:

        /// Constructor
        symbol_storage_tracker()
            : m_symbols_count(0)
            { }

        /// @copydoc layer::construct(uint32_t,uint32_t)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);
                m_symbols.resize(max_symbols, false);
            }

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                std::fill(m_symbols.begin(), m_symbols.end(), false);
                m_symbols_count = 0;
            }

        /// @copydoc layer::set_symbols(const sak::const_storage&)
        void set_symbols(const sak::const_storage &symbol_storage)
            {
                SuperCoder::set_symbols(symbol_storage);
                std::fill(m_symbols.begin(), m_symbols.end(), true);

                // This should set all symbols
                assert(m_symbols_count == 0);
                m_symbols_count = SuperCoder::symbols();
            }

        /// @copydoc layer::set_symbols(const sak::mutable_storage&)
        void set_symbols(const sak::mutable_storage &symbol_storage)
            {
                SuperCoder::set_symbols(symbol_storage);
                std::fill(m_symbols.begin(), m_symbols.end(), true);

                // This should set all symbols
                assert(m_symbols_count == 0);
                m_symbols_count = SuperCoder::symbols();
            }

        /// @copydoc layer::set_symbol(uint32_t,const sak::mutable_storage&)
        void set_symbol(uint32_t index, const sak::mutable_storage &symbol)
            {
                assert(index < SuperCoder::symbols());

                SuperCoder::set_symbol(index, symbol);

                // The symbol should not already exist
                assert(m_symbols[index] == false);
                m_symbols[index] = true;

                m_symbols_count += 1;
            }

        /// @copydoc layer::set_symbol(uint32_t, const sak::const_storage&)
        void set_symbol(uint32_t index, const sak::const_storage &symbol)
            {
                assert(index < SuperCoder::symbols());

                SuperCoder::set_symbol(index, symbol);

                // The symbol should not already exist
                assert(m_symbols[index] == false);
                m_symbols[index] = true;

                m_symbols_count += 1;
            }

        /// @copydoc layer::swap_symbols(std::vector<const uint8_t*>&)
        void swap_symbols(std::vector<const uint8_t *> &symbols)
            {
                SuperCoder::swap_symbols(symbols);
                std::fill(m_symbols.begin(), m_symbols.end(), true);

                // This should set all symbols
                assert(m_symbols_count == 0);
                m_symbols_count = SuperCoder::symbols();
            }

        /// @copydoc layer::swap_symbols(std::vector<uint8_t*>&)
        void swap_symbols(std::vector<uint8_t *> &symbols)
            {
                SuperCoder::swap_symbols(symbols);
                std::fill(m_symbols.begin(), m_symbols.end(), true);

                // This should set all symbols
                assert(m_symbols_count == 0);
                m_symbols_count = SuperCoder::symbols();
            }

        /// @copydoc layer::swap_symbols(std::vector<uint8_t>&)
        void swap_symbols(std::vector<uint8_t> &symbols)
            {
                SuperCoder::swap_symbols(symbols);
                std::fill(m_symbols.begin(), m_symbols.end(), true);

                // This should set all symbols
                assert(m_symbols_count == 0);
                m_symbols_count = SuperCoder::symbols();
            }

        /// @copydoc layer::symbol_exists(uint32_t) const
        bool symbol_exists(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());
                return m_symbols[index];
            }

        /// @copydoc layer::symbol_count() const
        uint32_t symbol_count() const
            {
                return m_symbols_count;
            }

        /// @copydoc layer::is_storage_full() const
        bool is_storage_full() const
            {
                return m_symbols_count == SuperCoder::symbols();
            }

    protected:

        /// Symbols count
        uint32_t m_symbols_count;

        /// Tracks which symbols have been set
        std::vector<bool> m_symbols;

    };
}

#endif

