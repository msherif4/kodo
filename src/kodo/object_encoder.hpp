// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>

#include "rfc5052_partitioning_scheme.hpp"

namespace kodo
{

    /// @brief The object encoder, allows encoding of objects too
    ///        large for a single block encoder.
    ///
    /// The following diagram tries to explain how the object encoder
    /// uses its dependencies to wrap an build encoders for arbitrary
    /// objects.
    ///
    /// <pre>
    /// +---------------+     +---------------+     +---------------+
    /// | partitioning  |     |encoder factory|     |  object data  |
    /// |---------------|     |---------------|     |---------------|
    /// | chops the     |     |               |     |               |
    /// | object into   |     | builds new    |     | object data   |
    /// | blocks for    |     | encoders      |     | can be a file |
    /// | encoding      |     |               |     | memory buffer |
    /// |               |     |               |     |               |
    /// +---------------+     +---------------+     +---------------+
    ///        ^                      ^                     ^
    ///        |                      |                     |
    ///        |                      | builds              |
    ///        |                      | encoder             | reads
    ///        |                      +                     |
    ///        |              +---------------+             |
    ///        |              | object encoder|             |
    ///        |              |---------------|             |
    ///        |              | uses the      |             |
    ///        +------------+ | factory and   | +-----------+
    ///                       | reader to init|
    ///                       | encoders for  |
    ///                       | an object     |
    ///                       +---------------+
    /// </pre>
    ///
    /// @tparam ObjectData object_data
    /// @tparam EncoderType An encoder stack which should be used
    /// @tparam BlockParitioning block_partitioning
    template
    <
        class ObjectData,
        class EncoderType,
        class BlockPartitioning = rfc5052_partitioning_scheme
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

        /// The data source type
        typedef ObjectData object_data;

    public:

        /// Constructs a new object encoder
        /// @param factory the encoder factory to use
        /// @param object the object to encode
        object_encoder(factory_type &factory, const object_data &data) :
            m_factory(factory),
            m_data(data)
        {

            assert(m_data.size() > 0);

            m_partitioning = block_partitioning(
                m_factory.max_symbols(),
                m_factory.max_symbol_size(),
                m_data.size());
        }

        /// @return The number of encoders which may be created for
        ///         this object
        uint32_t encoders() const
        {
            return m_partitioning.blocks();
        }

        /// Builds a specific encoder
        /// @param encoder_id Specifies the encoder to build
        /// @return The initialized encoder
        pointer_type build(uint32_t encoder_id)
        {
            assert(encoder_id < m_partitioning.blocks());

            // Build the encoder
            uint32_t symbols =
                m_partitioning.symbols(encoder_id);

            uint32_t symbol_size =
                m_partitioning.symbol_size(encoder_id);

            m_factory.set_symbols(symbols);
            m_factory.set_symbol_size(symbol_size);

            pointer_type encoder = m_factory.build();

            // Initialize encoder with data
            uint32_t offset =
                m_partitioning.byte_offset(encoder_id);

            uint32_t bytes_used =
                m_partitioning.bytes_used(encoder_id);

            m_data.read(encoder, offset, bytes_used);

            return encoder;
        }

        /// @return The total size of the object to encode in bytes
        uint32_t object_size() const
        {
            return m_data.size();
        }

    private:

        /// The encoder factory
        factory_type &m_factory;

        /// Store the object storage
        object_data m_data;

        /// The block partitioning scheme used
        block_partitioning m_partitioning;
    };

}



