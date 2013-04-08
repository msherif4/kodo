// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_DEEP_STORAGE_DECODER_HPP
#define KODO_DEEP_STORAGE_DECODER_HPP

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
    class deep_storage_decoder :
        public object_decoder<DecoderType, BlockPartitioning>
    {
    public:

        /// We need the code to use a shallow storage class - since
        /// we want the decoder to decode directly into the storage
        /// buffer. Contained within the shallow_storage_decoder
        static_assert(
            has_mutable_shallow_symbol_storage<DecoderType>::value,
            "Deep storage decoder only works with decoders using"
            "shallow storage");

        /// The base class
        typedef object_decoder<DecoderType, BlockPartitioning> base_decoder;

        /// The pointer to the decoder
        typedef typename base_decoder::pointer pointer;

        /// The factory
        typedef typename base_decoder::factory factory;

        /// Access the partitioning scheme
        using base_decoder::m_partitioning;

    public:

        /// Constructs a new storage decoder
        /// @param factory The decoder factory to use
        /// @param object_size The size of the object to be decoded in bytes
        /// @param decoding_buffer The storage where the object will be
        ///        decoded
        deep_storage_decoder(factory &factory, uint32_t object_size) :
            base_decoder(factory, object_size)
        {
            // Resize the decoding storage buffer to be large enough
            m_decoding_storage.resize(
                m_partitioning.total_block_size(), '\0');
        }

        /// @copydoc object_decoder::build(uint32_t)
        pointer build(uint32_t decoder_id)
        {
            auto decoder = base_decoder::build(decoder_id);

            uint32_t offset = m_partitioning.byte_offset(decoder_id);
            uint32_t block_size = m_partitioning.block_size(decoder_id);

            sak::mutable_storage data = sak::storage(m_decoding_storage);
            data += offset;

            assert(data.m_size >= block_size);

            // Adjust the size of the decoding buffer to fit this
            // decoder
            data.m_size = block_size;

            decoder->set_symbols(data);

            return decoder;
        }

        /// Returns the decoded data, this should not be called until all
        /// decoders have completed.
        ///
        /// @return A pointer to the decoded storage class
        const uint8_t* data() const
            {
                return &m_decoding_storage[0];
            }

        /// @copydoc data() const
        uint8_t* data()
            {
                return &m_decoding_storage[0];
            }

        /// If you wish to take ownership of the decoded data you may
        /// you the swap function to get the std::vector. This should
        /// not be called until all decoders have completed.
        ///
        /// @param decoding_storage The vector two swap with the internal
        ///        vector
        void swap(std::vector<uint8_t> &decoding_storage)
            {
                // Resize the storage buffer to have the object size
                m_decoding_storage.resize(base_decoder::object_size());
                m_decoding_storage.swap(decoding_storage);
            }

    private:

        /// The storage where the decoded data should be placed
        std::vector<uint8_t> m_decoding_storage;

    };

}

#endif

