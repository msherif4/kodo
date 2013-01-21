// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_STORAGE_READER_HPP
#define KODO_STORAGE_READER_HPP

#include <sak/storage.hpp>

namespace kodo
{

    template<class EncoderType>
    class storage_reader
    {
    public:
        
        typedef typename EncoderType::pointer pointer;
        
    public:
        
        storage_reader(const const_storage &storage)
            : m_storage(storage)
            {
                assert(m_storage.m_size > 0);
                assert(m_storage.m_data != 0);
            }
        
        uint32_t size() const
            {
                return m_storage.m_size;
            }
        
        void read(pointer &encoder, uint32_t offset, uint32_t size)
            {
                assert(encoder);
                assert(offset < m_storage.m_size);
                assert(size > 0);
                
                uint32_t remaining_bytes = m_storage.m_size - offset;
                
                assert(size <= remaining_bytes);
                
                const_storage storage;
                storage.m_data = m_storage.m_data + offset;
                storage.m_size = size;
                
                encoder->set_symbols(storage);
                
                // We require that encoders includes the has_bytes_used
                // layer to support partially filled encoders
                encoder->set_bytes_used(size);
            }
        
    private:
        
        const_storage m_storage;
        
    };
    
}

#endif

