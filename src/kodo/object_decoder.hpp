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

    /// @brief The object decoder supports decoding objects split over
    ///        multiple decoders.
    ///
    /// When an object is too large for a single encoder/decoder pair,
    /// the object decoder uses the specified partitioning scheme to
    /// determine how decoders should be constructed to cover the entire
    /// object.
    template
    <
        class DecoderType,
        class BlockPartitioning = rfc5052_partitioning_scheme
    >
    class object_decoder : boost::noncopyable
    {
    public:

        /// The type of factory used to build decoders
        typedef typename DecoderType::factory factory;

        /// Pointer to a decoder
        typedef typename DecoderType::pointer pointer;

        /// The block partitioning scheme used
        typedef BlockPartitioning block_partitioning;

    public:

        /// Constructs a new object decoder
        /// @param factory The decoder factory to use
        /// @param object_size The size in bytes of the object to be decoded
        object_decoder(factory &decoder_factory, uint32_t object_size)
            : m_factory(decoder_factory),
              m_object_size(object_size)
        {
            assert(m_object_size > 0);

            m_partitioning = block_partitioning(
                m_factory.max_symbols(),
                m_factory.max_symbol_size(),
                m_object_size);

        }

        /// @return The number of decoders which may be created for
        ///         this object
        uint32_t decoders() const
        {
            return m_partitioning.blocks();
        }

        /// Builds a specific decoder
        /// @param decoder_id Specifies the decoder to build
        /// @return The initialized decoder
        pointer build(uint32_t decoder_id)
        {
            assert(decoder_id < m_partitioning.blocks());

            uint32_t symbols =
                m_partitioning.symbols(decoder_id);

            uint32_t symbol_size =
                m_partitioning.symbol_size(decoder_id);

            m_factory.set_symbols(symbols);
            m_factory.set_symbol_size(symbol_size);

            pointer decoder = m_factory.build();

            // Set bytes used
            uint32_t bytes_used =
                m_partitioning.bytes_used(decoder_id);

            decoder->set_bytes_used(bytes_used);

            return decoder;
        }

        /// @return The total size of the object to decode in bytes
        uint32_t object_size() const
        {
            return m_object_size;
        }

    protected:

        /// The decoder factory
        factory &m_factory;

        /// The block partitioning scheme used
        block_partitioning m_partitioning;

        /// Store the total object size in bytes
        uint32_t m_object_size;
    };

}



