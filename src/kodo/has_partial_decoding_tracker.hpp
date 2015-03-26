// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "partial_decoding_tracker.hpp"

namespace kodo
{

    /// @ingroup type_traits
    /// Type trait helper allows compile time detection of whether an
    /// encoder / decoder contains the partial_decoding_tracker
    ///
    /// Example:
    ///
    /// typedef kodo::full_rlnc8_decoder decoder_t;
    ///
    /// if(kodo::has_partial_decoding_tracker<decoder_t>::value)
    /// {
    ///     // Do something here
    /// }
    ///
    template<class T>
    struct has_partial_decoding_tracker
    {
        template<class U>
        static uint8_t test(const kodo::partial_decoding_tracker<U> *);

        static uint32_t test(...);

        static const bool value = sizeof(test(static_cast<T*>(0))) == 1;
    };

}


