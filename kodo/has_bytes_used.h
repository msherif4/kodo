// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_HAS_BYTES_USED_H
#define KODO_HAS_BYTES_USED_H

#include <stdint.h>

namespace kodo
{

    /// Provides access to the number of bytes used out of the
    /// total size of the coders storage
    template<class SuperCoder>
    class has_bytes_used : public SuperCoder
    {
    public:

        /// Constructor
        has_bytes_used()
            : m_bytes_used(0)
            {}

        /// @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_bytes_used = 0;
            }
        
        /// Sets the number of bytes used
        /// @param bytes_used, then number of bytes used out of the total
        ///        coders block size
        void set_bytes_used(uint32_t bytes_used)
            {
                assert(bytes_used > 0);
                assert(bytes_used <= SuperCoder::block_size());
                
                m_bytes_used = bytes_used;
            }
        
        /// @return the number of bytes used
        uint32_t bytes_used() const
            {
                return m_bytes_used;
            }
                
    private:

        /// The number of bytes used
        uint32_t m_bytes_used;
        
    };    
}

#endif

