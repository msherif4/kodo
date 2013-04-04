// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_STORAGE_DECODER_HPP
#define KODO_STORAGE_DECODER_HPP

#include <cstdint>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <sak/storage.hpp>

#include "object_decoder.hpp"
#include "rfc5052_partitioning_scheme.hpp"
#include "has_shallow_symbol_storage.hpp"

namespace kodo
{

    /// @brief A storage decoder creates a number of decoders decoding
    ///        into a sak::mutable_storage object
    template
    <
        class DecoderType,
        class BlockPartitioning = rfc5052_partitioning_scheme
    >
    class storage_decoder
        : public object_decoder<DecoderType, BlockPartitioning>
    {
    public:

        /// We need the code to use a shallow storage class - since
        /// we want the decoder to decode directly into the storage
        /// buffer.
        static_assert(
            has_mutable_shallow_symbol_storage<DecoderType>::value,
            "Storage decoder only works with decoders using"
            "shallow storage");

        /// The base class
        typedef object_decoder<DecoderType, BlockPartitioning> base_decoder;

        /// The pointer to the decoder
        typedef typename base_decoder::pointer pointer;

        /// Access the partitioning scheme
        using base_decoder::m_partitioning;

        class factory : public DecoderType::factory
        {
        public:
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : DecoderType::factory(max_symbols, max_symbol_size)
                { }

            uint32_t max_storage_size(uint32_t object_size) const
                {
                    BlockPartitioning p(DecoderType::factory::max_symbols(),
                                       DecoderType::factory::max_symbol_size(),object_size);

                    return p.total_symbols()*p.symbol_size();

        };

    public:

        /// Constructs a new storage decoder
        /// @param factory the encoder factory to use
        /// @param object the object to decode into
        storage_decoder(typename DecoderType::factory &factory,
                        const sak::mutable_storage &data)
            : base_decoder(factory, data.m_size),
              m_data(data)
            { }

        pointer build(uint32_t decoder_id)
            {
                auto decoder = base_decoder::build(decoder_id);

                uint32_t offset = m_partitioning.byte_offset(decoder_id);
                sak::mutable_storage data = m_data + offset;

                decoder->set_symbols(data);

                return decoder;
            }


    private:

        /// The storage where the decoded data should be placed
        sak::mutable_storage m_data;

    };

}

#endif

