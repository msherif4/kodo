// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#ifndef KODO_OBJECT_DECODER_H
#define KODO_OBJECT_DECODER_H

#include <stdint.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>

#include "storage.h"

namespace kodo
{

    // Object decoder
    template<class DecoderType, class BlockPartitioning>
    class object_decoder : boost::noncopyable
    {
    public:

        // The type of factory used to build decoders
        typedef typename DecoderType::factory factory_type;

        // Pointer to a decoder
        typedef typename DecoderType::pointer pointer_type;

        // The block partitioning scheme used
        typedef BlockPartitioning block_partitioning;
        
    public:

        // Constructs a new object decoder
        // @param factory, the decoder factory to use
        // @param object_size, the size in bytes of the object that should be
        //        decoded
        object_decoder(factory_type &factory, uint32_t object_size)
            : m_factory(factory),
              m_object_size(object_size)
            {
                assert(m_object_size > 0);

                m_partitioning = block_partitioning(m_factory.max_symbols(),
                                                    m_factory.max_symbol_size(),
                                                    m_object_size);
                
            }
        
        // @return the number of decoders which may be created for
        //         this object
        uint32_t decoders() const
            {
                return m_partitioning.blocks();
            }
        
        // Builds a specific decoder
        // @param decoder_id, specifies the decoder to build
        // @return the initialized decoder
        pointer_type build(uint32_t decoder_id)
            {
                assert(decoder_id < m_partitioning.blocks());
                
                uint32_t symbols =
                    m_partitioning.symbols(decoder_id);

                uint32_t symbol_size =
                    m_partitioning.symbol_size(decoder_id);

                pointer_type decoder = m_factory.build(symbols, symbol_size);
                
                // Set bytes used
                uint32_t bytes_used =
                    m_partitioning.bytes_used(decoder_id);

                decoder->set_bytes_used(bytes_used);
                
                return decoder;
            }

        // @return the total size of the object to encode in bytes
        uint32_t object_size() const
            {
                return m_object_size;
            }

    private:
        
        // The decoder factory
        factory_type &m_factory;

        // The block partitioning scheme used
        block_partitioning m_partitioning;

        // Store the total object size in bytes
        uint32_t m_object_size;
        
    };

}        

#endif

