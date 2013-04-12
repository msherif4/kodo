// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RANDOM_ANNEX_DECODER_HPP
#define KODO_RANDOM_ANNEX_DECODER_HPP

#include <cstdint>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <sak/storage.hpp>

#include "random_annex_base.hpp"

namespace kodo
{

    /// @brief A random annex decoder
    template<class DecoderType, class BlockPartitioning>
    class random_annex_decoder : random_annex_base<BlockPartitioning>
    {
    public:

        /// The type of factory used to build decoders
        typedef typename DecoderType::factory factory_type;

        /// Pointer to a decoder
        typedef typename DecoderType::pointer internal_pointer_type;

        /// The block partitioning scheme used
        typedef BlockPartitioning block_partitioning;

        /// The base
        typedef random_annex_base<BlockPartitioning> Base;

        /// Pull up the annex
        using Base::m_annex;

        /// Pull up the reverse annex
        using Base::m_reverse_annex;

        /// The annex iterator type
        typedef typename std::set<annex_info>::iterator annex_iterator;

        /// The callback function to invoke when a decoder completes
        typedef boost::function<void ()> is_complete_handler;

    public:

        class call_proxy
        {
        public:

            call_proxy(const internal_pointer_type &c,
                       const is_complete_handler &h)
                : m_c(c),
                  m_h(h)
                {
                    assert(m_c);
                    m_complete = m_c->is_complete();
                }

            ~call_proxy()
                {
                    assert(m_c);
                    if(!m_complete && m_c->is_complete())
                        m_h();
                }

            DecoderType* operator->() const
                {
                    assert(m_c);
                    return m_c.get();
                }

        private:

            bool m_complete;

            internal_pointer_type m_c;
            is_complete_handler m_h;

        };

        class wrap_coder
        {
        public:

            wrap_coder()
                { };

            wrap_coder(const internal_pointer_type &c,
                       const is_complete_handler &h)
                : m_c(c),
                  m_h(h)
                { }

            call_proxy operator->() const
                {
                    return call_proxy(m_c, m_h);
                }

            internal_pointer_type unwrap()
                {
                    assert(m_c);
                    return m_c;
                }

            internal_pointer_type m_c;

            is_complete_handler m_h;

        };

        typedef wrap_coder pointer_type;

    public:

        /// Constructs a new random annex decoder
        /// @param annex_size the number of symbols used for the
        ///        random annex
        /// @param factory the decoder factory to use
        /// @param object_size size in bytes of the object that is to
        ///        be decoded
        random_annex_decoder(uint32_t annex_size, factory_type &factory,
                             uint32_t object_size)
            : m_annex_size(annex_size),
              m_factory(factory),
              m_object_size(object_size)
            {
                assert(m_object_size > 0);

                // In the random annex code part of the "encoding block"
                // consist of the random annex. We therefore ask the
                // partitioning scheme to make the base block smaller so
                // we may accommodate the annex when building the encoders.
                assert(m_annex_size < m_factory.max_symbols());
                m_base_size = m_factory.max_symbols() - m_annex_size;

                m_partitioning =
                    block_partitioning(m_base_size,
                                       m_factory.max_symbol_size(),
                                       m_object_size);

                // Build the annex
                Base::build_annex(m_annex_size, m_partitioning);

                // Build decoders
                build_decoders();
            }

        /// @return the number of decoders which may be created for
        ///         this object
        uint32_t decoders() const
            {
                return m_decoders.size();
            }

        /// Builds a specific decoder
        /// @param decoder_id specifies the decoder to build
        /// @return the initialized decoder
        pointer_type build(uint32_t decoder_id)
            {
                assert(decoder_id < m_decoders.size());

                return m_decoders[decoder_id];
            }

        /// @return the total size of the object to encode in bytes
        uint32_t object_size() const
            {
                return m_object_size;
            }

    private:

        void forward_annex(uint32_t from_decoder)
            {
                assert(from_decoder < m_decoders.size());
                assert(from_decoder < m_annex.size());

                // Where does the annex start
                uint32_t from_symbol =
                    m_decoders[from_decoder]->symbols() - m_annex_size;

                // Fetch the annex for the decoder
                std::set<annex_info> &annex = m_annex[from_decoder];

                // For every entry in the annex
                for(auto it = annex.begin(); it != annex.end(); ++it)
                {
                    forward_symbol(from_symbol, from_decoder,
                                   it->m_symbol_id, it->m_coder_id);

                    // Next entry in the annex
                    ++from_symbol;
                }
            }

