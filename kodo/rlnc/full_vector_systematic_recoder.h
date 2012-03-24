// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_SYSTEMATIC_RECODER_H
#define KODO_RLNC_FULL_VECTOR_SYSTEMATIC_RECODER_H

#include <stdint.h>
#include <sak/convert_endian.h>

#include "../systematic_base_coder.h" 

namespace kodo
{

    // This layer makes recoding compatible with existing systematic
    // encoding and decoding layers
    template<class SuperCoder>
    class full_vector_systematic_recoder : public SuperCoder
    {
    public:
        
        // The flag type
        typedef typename systematic_base_coder::flag_type
            flag_type;
        
    public:
                
        // Iterates over the symbols stored in the encoding symbol id part
        // of the payload id, and calls the encode_symbol function.
        uint32_t recode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);
                
                // Flag non_systematic packet
                sak::big_endian::put<flag_type>(
                    systematic_base_coder::non_systematic_flag, symbol_id);

                symbol_id += sizeof(flag_type);
                
                return sizeof(flag_type) +
                    SuperCoder::recode(symbol_data, symbol_id);
                        
            }
    };    
}

#endif
            
            
