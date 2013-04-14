// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "object_encoder.hpp"
#include "file_reader.hpp"
#include "rfc5052_partitioning_scheme.hpp"

namespace kodo
{

    /// @brief A file encoder creates a number of encoders
    ///        over the data of a file.
    ///
    /// The file encoder uses the specified block partitioning scheme to
    /// allocate a number of encoders of a file.
    template
    <
        class EncoderType,
        class BlockPartitioning = rfc5052_partitioning_scheme
    >
    class file_encoder : public
            object_encoder
            <
                file_reader<EncoderType>,
                EncoderType,
                BlockPartitioning
            >
    {
    public:

        /// The encoder factory type
        typedef typename EncoderType::factory factory;

    public:

        /// Constructs a new object encoder
        /// @param factory the encoder factory to use
        /// @param object the object to encode
        file_encoder(typename EncoderType::factory &factory,
                     const std::string &filename)
            : object_encoder
                  <
                  file_reader<EncoderType>,
                  EncoderType,
                  BlockPartitioning
                  >
              (factory, file_reader<EncoderType>(
                  filename,
                  factory.max_symbols() * factory.max_symbol_size()))
            { }
    };
}



