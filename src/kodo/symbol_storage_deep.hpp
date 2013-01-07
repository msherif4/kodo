// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_SYMBOL_STORAGE_DEEP_HPP
#define KODO_SYMBOL_STORAGE_DEEP_HPP

#include <stdint.h>
#include <vector>

#include <boost/shared_array.hpp>

#include <fifi/fifi_utils.hpp>

#include "storage.hpp"

namespace kodo
{
    /// The deep storage implementation. In this context deep
    /// means that the symbol storage contains the entire coding buffer
    /// internally. This is useful in cases where incoming data is to be
    /// decoded and no existing decoding buffer exist.
    template<class SuperCoder>
    class symbol_storage_deep : public SuperCoder
    {
    public:

        /// @see final_coder::construct(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                uint32_t max_data_needed =
                    max_symbols * max_symbol_size;

                assert(max_data_needed > 0);
                m_data.resize(max_data_needed, 0);
            }

        /// @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                std::fill(m_data.begin(), m_data.end(), 0);
            }

        /// @return uint8_t pointer to the symbol
        const uint8_t* raw_symbol(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());
                return reinterpret_cast<const uint8_t*>(
                    symbol(index));
            }

        /// @return value_type pointer to the symbol
        uint8_t* symbol(uint32_t index)
            {
                assert(index < SuperCoder::symbols());
                return &m_data[index * SuperCoder::symbol_size()];
            }

        /// @return value_type pointer to the symbol
        const uint8_t* symbol(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());
                return &m_data[index * SuperCoder::symbol_size()];
            }

        /// Sets the storage
        /// @param storage a const storage container
        void set_symbols(const const_storage &symbol_storage)
            {
                assert(symbol_storage.m_size > 0);
                assert(symbol_storage.m_data != 0);
                assert(symbol_storage.m_size ==
                       SuperCoder::symbols() * SuperCoder::symbol_size());

                /// Use the copy function
                copy_storage(storage(m_data), symbol_storage);
            }

        /// Sets a symbol - by copying it into the right location in the buffer
        /// @param index the index of the symbol into the coding block
        /// @param symbol the actual data of that symbol
        void set_symbol(uint32_t index, const const_storage &symbol)
            {
                assert(symbol.m_data != 0);
                assert(symbol.m_size == SuperCoder::symbol_size());
                assert(index < SuperCoder::symbols());

                mutable_storage data = storage(m_data);

                uint32_t offset = index * SuperCoder::symbol_size();
                data += offset;

                /// Copy the data
                copy_storage(data, symbol);
            }

        /// Create an overload of the copy_storage(...) function for this symbol
        /// storage.
        void copy_symbols(mutable_storage dest_storage)
            {
                assert(dest_storage.m_size > 0);
                assert(dest_storage.m_data != 0);

                uint32_t data_to_copy = std::min(dest_storage.m_size,
                                                 SuperCoder::block_size());

                /// Wrap our buffer in a storage object
                const_storage src_storage = storage(data(), data_to_copy);

                /// Use the copy_storage(...) function to copy the data
                copy_storage(dest_storage, src_storage);
            }


        /// Access to the data of the block
        /// @return a pointer to the data of the block
        const uint8_t* data() const
            {
                return &m_data[0];
            }

    private:

        /// Storage for the symbol data
        std::vector<uint8_t> m_data;
    };
}

#endif

