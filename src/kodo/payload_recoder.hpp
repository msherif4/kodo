// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_PAYLOAD_RECODER_HPP
#define KODO_PAYLOAD_RECODER_HPP

#include <cstdint>

namespace kodo
{

    /// @ingroup payload_codec_layers
    /// @brief Implements the recode function for Network Codes.
    ///
    /// The payload recoder layer has been implemented as
    /// a simple layer forwarding calls to recode to a proxy
    /// encoder. The reason for this design was that recoding could
    /// be implemented by almost entirely copying the layers of an
    /// encoder. The only difference being that the a special Symbol ID
    /// layer generating the recoding coefficients and creating the
    /// recoded symbol id (or encoding vector).
    template<template <class> class RecodingStack, class SuperCoder>
    class payload_recoder : public SuperCoder
    {
    public:

        /// The recoding stack. Any calls not implemented in the
        /// recoding stack will be forwarded to the SuperCoder
        /// stack typically via the proxy_layer class.
        typedef RecodingStack<SuperCoder> recode_stack;

        typedef typename SuperCoder::pointer pointer;

        typedef typename recode_stack::pointer recode_pointer;

    public:

        /// @ingroup factory_layers
        /// The factory layer associated with this coder.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size),
                  m_stack_factory(max_symbols, max_symbol_size)
                {
                    m_stack_factory.set_proxy(this);
                }

            /// @copydoc layer::factory::build(uint32_t,uint32_t)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    assert(SuperCoder::factory::max_payload_size() ==
                           m_stack_factory.max_payload_size());

                    auto coder =
                        SuperCoder::factory::build(symbols, symbol_size);

                    // This is a bit more complicated than I would
                    // like it to be. Anyway here is a brief description
                    // of what is going on. The recoding stack is
                    // implemented as a parallel stack i.e. it does
                    // not affect the public API of the decoding stack
                    // (the main stack).
                    // To do this we use the proxy_layer to avoid
                    // having to re-implement functions from the
                    // main stack. What complicates this is that,
                    // the recoding stack also needs to be constructed
                    // and initialized. We cannot do it from the
                    // build() function in the proxy factory since
                    // we at that time have not specified any proxy
                    // this means build() in the proxy layer cannot call
                    // construct() and initialize() so we have to do it
                    // here (after setting the proxy so calls can be
                    // forwarded). Adding some of complexity in layers
                    // using a proxy.

                    auto recoder =
                        m_stack_factory.build(symbols, symbol_size);

                    if(!coder->has_recode_stack())
                    {
                        recoder->set_proxy(coder.get());
                        coder->set_recode_stack(recoder);

                        recoder->construct(
                            SuperCoder::factory::max_symbols(),
                            SuperCoder::factory::max_symbol_size());

                        recoder->initialize(symbols, symbol_size);

                    }

                    return coder;
                }

        private:

            typename recode_stack::factory m_stack_factory;

        };

    public:

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {

                if(has_recode_stack())
                {
                    m_recode_stack->initialize(symbols, symbol_size);

                    assert(SuperCoder::payload_size() ==
                           m_recode_stack->payload_size());
                }

                SuperCoder::initialize(symbols, symbol_size);
            }

        /// @copydoc layer::recode(uint8_t*)
        void recode(uint8_t *payload)
            {
                m_recode_stack->encode(payload);
            }

    private:

        void set_recode_stack(const recode_pointer &recoder)
            {
                assert(!m_recode_stack);
                m_recode_stack = recoder;
            }

        bool has_recode_stack() const
            {
                return m_recode_stack.get() != 0;
            }

    protected:

        /// Store the recode stack
        recode_pointer m_recode_stack;

    };
}

#endif

