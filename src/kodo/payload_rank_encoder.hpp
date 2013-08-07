// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <sak/convert_endian.hpp>

namespace kodo
{

    /// @ingroup payload_codec_layers
    /// @brief The payload rank encoder writes the rank of the encoder into
    ///        the payload buffer.
    ///
    /// Storing the rank of the encoder in the payload buffer allows a decoder
    /// at a receiver to detect early decoding opportunities. This is possible
    /// if the receiver detects that it's decoder has the same rank as the
    /// encoder from which it is receiving packets. Note, that the rank of an
    /// encoder means how many packets that encoder has available for coding.
    /// So if the decoder has decoded as many packets as were available then
    /// we might be able to access the packets even before reaching full rank
    /// at the decoder. The reason we say "might" is that if the decoder is not
    /// a Gaussian Elimination decoder (which also does backward substitution)
    /// immediately then symbols might not yet be fully decoded. So you have to
    /// make sure the decoder used performs both forward and backward
    /// substitution on every symbols as it is being received to use this
    /// feature.
    template<class SuperCoder>
    class payload_rank_encoder : public SuperCoder
    {
    public:

        /// @copydoc layer::rank_type
        typedef typename SuperCoder::rank_type rank_type;

    public:

        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
            { }

            /// @copydoc layer::factory::max_payload_size() const
            uint32_t max_payload_size() const
            {
                return SuperCoder::factory::max_payload_size() +
                    sizeof(rank_type);
            }

        };

    public:

        /// @copydoc layer::encode(uint8_t*)
        uint32_t encode(uint8_t* payload)
        {
            assert(payload != 0);

            // Write the encoder rank to the payload
            uint32_t written = write_rank(payload);
            return SuperCoder::encode(payload + written) + written;
        }

        /// Helper function which writes the rank of the encoder into
        /// the payload buffer
        /// @param payload The buffer where the rank should be written
        /// @return The number of bytes written to the payload
        uint32_t write_rank(uint8_t* payload)
        {
            assert(payload != 0);

            // Write the encoder rank to the payload
            sak::big_endian::put<rank_type>(SuperCoder::rank(), payload);

            return sizeof(rank_type);
        }

        /// @copydoc layer::payload_size() const
        uint32_t payload_size() const
        {
            return SuperCoder::payload_size() + sizeof(rank_type);
        }

    };

}

