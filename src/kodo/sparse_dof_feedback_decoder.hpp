// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <sak/convert_endian.hpp>

namespace kodo
{

    /// @todo: fix docs
    template<class SuperCoder>
    class sparse_dof_feedback_decoder : public SuperCoder
    {
    public:

        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
            { }

            /// @copydoc layer::factory::max_feedback_size() const
            uint32_t max_feedback_size() const
            {
                return 4U;
            }
        };

    public:

        /// @copydoc layer::feedback_size() const
        uint32_t feedback_size() const
        {
            return 4U;
        }

        void write_feedback(uint8_t* feedback)
        {
            uint32_t dof = SuperCoder::rank();
            sak::big_endian::put<uint32_t>(dof, feedback);
        }

    };

}


