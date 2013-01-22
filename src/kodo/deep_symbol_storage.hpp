// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_DEEP_SYMBOL_STORAGE_HPP
#define KODO_DEEP_SYMBOL_STORAGE_HPP

#include <cstdint>
#include <vector>

#include <fifi/fifi_utils.hpp>
#include <sak/storage.hpp>

namespace kodo
{
    /// The deep storage implementation. In this context deep
    /// means that the symbol storage contains the entire coding buffer
    /// internally. This is useful in cases where incoming data is to be
    /// decoded and no existing decoding buffer exist.
    template<class SuperCoder>
    class deep_symbol_storage : public SuperCoder
    {
    public:

        /// The container used to store the coding symbols
        typedef std::vector<uint8_t> symbol_container_type;

    public:

        /// @copydoc final_coder_factory::construct()
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                uint32_t max_data_needed =
                    max_symbols * max_symbol_size;

                assert(max_data_needed > 0);
                m_data.resize(max_data_needed, 0);
            }

        /// @copydoc final_coder_factory::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                std::fill(m_data.begin(), m_data.end(), 0);
            }

        /// @copydoc symbol_storage_shallow::raw_symbol()
        const uint8_t* raw_symbol(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());
                return reinterpret_cast<const uint8_t*>(
                    symbol(index));
            }

        /// @param index the index number of the symbol
        /// @return value_type pointer to the symbol
        uint8_t* symbol(uint32_t index)
            {
                assert(index < SuperCoder::symbols());
                return &m_data[index * SuperCoder::symbol_size()];
            }

        /// @param index the index number of the symbol
        /// @return value_type pointer to the symbol
        const uint8_t* symbol(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());
                return &m_data[index * SuperCoder::symbol_size()];
            }

        /// Set the symbols by swapping the std::vector
        void swap_symbols(symbol_container_type &symbols)
            {
                assert(m_data.size() == symbols.size());
                m_data.swap(symbols);
            }
        
        /// @copydoc symbol_storage_shallow::set_symbols()
        void set_symbols(const sak::const_storage &symbol_storage)
            {
                assert(symbol_storage.m_size > 0);
                assert(symbol_storage.m_data != 0);
                assert(symbol_storage.m_size <=
                       SuperCoder::symbols() * SuperCoder::symbol_size());

                /// Use the copy function
                copy_storage(sak::storage(m_data), symbol_storage);
            }
        
        /// @copydoc symbol_storage_shallow::set_symbol()
        void set_symbol(uint32_t index, const sak::const_storage &symbol)
            {
                assert(symbol.m_data != 0);
                assert(symbol.m_size == SuperCoder::symbol_size());
                assert(index < SuperCoder::symbols());

                sak::mutable_storage data = sak::storage(m_data);

                uint32_t offset = index * SuperCoder::symbol_size();
                data += offset;

                /// Copy the data
                copy_storage(data, symbol);
            }

        /// @copydoc symbol_storage_shallow::copy_symbols()
        void copy_symbols(sak::mutable_storage dest_storage)
            {
                assert(dest_storage.m_size > 0);
                assert(dest_storage.m_data != 0);

                uint32_t data_to_copy = std::min(dest_storage.m_size,
                                                 SuperCoder::block_size());

                /// Wrap our buffer in a storage object
                sak::const_storage src_storage = sak::storage(data(), data_to_copy);

                /// Use the copy_storage() function to copy the data
                sak::copy_storage(dest_storage, src_storage);
            }

        /// Access to the data of the block
        /// @return a pointer to the data of the block
        const uint8_t* data() const
            {
                return &m_data[0];
            }

    private:

        /// Storage for the symbol data
        symbol_container_type m_data;
    };
}

#endif

