// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

namespace kodo
{

    /// @ingroup finite_field_layers
    /// Terminates the layered coder and contains the coder allocation
    /// policy
    template<class Field, class SuperCoder>
    class finite_field_info : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef Field field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

    };

}

