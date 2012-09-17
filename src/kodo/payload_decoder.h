// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_PAYLOAD_DECODER_H
#define KODO_PAYLOAD_DECODER_H

#include <stdint.h>

#include "systematic_base_coder.h" 

namespace kodo
{

    /// Systematic encoder layer
    template<class SuperCoder>
    class payload_decoder : public SuperCoder
    {
    public:

        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        public:
            
            /// @see final_coder_factory::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @return the required payload buffer size in bytes
            uint32_t max_payload_size() const
                {
                    return SuperCoder::factory::max_symbol_size() +
                        SuperCoder::factory::max_symbol_id_size();
                }
        };

    public:
                
        /// Unpacks the symbol data and symbol id from the payload
        /// buffer. @see payload_encoder::encode(...) for memory layout.
        /// @param payload, the buffer from which we take the data and id
        void decode(uint8_t *payload)
            {
                assert(payload != 0);

                uint8_t *symbol_data = payload;
                uint8_t *symbol_id = payload + SuperCoder::symbol_size();

                return SuperCoder::decode(symbol_data, symbol_id);
            }

        /// @return the required payload buffer size in bytes
        uint32_t payload_size() const
            {
                return SuperCoder::symbol_size() +
                    SuperCoder::symbol_id_size();
            }
    };        
}

#endif

