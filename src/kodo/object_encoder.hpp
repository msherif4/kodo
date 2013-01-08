// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_OBJECT_ENCODER_HPP
#define KODO_OBJECT_ENCODER_HPP

#include <stdint.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>

#include "storage.hpp"

namespace kodo
{
    /// Object encoder, allows easy encoding of objects too
    /// large for a single block encoder.
    ///
    /// The following diagram tries to explain how the object encoder
    /// uses its dependencies to wrap an build encoders for arbitrary
    /// objects.
    ///
    /// +---------------+       +---------------+
    /// | partitioning  |       |encoder factory|
    /// |---------------|       |---------------|
    /// | chops the     |       |               |
    /// | object into   |       | builds new    |
    /// | blocks for    |       | encoders      |
    /// | encoding      |       |               |
    /// |               |       |               |
    /// +---------------+       +---------------+
    ///        ^                        ^
    ///        |                        |
    ///        |                        | builds
    ///        |                        | encoder
    ///        |                        +
    ///        |                +---------------+
    ///        |                | object encoder|
    ///        |                |---------------|
    ///        |                | uses the      |
    ///        +--------------+ | factory and   |
    ///                         | reader to init|
    ///                         | encoders for  |
    ///                         | an object     |
    ///                         +---------------+
    template
    <
        class EncoderType,
        class BlockPartitioning
    >
    class object_encoder : boost::noncopyable
    {
    public:

        /// The type of factory used to build encoders
        typedef typename EncoderType::factory factory_type;

        /// Pointer to an encoder
        typedef typename EncoderType::pointer pointer_type;

        /// The block partitioning scheme used
        typedef BlockPartitioning block_partitioning;

    public:

        /// Constructs a new object encoder
        /// @param factory the encoder factory to use
        /// @param object the object to encode
        object_encoder(factory_type &factory, const const_storage &object)
            : m_factory(factory),
              m_object(object)
            {

                assert(m_object.m_size > 0);
                assert(m_object.m_data != 0);

                m_partitioning = block_partitioning(m_factory.max_symbols(),
                                                    m_factory.max_symbol_size(),
                                                    m_object.m_size);
            }

        /// @return the number of encoders which may be created for this object
        uint32_t encoders() const
            {
                return m_partitioning.blocks();
            }

        /// Builds a specific encoder
        /// @param encoder_id specifies the encoder to build
        /// @return the initialized encoder
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

                init_encoder(offset, bytes_used, encoder);

                return encoder;
            }

        /// @return the total size of the object to encode in bytes
        uint32_t object_size() const
            {
                return m_object.m_size;
            }

    private:

        void init_encoder(uint32_t offset, uint32_t size, pointer_type encoder) const
            {
                assert(offset < m_object.m_size);
                assert(size > 0);
                assert(encoder);

                uint32_t remaining_bytes = m_object.m_size - offset;

                assert(size <= remaining_bytes);

                const_storage storage;
                storage.m_data = m_object.m_data + offset;
                storage.m_size = size;

                encoder->set_symbols(storage);

                // We require that encoders includes the has_bytes_used
                // layer to support partially filled encoders
                encoder->set_bytes_used(size);
            }

    private:

        /// The encoder factory
        factory_type &m_factory;

        /// Store the object storage
        const_storage m_object;

        /// The block partitioning scheme used
        block_partitioning m_partitioning;
    };
}

#endif

