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

    /// @ingroup storage_layers
    /// @brief The deep storage implementation. In this context deep
    ///        means that the symbol storage contains the entire coding buffer
    ///        internally.
    ///
    /// This is useful in cases where incoming data is to be
    /// decoded and no existing decoding buffer exist.
    template<class SuperCoder>
    class deep_symbol_storage : public SuperCoder
    {
    public:

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

    public:

        /// @copydoc layer::construct(uint32_t,uint32_t)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                uint32_t max_data_needed =
                    max_symbols * max_symbol_size;

                assert(max_data_needed > 0);

                // Construct should only be called once so
                // m_data.size() should be zero
                assert(m_data.size() == 0);
                m_data.resize(max_data_needed, 0);
            }

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                std::fill(m_data.begin(), m_data.end(), 0);
            }

        /// @copydoc layer::symbol(uint32_t)
        uint8_t* symbol(uint32_t index)
            {
                assert(index < SuperCoder::symbols());
                return &m_data[index * SuperCoder::symbol_size()];
            }

        /// @copydoc layer::symbol_value(uint32_t)
        value_type* symbol_value(uint32_t index)
            {
                return reinterpret_cast<value_type*>(symbol(index));
            }


        /// @copydoc layer::symbol(uint32_t) const
        const uint8_t* symbol(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());
                return &m_data[index * SuperCoder::symbol_size()];
            }

        /// @copydoc layer::symbol_value(uint32_t) const
        const value_type* symbol_value(uint32_t index) const
            {
                return reinterpret_cast<const value_type*>(symbol(index));
            }

        /// @copydoc layer::swap_symbols(std::vector<uint8_t> &)
        void swap_symbols(std::vector<uint8_t> &symbols)
            {
                assert(m_data.size() == symbols.size());
                m_data.swap(symbols);
            }

        /// @copydoc layer::set_symbols(const sak::const_storage&)
        void set_symbols(const sak::const_storage &symbol_storage)
            {
                assert(symbol_storage.m_size > 0);
                assert(symbol_storage.m_data != 0);
                assert(symbol_storage.m_size <=
                       SuperCoder::symbols() * SuperCoder::symbol_size());

                /// Use the copy function
                copy_storage(sak::storage(m_data), symbol_storage);
            }

        /// @copydoc layer::set_symbol(uint32_t, const sak::const_storage&)
        void set_symbol(uint32_t index, const sak::const_storage &symbol)
            {
                assert(symbol.m_data != 0);
                assert(symbol.m_size == SuperCoder::symbol_size());
                assert(index < SuperCoder::symbols());

                sak::mutable_storage dest_data = sak::storage(m_data);

                uint32_t offset = index * SuperCoder::symbol_size();
                dest_data += offset;

                assert(dest_data.m_size >= SuperCoder::symbol_size());

                /// Copy the data
                sak::copy_storage(dest_data, symbol);
            }

        /// @copydoc layer::copy_symbols(const sak::mutable_storage&)
        void copy_symbols(const sak::mutable_storage &dest_storage)
            {
                assert(dest_storage.m_size > 0);
                assert(dest_storage.m_data != 0);

                uint32_t data_to_copy =
                    std::min(dest_storage.m_size, SuperCoder::block_size());

                /// Wrap our buffer in a storage object
                sak::const_storage src_storage =
                    sak::storage(&m_data[0], data_to_copy);

                /// Use the copy_storage() function to copy the data
                sak::copy_storage(dest_storage, src_storage);
            }

        /// @copydoc layer::copy_symbol(uint32_t, const sak::mutable_storage&)
        void copy_symbol(uint32_t index,
                         const sak::mutable_storage &dest) const
            {
                assert(dest.m_size > 0);
                assert(dest.m_data != 0);

                uint32_t data_to_copy =
                    std::min(dest.m_size, SuperCoder::symbol_size());

                sak::const_storage src =
                    sak::storage(symbol(index), data_to_copy);

                sak::copy_storage(dest, src);
            }

    private:

        /// Storage for the symbol data
        std::vector<uint8_t> m_data;

    };
}

#endif

