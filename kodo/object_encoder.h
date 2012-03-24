// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_OBJECT_ENCODER_H
#define KODO_OBJECT_ENCODER_H

#include <stdint.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>

#include "storage.h"
#include "object_reader.h"

namespace kodo
{

    // Object encoder, allows easy encoding of objects too
    // large for a single block encoder.
    //
    // The following diagram tries to explain how the object encoder
    // uses its dependencies to wrap an build encoders for arbitrary
    // objects.
    //
    // +---------------+       +---------------+       +---------------+
    // | partitioning  |       |encoder factory|       |  object data  |
    // |---------------|       |---------------|       |---------------|
    // | chops the     |       |               |       |               |
    // | object into   |       | builds new    |       | object data   |
    // | blocks for    |       | encoders      |       | can be a file |
    // | encoding      |       |               |       | memory buffer |
    // |               |       |               |       |               |
    // +---------------+       +---------------+       +---------------+
    //        ^                        ^                       ^
    //        |                        |                       |
    //        |                        | builds                |
    //        |                        | encoder               | reads
    //        |                        +                       |
    //        |                +---------------+               |
    //        |                | object encoder|               +
    //        |                |---------------|          +----------+
    //        |                | uses the      |          | object   |
    //        +--------------+ | factory and   | +------> | reader   |
    //                         | reader to init|          | function |
    //                         | encoders for  |          +----------+
    //                         | an object     |
    //                         +---------------+
    //
    //
    template
    <
        class EncoderType,
        class BlockPartitioning,
        template <class> class MakeReader = make_object_reader
    >
    class object_encoder : boost::noncopyable
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
        
    public:

        // Constructs a new object encoder
        // @param factory, the encoder factory to use
        // @param object, the object to encode
        // @param make_reader, functor supplying the make() and size() functions
        //                     to allow access to arbitrary objects
        template<class ObjectType>
        object_encoder(factory_type &factory, const ObjectType &object,
                       make_reader_type make_reader = make_reader_type())
            : m_factory(factory),
              m_object_reader(make_reader.make(object)),
              m_object_size(make_reader.size(object))
            {

                assert(m_object_size > 0);

                m_partitioning = block_partitioning(m_factory.max_symbols(),
                                                    m_factory.max_symbol_size(),
                                                    m_object_size);
                
            }

        // @return the number of encoders which may be created for
        //         this object
        uint32_t encoders() const
            {
                return m_partitioning.blocks();
            }

        // Builds a specific encoder
        // @param encoder_id, specifies the encoder to build
        // @return the initialized encoder
        pointer_type build(uint32_t encoder_id)
            {
                assert(encoder_id < m_partitioning.blocks());

                // Build the encoder
                uint32_t symbols =
                    m_partitioning.symbols(encoder_id);

                uint32_t symbol_size =
                    m_partitioning.symbol_size(encoder_id);

                pointer_type encoder =
                    m_factory.build(symbols, symbol_size);

                // Initialize encoder with data
                uint32_t offset =
                    m_partitioning.byte_offset(encoder_id);

                uint32_t bytes_used =
                    m_partitioning.bytes_used(encoder_id);
                
                m_object_reader(offset, bytes_used, encoder);
                
                return encoder;
            }

        // @return the total size of the object to encode in bytes
        uint32_t object_size() const
            {
                return m_object_size;
            }
        
    private:

        // The encoder factory
        factory_type &m_factory;

        // The object reader function used to initialize the encoders
        // with data
        object_reader_type m_object_reader;

        // Store the total object size in bytes
        uint32_t m_object_size;
        
        // The block partitioning scheme used
        block_partitioning m_partitioning;
        
    };

}        

#endif

