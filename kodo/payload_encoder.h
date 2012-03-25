// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_PAYLOAD_ENCODER_H
#define KODO_PAYLOAD_ENCODER_H

#include <stdint.h>

namespace kodo
{

    // Payload encoder splits payload buffer into payload_data and payload_header
    template<class SuperCoder>
    class payload_encoder : public SuperCoder
    {
    public:
       
        // The factory layer associated with this coder.
        // In this case only needed to provide the max_payload_size()
        // function.
        class factory : public SuperCoder::factory
        {
        public:
            
            // @see final_coder_factory::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            // @return the maximum required payload buffer size in bytes
            uint32_t max_payload_size() const
                {
                    return SuperCoder::factory::max_symbol_size() +
                        SuperCoder::factory::max_symbol_id_size();
                }
        };

    public:

        // Encodes a symbol into the provided payload buffer using the
        // following layout:
        //
        //   +-------------------+---------------+
        //   |    symbol data    |   symbol id   |
        //   +-------------------+---------------+
        //
        // The reason the symbol data is placed first in the payload buffer
        // is to enable 16 byte-alignment of the symbol data. If the variable
        // length id would be place in front of the symbol it would
        // easily become unaligned. Unaligned symbol data access will most
        // likely result in very bad performance.
        // @param payload, the buffer which should contain the encoded symbol and
        //        symbol header.
        // @return the total bytes used from the payload buffer
        uint32_t encode(uint8_t *payload)
            {
                assert(payload != 0);
                
                uint8_t *symbol_data = payload;
                uint8_t *symbol_id = payload + SuperCoder::symbol_size();

                return SuperCoder::encode(symbol_data, symbol_id);
            }

        // @return the required payload buffer size in bytes
        uint32_t payload_size() const
            {
                return SuperCoder::symbol_size() +
                    SuperCoder::symbol_id_size();
            }
        
    };    
    
}

#endif
            
            
