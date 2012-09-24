// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_STORAGE_HPP
#define KODO_STORAGE_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>

namespace kodo
{
    /// The mutable storage class contains a pointer
    /// and size of a modifiable/mutable buffer
    struct mutable_storage
    {
    public:

        typedef uint8_t value_type;
        typedef value_type* value_ptr;

    public:

        /// Create an empty storage object
        mutable_storage()
            : m_size(0),
              m_data(0)
            { }

        /// Create an initialized mutable storage object
        /// @param size the size of the buffer in bytes
        /// @param data pointer to the storage buffer
        mutable_storage(uint32_t size, value_ptr data)
            : m_size(size),
              m_data(data)
            {
                assert(m_size > 0);
                assert(m_data != 0);
            }

        /// Offset the storage
        mutable_storage& operator+=(uint32_t offset)
            {
                assert(offset <= m_size);
                m_size -= offset;
                m_data += offset;
                return *this;
            }

        /// The size of the mutable buffer
        uint32_t m_size;

        /// Pointer to the mutable buffer storage
        value_ptr m_data;

    };

    /// The const storage class contains a pointer and
    /// size of a non-modifiable/const buffer
    struct const_storage
    {
    public:

        typedef uint8_t value_type;
        typedef const value_type* value_ptr;

    public:

        /// Create an empty storage object
        const_storage()
            : m_size(0),
              m_data(0)
            { }

        /// Create an initialized const storage object
        /// @param size the size of the buffer in bytes
        /// @param data pointer to the storage buffer
        const_storage(uint32_t size, value_ptr data)
            : m_size(size),
              m_data(data)
            { }

        /// Creates and const storage object from a mutable
        /// @param s the mutable storage object
        const_storage(const mutable_storage &s)
            : m_size(s.m_size),
              m_data(s.m_data)
            { }

        /// Assigns and converts a mutable storage buffer
        /// into a const storage buffer
        /// @param s the mutable storage object
        const_storage& operator=(const mutable_storage &s)
            {
                m_size = s.m_size;
                m_data = s.m_data;
                return *this;
            }

        /// The size of the mutable buffer
        uint32_t m_size;

        /// Pointer to the non-mutable buffer storage
        value_ptr m_data;

    };

    // Storage arithmetics - adds an offset to a storage object
    // and returns a new storage object with data and size
    // offset adjusted
    // template<class SymbolStorage>
    // SymbolStorage operator+(const SymbolStorage &storage, uint32_t byte_offset)
    // {
    //     assert(byte_offset > 0);
    //     assert(storage.m_size >= byte_offset);
    //     assert(storage.m_data != 0);

    //     return SymbolStorage(storage.m_size - byte_offset,
    //                          storage.m_data + byte_offset);
    // }

    // template<class SymbolStorage>
    // SymbolStorage operator+(uint32_t byte_offset, const SymbolStorage &storage)
    // {
    //     assert(byte_offset > 0);
    //     assert(storage.m_size >= byte_offset);
    //     assert(storage.m_data != 0);

    //     return SymbolStorage(storage.m_size - byte_offset,
    //                          storage.m_data + byte_offset);
    // }


    /// Defines a storage sequence i.e. a storage mapping where
    /// buffers may be in disjoint memory locations
    template<class Storage>
    struct storage_sequence;

    template<>
    struct storage_sequence<const_storage>
    {
        typedef std::vector<const_storage> type;
    };

    template<>
    struct storage_sequence<mutable_storage>
    {
        typedef std::vector<mutable_storage> type;
    };

    /// Typedefs for the sequences
    typedef storage_sequence<const_storage>::type
        const_storage_sequence;

    typedef storage_sequence<mutable_storage>::type
        mutable_storage_sequence;

    /// Splits a continues storage buffer into a sequence of
    /// storage buffers where the continues buffer is split at
    /// a specified number of bytes
    template<class StorageType>
    inline typename storage_sequence<StorageType>::type
    split_storage(const StorageType &storage, uint32_t split)
    {
        typedef typename StorageType::value_ptr
            value_ptr;

        typedef typename storage_sequence<StorageType>::type
            storage_sequence_type;

        uint32_t remaining_size = storage.m_size;
        value_ptr data_offset = storage.m_data;

        storage_sequence_type sequence;

        while(remaining_size > 0)
        {
            uint32_t next_size = std::min(remaining_size, split);

            sequence.push_back(StorageType(next_size, data_offset));

            data_offset += next_size;
            remaining_size -= next_size;
        }

        return sequence;
    }

    /// Returns the size of all the buffers in a storage sequence
    template<class StorageIterator>
    inline uint32_t storage_sequence_size(StorageIterator first, StorageIterator last)
    {
        uint32_t size = 0;
        while(first != last)
        {
            size += first->m_size;
            ++first;
        }
        return size;
    }

    /// Zeros the memory pointed to by a mutable storage
    /// object
    /// @param storage the mutable storage buffer
    inline void zero_storage(mutable_storage &storage)
    {
        std::fill_n(storage.m_data, storage.m_size, 0);
    }

    /// Copies the source storage into the destination storage buffer
    inline void copy_storage(const mutable_storage &dest, const const_storage &src)
    {
        assert(dest.m_size > 0);
        assert(dest.m_size >= src.m_size);
        assert(dest.m_data != 0);
        assert(src.m_data != 0);

        std::copy(src.m_data,
                  src.m_data + src.m_size,
                  dest.m_data);
    }

    template<class Coder>
    inline void copy_symbols(const mutable_storage &dest,
                             const boost::shared_ptr<Coder> &coder)
    {
        assert(coder);
        coder->copy_symbols(dest);
    }

    template<class StorageType, class Coder>
    inline void set_symbols(const StorageType &storage,
                            const boost::shared_ptr<Coder> &coder)
    {
        coder->set_symbols(storage);
    }


    /// Casts the stored pointer to a different data type
    template<class ValueType>
    inline ValueType* cast_storage(const mutable_storage &s)
    {
        return reinterpret_cast<ValueType*>(s.m_data);
    }

    template<class ValueType>
    inline const ValueType* cast_storage(const const_storage &s)
    {
        return reinterpret_cast<const ValueType*>(s.m_data);
    }

    /// Storage function for std::vector<T>
    template<class PodType, class Allocator>
    inline const_storage storage(const std::vector<PodType, Allocator> &v)
    {
        uint32_t size = v.size() * sizeof(PodType);
        const uint8_t *data = reinterpret_cast<const uint8_t*>(&v[0]);

        return const_storage(size, data);
    }

    template<class PodType, class Allocator>
    inline mutable_storage storage(std::vector<PodType, Allocator> &v)
    {
        uint32_t size = v.size() * sizeof(PodType);
        uint8_t *data = reinterpret_cast<uint8_t*>(&v[0]);

        return mutable_storage(size, data);
    }

    /// Storage function for pointers to data
    inline const_storage storage(const void *data, uint32_t size)
    {
        const uint8_t *ptr = reinterpret_cast<const uint8_t*>(data);
        return const_storage(size, ptr);
    }

    inline mutable_storage storage(void *data, uint32_t size)
    {
        uint8_t *ptr = reinterpret_cast<uint8_t*>(data);
        return mutable_storage(size, ptr);
    }
}

#endif

