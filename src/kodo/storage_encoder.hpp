// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "object_encoder.hpp"
#include "storage_reader.hpp"
#include "rfc5052_partitioning_scheme.hpp"

namespace kodo
{

    /// @brief A storage encoder creates a number of encoders over a
    ///        sak::const_storage object.
    ///
    /// If the sak::const_storage object is too large for a single
    /// encoder/decoder pair one may use the storage encoder.
    template
    <
        class EncoderType,
        class BlockPartitioning = rfc5052_partitioning_scheme
    >
    class storage_encoder : public
        object_encoder
            <
            storage_reader<EncoderType>,
            EncoderType,
            BlockPartitioning
            >
    {
    public:

        /// The factory type
        typedef typename EncoderType::factory factory;

    public:

        /// Constructs a new storage encoder
        /// @param factory the encoder factory to use
        /// @param object the object to encode
        storage_encoder(typename EncoderType::factory &factory,
                        const sak::const_storage &data)
            : object_encoder
                  <
                  storage_reader<EncoderType>,
                  EncoderType,
                  BlockPartitioning
                  >
              (factory, storage_reader<EncoderType>(data))
            { }
    };
}


