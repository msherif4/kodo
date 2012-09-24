// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_PROXY_ENCODER_HPP
#define KODO_PROXY_ENCODER_HPP

#include <stdint.h>

namespace kodo
{
    /// The proxy encoder serves as a way to intercept calls
    /// and redirect them.
    template<class Proxy>
    class proxy_encoder
    {
    public:

        /// Constructor
        proxy_encoder()
            : m_proxy(0)
            {}

        /// @see final_coder::construct(...)
        void construct(uint32_t /*max_symbols*/, uint32_t /*max_symbol_size*/)
            { }

        /// @see final_coder::initialize(...)
        void initialize(uint32_t /*symbols*/, uint32_t /*symbol_size*/)
            { }

        /// Forwards the encode(...) function to the
        /// proxy_encode(...) function in the specified proxy
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(m_proxy);
                return m_proxy->proxy_encode(symbol_data, symbol_id);
            }

        /// Forwards the encode(...) function to the
        /// proxy_encode(...) function in the specified proxy
        uint32_t symbol_id_size() const
            {
                assert(m_proxy);
                return m_proxy->proxy_symbol_id_size();
            }

        /// Forwards the encode(...) function to the
        /// proxy_encode(...) function in the specified proxy
        uint32_t symbol_size()
            {
                assert(m_proxy);
                return m_proxy->proxy_symbol_size();
            }

        /// Set the pointer to the proxy object
        void set_proxy(Proxy *proxy)
            {
                assert(proxy);
                m_proxy = proxy;
            }

    private:

        /// Pointer to the proxy class
        Proxy *m_proxy;
    };
}

#endif

