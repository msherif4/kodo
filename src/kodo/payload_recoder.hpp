// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

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

        /// Pointer to the coder constructed by the factory
        typedef typename SuperCoder::pointer pointer;

        /// Pointer to the recoding stack
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
                m_stack_factory.set_factory_proxy(this);
            }

            /// Make sure we have enough space for both the payload
            /// produced by the main stack and the recoding stack.
            /// @copydoc layer::factory::max_payload_size() const
            uint32_t max_payload_size() const
            {
                return std::max(SuperCoder::factory::max_payload_size(),
                                m_stack_factory.max_payload_size());
            }

        private:

            /// Give the layer access
            friend class payload_recoder;

            /// @return A reference to recoding stack factory
            typename recode_stack::factory& recode_factory()
            {
                return m_stack_factory;
            }

        private:

            typename recode_stack::factory m_stack_factory;

        };

    public:

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            SuperCoder::initialize(the_factory);

            // We have to postpone building the recoding stack to the
            // initialize function, since we have to ensure that the
            // main stack has been constructed and initialized.

            auto& stack_factory = the_factory.recode_factory();

            if(!m_recode_stack)
            {
                stack_factory.set_stack_proxy(this);
                m_recode_stack = stack_factory.build();
            }
            else
            {

                m_recode_stack->initialize(the_factory.recode_factory());
            }
        }

        /// @copydoc layer::recode(uint8_t*)
        uint32_t recode(uint8_t *payload)
        {
            assert(m_recode_stack);
            return m_recode_stack->encode(payload);
        }

        /// Make sure we have enough space for both the payload
        /// produced by the main stack and the recoding stack.
        /// @copydoc layer::payload_size() const
        uint32_t payload_size() const
        {
            assert(m_recode_stack);
            return std::max(SuperCoder::payload_size(),
                            m_recode_stack->payload_size());
        }

    private:

        /// Sets the recoding stack on the coder
        /// @param recoder The recoding stack.
        void set_recode_stack(const recode_pointer &recoder)
        {
            assert(!m_recode_stack);
            m_recode_stack = recoder;
        }

        /// @return True if the coder already has already been initialized
        ///         with an recoding stack
        bool has_recode_stack() const
        {
            return m_recode_stack.get() != 0;
        }

    protected:

        /// Store the recode stack
        recode_pointer m_recode_stack;

    };

}


