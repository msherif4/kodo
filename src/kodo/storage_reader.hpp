// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup object_data_implementation
    ///
    /// @brief The storage reader class reads data from a memory buffer
    ///        and initializes the an encoder with data from a specific
    ///        offset within the memory buffer. This class can be used in
    ///        conjunction with object encoders.
    ///
    /// Note that when used together with shallow_symbol_storage
    /// encoders the caller must ensure that the memory buffer remains
    /// valid throughout the life-time of the storage_reader object.
    template<class EncoderType>
    class storage_reader
    {
    public:

        /// Pointer to the encoder used
        typedef typename EncoderType::pointer pointer;

    public:

        /// Creates a new storage reader using the memory buffer
        /// wrapped by the const_storage object.
        /// @param storage the memory buffer to use
        storage_reader(const sak::const_storage &storage)
            : m_storage(storage)
        {
            assert(m_storage.m_size > 0);
            assert(m_storage.m_data != 0);
        }

        /// @return the size of the storage object in bytes
        uint32_t size() const
        {
            return m_storage.m_size;
        }

        /// Initializes the encoder with data from the storage object.
        /// @param encoder to be initialized
        /// @param offset in bytes into the storage object
        /// @param size the number of bytes to use
        void read(pointer &encoder, uint32_t offset, uint32_t size)
        {
            assert(encoder);
            assert(offset < m_storage.m_size);
            assert(size > 0);

            uint32_t remaining_bytes = m_storage.m_size - offset;

            assert(size <= remaining_bytes);

            sak::const_storage storage;
            storage.m_data = m_storage.m_data + offset;
            storage.m_size = size;

            encoder->set_symbols(storage);

            // We require that encoders includes the has_bytes_used
            // layer to support partially filled encoders
            encoder->set_bytes_used(size);
        }

    private:

        /// The memory buffer
        sak::const_storage m_storage;

    };

}


