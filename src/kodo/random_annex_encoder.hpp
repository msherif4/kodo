// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RANDOM_ANNEX_ENCODER_HPP
#define KODO_RANDOM_ANNEX_ENCODER_HPP

#include <cstdint>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>

#include <sak/storage.hpp>

#include "random_annex_base.hpp"

namespace kodo
{

    /// @brief A random annex encoder.
    ///
    /// The Random Annex Code has been proposed in the paper "Collecting
    /// Coded Coupons over Overlapping Generations" by Y.Li. et al.
    template
    <
        class EncoderType,
        class BlockPartitioning
    >
    class random_annex_encoder : random_annex_base<BlockPartitioning>
    {
    public:

        /// The type of factory used to build encoders
        typedef typename EncoderType::factory factory_type;

        /// Pointer to an encoder
        typedef typename EncoderType::pointer pointer_type;

        /// The block partitioning scheme used
        typedef BlockPartitioning block_partitioning;

        /// The base
        typedef random_annex_base<BlockPartitioning> Base;

        /// Pull up the annex
        using Base::m_annex;

    public:

        /// Constructs a new random annex encoder
        /// @param annex_size the number of symbols used for the random annex
        /// @param factory the encoder factory to use
        /// @param object the object to encode
        random_annex_encoder(uint32_t annex_size, factory_type &factory,
                             const sak::const_storage &object)
            : m_annex_size(annex_size),
              m_factory(factory),
              m_object(object)
            {
                assert(m_object.m_size > 0);
                assert(m_object.m_data != 0);

                // In the random annex code part of the "encoding block"
                // consist of the random annex. We therefore ask the
                // partitioning scheme to make the base block smaller so
                // we may accommodate the annex when building the encoders.
                assert(m_annex_size < m_factory.max_symbols());
                m_base_size = m_factory.max_symbols() - m_annex_size;

                m_partitioning = block_partitioning(
                    m_base_size,
                    m_factory.max_symbol_size(),
                    m_object.m_size);

                // Build the annex
                Base::build_annex(m_annex_size, m_partitioning);

                //std::cout << "Build annex" << std::endl;

                // First initialize all encoders with the data in the
                // base blocks
                map_base();

                // Now initialize the annex
                map_annex();
            }

        /// @return the number of encoders which may be created for
        ///         this object
        uint32_t encoders() const
            {
                return m_encoders.size();
            }

        /// Builds a specific encoder
        /// @param encoder_id specifies the encoder to build
        /// @return the initialized encoder
        pointer_type build(uint32_t encoder_id)
            {
                assert(encoder_id < m_encoders.size());

                return m_encoders[encoder_id];
            }

        /// @return the total size of the object to encode in bytes
        uint32_t object_size() const
            {
                return m_object.m_size;
            }

    protected:

        void init_encoder(uint32_t offset, uint32_t size,
                          pointer_type encoder) const
            {
                assert(offset < m_object.m_size);
                assert(size > 0);
                assert(encoder);

                uint32_t remaining_bytes = m_object.m_size - offset;

                assert(size <= remaining_bytes);

                sak::const_storage storage;
                storage.m_data = m_object.m_data + offset;
                storage.m_size = size;

                auto symbol_sequence = sak::split_storage(
                    storage, encoder->symbol_size());

                uint32_t sequence_size = symbol_sequence.size();
                for(uint32_t i = 0; i < sequence_size; ++i)
                {
                    encoder->set_symbol(i, symbol_sequence[i]);
                }

                // We require that encoders includes the has_bytes_used
                // layer to support partially filled encoders
                encoder->set_bytes_used(size);
            }

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

                    m_factory.set_symbols(symbols);
                    m_factory.set_symbol_size(symbol_size);

                    pointer_type encoder = m_factory.build();

                    // Initialize encoder with data
                    uint32_t offset =
                        m_partitioning.byte_offset(i);

                    uint32_t bytes_used =
                        m_partitioning.bytes_used(i);

                    init_encoder(offset, bytes_used, encoder);

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

                    for(it = m_annex[i].begin();
                        it != m_annex[i].end(); ++it)
                    {
                        annex_info annex = *it;

                        assert(annex.m_coder_id < m_encoders.size());

                        pointer_type from_encoder =
                            m_encoders[annex.m_coder_id];

                        assert(from_encoder);

                        assert(from_encoder->symbol_size() ==
                               to_encoder->symbol_size());

                        assert(annex.m_symbol_id <
                               from_encoder->symbols() - m_annex_size);

                        sak::const_storage symbol = sak::storage(
                            from_encoder->symbol(annex.m_symbol_id),
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

        /// Annex size
        uint32_t m_annex_size;

        /// The base block size
        uint32_t m_base_size;

        /// The encoder factory
        factory_type &m_factory;

        /// The block partitioning scheme used
        block_partitioning m_partitioning;

        /// Store the total object storage
        sak::const_storage m_object;

        /// Vector for all the encoders
        std::vector<pointer_type> m_encoders;
    };
}

#endif

