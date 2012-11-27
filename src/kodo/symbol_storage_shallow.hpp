// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_SYMBOL_STORAGE_SHALLOW_HPP
#define KODO_SYMBOL_STORAGE_SHALLOW_HPP

#include <stdint.h>
#include <vector>

#include <boost/shared_array.hpp>

#include <fifi/fifi_utils.hpp>

#include "storage.hpp"

namespace kodo
{
    /// Storage traits class for the const storage
    template<class ValueType>
    class shallow_const_trait
    {
    public:

        /// value type pointer
        typedef const ValueType* value_ptr;

        /// storage type
        typedef const_storage storage_type;

    };

    /// Storage traits class for the mutable storage
    template<class ValueType>
    class shallow_mutable_trait
    {
    public:

        /// value type pointer
        typedef ValueType* value_ptr;

        /// storage type
        typedef mutable_storage storage_type;

    };

    /// The shallow storage implementation. In this context shallow
    /// means that the symbol storage only contains pointers to some
    /// external data structure. This is useful in cases where data to
    /// be encoded already has been read into memory or if a user requires
    /// incoming data to be directly decoded into a specific buffer.
    template<template <class> class StorageTraits, class SuperCoder>
    class symbol_storage_shallow : public SuperCoder
    {
    public:

        /// The value type used
        typedef typename SuperCoder::value_type value_type;

        /// The storage traits
        typedef StorageTraits<value_type> storage_trait;

        /// The pointer used
        typedef typename storage_trait::value_ptr value_ptr;

        /// The storage type used
        typedef typename storage_trait::storage_type storage_type;

        /// A sequence of storage types
        typedef typename storage_sequence<storage_type>::type
            storage_sequence_type;

    public:

        /// @see final_coder::construct(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                m_mapping.resize(max_symbols, 0);
            }

        /// @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                std::fill(m_mapping.begin(), m_mapping.end(), (value_ptr) 0);
            }

        /// @return uint8_t pointer to the symbol
        const uint8_t* raw_symbol(uint32_t index) const
            {
                return reinterpret_cast<const uint8_t*>(
                    symbol(index));
            }

        /// @return value_type pointer to the symbol
        value_ptr symbol(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());

                return m_mapping[index];
            }

        /// Sets the storage
        /// @param symbol_storage a const storage container
        void set_symbols(const storage_type &symbol_storage)
            {
                storage_sequence_type symbol_sequence =
                    split_storage(symbol_storage, SuperCoder::symbol_size());

                uint32_t sequence_size = symbol_sequence.size();
                assert(sequence_size == SuperCoder::symbols());

                for(uint32_t i = 0; i < sequence_size; ++i)
                {
                    set_symbol(i, symbol_sequence[i]);
                }
            }

        /// Sets a symbol -> data mapping
        /// @param index the index of the symbol into the coding block
        /// @param symbol the actual data of that symbol
        void set_symbol(uint32_t index, const storage_type &symbol)
            {
                assert(symbol.m_data != 0);
                assert(symbol.m_size == SuperCoder::symbol_size());
                assert(index < SuperCoder::symbols());

                m_mapping[index] = cast_storage<value_type>(symbol);
            }

        /// Create an overload of the copy_storage(...) function for this symbol
        /// storage.
        void copy_symbols(mutable_storage dest_storage)
            {
                assert(dest_storage.m_size > 0);
                assert(dest_storage.m_data != 0);

                uint32_t data_to_copy = std::min(dest_storage.m_size,
                                                 SuperCoder::block_size());

                uint32_t symbol_index = 0;

                while(data_to_copy > 0)
                {
                    uint32_t copy_size = std::min(data_to_copy, SuperCoder::symbol_size());
                    data_to_copy -= copy_size;

                    const_storage src_storage =
                        storage(symbol(symbol_index), copy_size);

                    copy_storage(dest_storage, src_storage);

                    dest_storage.m_size -= copy_size;
                    dest_storage.m_data += copy_size;

                    ++symbol_index;

                }

            }

    protected:

        /// Symbol mapping
        std::vector<value_ptr> m_mapping;

    };

    /// Defines a coding layer for 'const' symbol storage. Only useful
    /// for encoders since these to modify the buffers / data they
    /// operate on.
    template<class SuperCoder>
    class symbol_storage_shallow_const
        : public symbol_storage_shallow<shallow_const_trait, SuperCoder>
    {};

    /// Defines a coding layer for 'mutable' symbol storage. Allows the
    /// buffer data to be modified i.e. useful in decoders which need to
    /// access and modify the incoming symbols
    template<class SuperCoder>
    class symbol_storage_shallow_mutable
        : public symbol_storage_shallow<shallow_mutable_trait, SuperCoder>
    {};
}

#endif

