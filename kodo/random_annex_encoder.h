// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#ifndef KODO_RANDOM_ANNEX_ENCODER_H
#define KODO_RANDOM_ANNEX_ENCODER_H

#include <stdint.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>

#include "storage.h"
#include "object_encoder.h"
#include "random_annex_base.h"

namespace kodo
{

    // A random annex encoder
    template
    <
        class EncoderType,
        class BlockPartitioning,
        template <class> class MakeReader = make_object_reader
    >
    class random_annex_encoder : random_annex_base<BlockPartitioning>
    {
    public:

        // The type of factory used to build encoders
        typedef typename EncoderType::factory factory_type;

        // Pointer to an encoder
        typedef typename EncoderType::pointer pointer_type;

        // The object reader function type
        typedef typename object_reader<pointer_type>::type
            object_reader_type;

        // The block partitioning scheme used
        typedef BlockPartitioning block_partitioning;

        // The object reader builder
        typedef MakeReader<pointer_type> make_reader_type;

        // The base
        typedef random_annex_base<BlockPartitioning> Base;

        // Pull up the annex
        using Base::m_annex;
        
    public:

        // Constructs a new random annex encoder
        // @param annex_size, the number of symbols used for the random annex
        // @param factory, the encoder factory to use
        // @param object, the object to encode
        // @param make_reader, functor supplying the make() and size() functions
        //                     to allow access to arbitrary objects
        template<class ObjectType>
        random_annex_encoder(uint32_t annex_size, factory_type &factory,
                             const ObjectType &object,
                             make_reader_type make_reader = make_reader_type())
            : m_annex_size(annex_size),
              m_factory(factory),
              m_object_reader(make_reader.make(object)),
              m_object_size(make_reader.size(object))
            {
                assert(m_object_size > 0);

                // In the random annex code part of the "encoding block"
                // consist of the random annex. We therefore ask the
                // partitioning scheme to make the base block smaller so
                // we may accommodate the annex when building the encoders.
                assert(m_annex_size < m_factory.max_symbols());
                m_base_size = m_factory.max_symbols() - m_annex_size;
                
                m_partitioning = block_partitioning(m_base_size,
                                                    m_factory.max_symbol_size(),
                                                    m_object_size);

                //std::cout << "Partitioning blocks " << m_partitioning.blocks()
                //          << std::endl;
                
                // Build the annex
                Base::build_annex(m_annex_size, m_partitioning);

                //std::cout << "Build annex" << std::endl;
                
                // First initialize all encoders with the data in the base blocks
                map_base();

                // Now initialize the annex
                map_annex();
            }



        // @return the number of encoders which may be created for
        //         this object
        uint32_t encoders() const
            {
                return m_encoders.size();
            }

        // Builds a specific encoder
        // @param encoder_id, specifies the encoder to build
        // @return the initialized encoder
        pointer_type build(uint32_t encoder_id)
            {
                assert(encoder_id < m_encoders.size());
                                
                return m_encoders[encoder_id];
            }

        // @return the total size of the object to encode in bytes
        uint32_t object_size() const
            {
                return m_object_size;
            }

    protected:

        void map_base()
            {
                uint32_t encoders_needed = m_partitioning.blocks();
                
                m_encoders.resize(encoders_needed);

                for(uint32_t i = 0; i < encoders_needed; ++i)
                {

                    // Build the encoder
                    uint32_t symbols =
                        m_partitioning.symbols(i) + m_annex_size;
                    
                    uint32_t symbol_size =
                        m_partitioning.symbol_size(i);
                    
                    pointer_type encoder =
                        m_factory.build(symbols, symbol_size);
                    
                    // Initialize encoder with data
                    uint32_t offset =
                        m_partitioning.byte_offset(i);
                    
                    uint32_t bytes_used =
                        m_partitioning.bytes_used(i);
                    
                    m_object_reader(offset, bytes_used, encoder);

                    // Save the encoder
                    m_encoders[i] = encoder;
                }

            }

        void map_annex()
            {
                for(uint32_t i = 0; i < m_encoders.size(); ++i)
                {

                    pointer_type to_encoder = m_encoders[i];
                    assert(to_encoder);

                    uint32_t annex_position = 0;
                    
                    typename std::set<annex_info>::iterator it;
                    
                    for(it = m_annex[i].begin(); it != m_annex[i].end(); ++it)
                    {
                        annex_info annex = *it;

                        assert(annex.m_coder_id < m_encoders.size());
                        
                        pointer_type from_encoder = m_encoders[annex.m_coder_id];
                        
                        assert(from_encoder);
                        assert(from_encoder->symbol_size() == to_encoder->symbol_size());
                        assert(annex.m_symbol_id < from_encoder->symbols() - m_annex_size);
                        
                        const_storage symbol =
                            kodo::storage(from_encoder->symbol(annex.m_symbol_id),
                                          from_encoder->symbol_size());

                        assert(symbol.m_data != 0);
                        assert(symbol.m_size > 0);
                        assert(annex_position < m_annex_size);
                        
                        uint32_t to_symbol_id = to_encoder->symbols()
                            - m_annex_size + annex_position;

                        to_encoder->set_symbol(to_symbol_id, symbol);

                        ++annex_position;
                    }                    
                }                
            }

    private:

        // Annex size
        uint32_t m_annex_size;

        // The base block size
        uint32_t m_base_size;
        
        // The encoder factory
        factory_type &m_factory;

        // The block partitioning scheme used
        block_partitioning m_partitioning;

        // The object reader function used to initialize the encoders
        // with data
        object_reader_type m_object_reader;

        // Store the total object size in bytes
        uint32_t m_object_size;

        // Vector for all the encoders
        std::vector<pointer_type> m_encoders;
        
    };

}        

#endif

