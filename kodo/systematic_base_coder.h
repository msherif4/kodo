// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#ifndef KODO_SYSTEMATIC_BASE_CODER_H
#define KODO_SYSTEMATIC_BASE_CODER_H

#include <stdint.h>


namespace kodo
{
    // Convenience for the systematic encoder and decoder
    class systematic_base_coder
    {
    public:

        // Typedef the symbol count data type used
        // this choice will add additional x bytes
        // to every packet encode
        typedef uint32_t counter_type;

        // The type used to store the systematic
        // non-systematic flag
        typedef uint8_t flag_type;

        // Systematic flag
        static const flag_type systematic_flag = 0xFFU;

        // Non-systematic flag
        static const flag_type non_systematic_flag = 0x00U;

        
    };
    
}

#endif
            
            