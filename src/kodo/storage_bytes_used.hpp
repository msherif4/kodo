// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_STORAGE_BYTES_USED_HPP
#define KODO_STORAGE_BYTES_USED_HPP

#include <cstdint>

namespace kodo
{

    /// @ingroup storage_layers
    /// @brief Provides access to the number of useful bytes used out of the
    /// total size of the encoders or decoders storage.
    template<class SuperCoder>
    class storage_bytes_used : public SuperCoder
    {
    public:

        /// Constructor
        storage_bytes_used()
            : m_bytes_used(0)
            {}

        /// @copydoc layer::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_bytes_used = 0;
            }

        /// @copydoc layer::set_bytes_used()
        void set_bytes_used(uint32_t bytes_used)
            {
                assert(bytes_used > 0);
                assert(bytes_used <= SuperCoder::block_size());

                m_bytes_used = bytes_used;
            }

        /// @copydoc layer::bytes_used()
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

