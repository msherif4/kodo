// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_STORAGE_ENCODER_HPP
#define KODO_STORAGE_ENCODER_HPP

#include "object_encoder.hpp"
#include "storage_reader.hpp"
#include "rfc5052_partitioning_scheme.hpp"

namespace kodo
{

    template
    <
        class EncoderType,
        class BlockPartitioning = rfc5052_partitioning_scheme
    >
    class storage_encoder : public
        object_encoder
            <
            EncoderType,
            BlockPartitioning,
            storage_reader<EncoderType>
            >
    {
    public:

        /// Constructs a new object encoder
        /// @param factory the encoder factory to use
        /// @param object the object to encode
        storage_encoder(typename EncoderType::factory &factory,
                        const const_storage &data)
            : object_encoder
                  <
                  EncoderType,
                  BlockPartitioning,
                  storage_reader<EncoderType>
        >(factory, storage_reader<EncoderType>(data))
            {}
    };
}

#endif

