// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

namespace kodo
{

    /// @ingroup payload_codec_layers
    /// @brief The payload decoder splits the payload buffer into
    ///        symbol header and symbol.
    template<class SuperCoder>
    class payload_rank_decoder : public SuperCoder
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

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            SuperCoder::initialize(the_factory);

            /// Reset the state
            m_encoder_rank = 0;
        }

        /// Unpacks the symbol data and symbol header from the payload
        /// buffer.
        /// @copydoc layer::decode(uint8_t*)
        void decode(uint8_t* payload)
        {
            assert(payload != 0);

            uint32_t read = read_rank(payload);
            SuperCoder::decode(payload + read);
        }

        /// Unpacks the symbol data and symbol header from the payload
        /// buffer.
        /// @todo
        uint32_t read_rank(uint8_t* payload)
        {
            assert(payload != 0);

            // Write the encoder rank to the payload
            rank_type encoder_rank =
                sak::big_endian::get<rank_type>(payload);

            // We should never see an encoder which reduces
            // its rank
            assert(m_encoder_rank <= encoder_rank);
            m_encoder_rank = encoder_rank;

            return sizeof(rank_type);
        }

        /// @copydoc layer::payload_size() const
        uint32_t payload_size() const
        {
            return SuperCoder::payload_size() +
                sizeof(rank_type);
        }

        /// @return The rank of the encoder as read from the packet
        rank_type encoder_rank() const
        {
            return m_encoder_rank;
        }

    private:

        /// Stores the read encoder rank
        rank_type m_encoder_rank;

    };

}


