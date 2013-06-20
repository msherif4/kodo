// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

namespace kodo
{

    /// @todo: fix docs
    template<class SuperCoder>
    class sparse_staircase_feedback_generator : public SuperCoder
    {
    public:

        typedef std::function<void ()> generate_feedback_callback;

    public:

        template<class Factory>
        void initialize(Factory& factory)
        {
            SuperCoder::initialize(factory);
            m_feedback_threshold = (uint32_t)std::ceil(SuperCoder::symbols() * 0.05);
            m_feedback_sent = false;

            assert(SuperCoder::symbols() > m_feedback_threshold);
        }

        void decode(uint8_t *payload)
        {
            SuperCoder::decode(payload);

            if(m_feedback_sent)
                return;

            if(SuperCoder::rank() > SuperCoder::symbols() - m_feedback_threshold)
            {
                m_feedback_sent = true;

                assert(m_feedback_callback);
                m_feedback_callback();
            }
        }

        void set_feedback_callback(generate_feedback_callback callback)
        {
            m_feedback_callback = callback;
        }

    protected:

        generate_feedback_callback m_feedback_callback;
        bool m_feedback_sent;
        uint32_t m_feedback_threshold;

    };

}


