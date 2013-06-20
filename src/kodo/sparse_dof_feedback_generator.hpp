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
    class sparse_dof_feedback_generator : public SuperCoder
    {
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


