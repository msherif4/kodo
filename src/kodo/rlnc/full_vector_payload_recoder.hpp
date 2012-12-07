// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_PAYLOAD_RECODER_HPP
#define KODO_RLNC_FULL_VECTOR_PAYLOAD_RECODER_HPP

#include <stdint.h>

namespace kodo
{
    /// Systematic encoder layer
    template<class SuperCoder>
    class full_vector_payload_recoder : public SuperCoder
    {
    public:

        /// Unpacks the symbol data and symbol id from the payload
        /// buffer. @see payload_encoder::encode(...) for memory layout.
        /// @param payload the buffer from which we take the data and id
        /// @return the amount of used buffer in bytes
        uint32_t recode(uint8_t *payload)
            {
                assert(payload != 0);

                uint8_t *symbol_data = payload;
                uint8_t *symbol_id = payload + SuperCoder::symbol_size();

                return SuperCoder::recode(symbol_data, symbol_id);
            }
    };
}

#endif

