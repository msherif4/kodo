// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_OBJECT_READER_H
#define KODO_OBJECT_READER_H

#include <stdint.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "storage.h"

namespace kodo
{
    /// The object reader type - defines a function which can be used to
    /// set the storage for an encoder.
    /// As an example see the const_storage_reader.
    template<class EncoderPointer>
    struct object_reader
    {
        
        typedef boost::function<void (uint32_t, uint32_t, EncoderPointer)>
            type;
    };

    /// Reads data from the input stream into the Encoder data storage
    /// @param offset, the byte offset into the input stream
    /// @param size, the number of bytes to read from the input stream
    /// @param encoder, the encoder to initialize with storage
    /// @param storage, the storage object from which we read data
    template<class EncoderPointer>
    inline void
    const_storage_reader(uint32_t offset, uint32_t size, EncoderPointer encoder,
                         const_storage storage)
    {
        assert(offset < storage.m_size);
        assert(size > 0);
        assert(encoder);
        assert(storage.m_data != 0);
        assert(storage.m_size > 0);

        uint32_t remaining_bytes = storage.m_size - offset;

        assert(size <= remaining_bytes); 
                        
        const_storage encoder_storage;
        encoder_storage.m_data = storage.m_data + offset;
        encoder_storage.m_size = size;

        kodo::set_symbols(encoder_storage, encoder);

        /// We require that encoders includes the has_bytes_used
        /// layer to support partially filled encoders
        encoder->set_bytes_used(size);
    }
                
    /// Helper object which can be used to produce object_readers for a
    /// number of different data sources. Notice how we bind the actual
    /// storage object as the last parameter - doing this we the avoid
    /// having to "inform" the code using an object reader about what
    /// data source it is actually using.
    template<class EncoderPointer>
    struct make_object_reader
    {
    public:

        /// Makes an object reader for const_storage object.
        typename object_reader<EncoderPointer>::type
        make(const const_storage &storage) const
            {
                assert(storage.m_size > 0);
                assert(storage.m_data != 0);

                return boost::bind(
                    const_storage_reader<EncoderPointer>, _1, _2, _3, storage);
            }

        /// Returns the size for and const_stroage object
        uint32_t size(const const_storage &storage) const
            {
                assert(storage.m_size > 0);
                assert(storage.m_data != 0);

                return storage.m_size;
            }
    };
}        

#endif

