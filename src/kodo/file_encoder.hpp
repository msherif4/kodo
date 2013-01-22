// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_FILE_ENCODER_HPP
#define KODO_FILE_ENCODER_HPP

#include "object_encoder.hpp"
#include "file_reader.hpp"
#include "rfc5052_partitioning_scheme.hpp"

namespace kodo
{

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
                  filename, factory.max_symbols() * factory.max_symbol_size()))
            {}
    };
}

#endif

