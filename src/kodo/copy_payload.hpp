// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_COPY_PAYLOAD_HPP
#define KODO_COPY_PAYLOAD_HPP

#include <cstdint>
#include <vector>

namespace kodo
{

    /// @ingroup payload_codec_layers
    ///
    /// @brief Make decoder work on a copy of the payload
    ///
    /// Make decoder work on a copy of the payload to ensure that the
    /// payload isn't overwritten during decoding
    template<class SuperCoder>
    class copy_payload : public SuperCoder
    {
    public:

        /// @copydoc layer::construct(uint32_t,uint32_t)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                m_payload_copy.resize(SuperCoder::payload_size(), 0);
            }

        /// Copy the payload data to ensure that the payload isn't
        /// overwritten during decoding
        /// @copydoc layer::decode(uint8_t*)
        void decode(const uint8_t *payload)
            {
                assert(payload != 0);

                /// Copy payload
                memcpy(&m_payload_copy[0], payload, SuperCoder::payload_size());

                SuperCoder::decode(&m_payload_copy[0]);
            }

    private:

        /// Copy of payload 
        std::vector<uint8_t> m_payload_copy;

    };
}

#endif