        void forward_symbol(uint32_t from_symbol, uint32_t from_decoder,
                            uint32_t to_symbol, uint32_t to_decoder)
            {
                assert(from_decoder < m_decoders.size());
                assert(to_decoder < m_decoders.size());
                assert(from_symbol < m_decoders[from_decoder]->symbols());
                assert(to_symbol < m_decoders[to_decoder]->symbols());

                if(m_decoders[to_decoder]->is_complete())
                    return;

                // Get the symbol data
                uint8_t *symbol_data =
                    m_decoders[from_decoder]->symbol(from_symbol);

                // Pass it to the other decoder
                m_decoders[to_decoder]->decode_symbol(
                    symbol_data, to_symbol);
            }

        void decoder_complete(uint32_t decoder_id)
            {
                /// @todo: can this check be done better
                //assert(m_annex_size != 0);
                //assert(m_partitioning.blocks() > 1);

                forward_annex(decoder_id);
                reverse_annex(decoder_id);
            }

        void reverse_annex(uint32_t from_decoder)
            {
                assert(from_decoder < m_decoders.size());
                assert(from_decoder < m_annex.size());
                assert(from_decoder < m_reverse_annex.size());

                // Now we use the reverse annex info to further pass
                // symbols to decoders with our decoded block in
                // their annex
                uint32_t reverse_annex_size =
                    m_reverse_annex[from_decoder].size();

                for(uint32_t to_decoder = 0;
                    to_decoder < reverse_annex_size; ++to_decoder)
                {
                    if(!m_reverse_annex[from_decoder][to_decoder])
                        continue;

                    if(m_decoders[to_decoder]->is_complete())
                        continue;

                    // Decoder 'to_decoder' has 'from_decoder' in the annex -
                    //we need to inspect the annex of 'to_decoder' to see
                    // which symbols it is. Fetch the annex for the decoder
                    std::set<annex_info> &annex = m_annex[to_decoder];

                    for(annex_iterator it = annex.begin();
                        it != annex.end(); ++it)
                    {
                        if(it->m_coder_id != from_decoder)
                            continue;

                        uint32_t to_symbol =
                            m_decoders[to_decoder]->symbols() -
                            m_annex_size + std::distance(annex.begin(), it);

                        uint32_t from_symbol = it->m_symbol_id;

                        forward_symbol(from_symbol, from_decoder,
                                       to_symbol, to_decoder);
                    }
                }
            }

        /// Builds a specific decoder
        /// @param decoder_id specifies the decoder to build
        /// @return the initialized decoder
        void build_decoders()
            {
                uint32_t decoders_needed = m_partitioning.blocks();

                m_decoders.resize(decoders_needed);

                for(uint32_t i = 0; i < decoders_needed; ++i)
                {
                    uint32_t symbols =
                        m_partitioning.symbols(i) + m_annex_size;

                    uint32_t symbol_size =
                        m_partitioning.symbol_size(i);

                    m_factory.set_symbols(symbols);
                    m_factory.set_symbol_size(symbol_size);

                    internal_pointer_type decoder = m_factory.build();

                    // Set bytes used
                    uint32_t bytes_used =
                        m_partitioning.bytes_used(i);

                    decoder->set_bytes_used(bytes_used);

                    is_complete_handler handler =
                        boost::bind(&random_annex_decoder::decoder_complete,
                                    this, i);

                    wrap_coder wrap(decoder, handler);

                    // Save the decoder
                    m_decoders[i] = wrap;
                }
            }

    private:

        /// Annex size
        uint32_t m_annex_size;

        /// The base block size
        uint32_t m_base_size;

        /// The decoder factory
        factory_type &m_factory;

        /// The block partitioning scheme used
        block_partitioning m_partitioning;

        /// Store the total object size in bytes
        uint32_t m_object_size;

        /// Vector for all the decoders
        std::vector<wrap_coder> m_decoders;
    };

}

#endif

