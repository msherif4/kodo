// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "partial_decoding_tracker.hpp"
#include "has_partial_decoding_tracker.hpp"

namespace kodo
{

    /// @ingroup generic_api
    /// Generic function overload for cases where is_partial_complete is part
    /// of a stack. @see partial_decoding_tracker::is_partial_complete() const
    /// @param t The stack to query
    template<class T>
    inline bool is_partial_complete(const T& t)
    {
        return is_partial_complete<has_partial_decoding_tracker<T>::value>(t);
    }

    /// @ingroup generic_api
    /// @copydoc is_partial_complete(const T&)
    template<class T>
    inline bool is_partial_complete(const boost::shared_ptr<T>& t)
    {
        return is_partial_complete(*t);
    }

    /// @ingroup generic_api
    /// @copydoc is_partial_complete(const T&)
    template<bool what, class T>
    inline bool is_partial_complete(const T& t, char (*)[what] = 0)
    {
        // Explanation for the char (*)[what] here:
        // http://stackoverflow.com/a/6917354/1717320
        return t.is_partial_complete();
    }

    /// @ingroup generic_api
    /// @copydoc is_partial_complete(const T&)
    template<bool what, class T>
    inline bool is_partial_complete(const T& t, char (*)[!what] = 0)
    {
        (void)t;

        // We do the assert here - to make sure that this call is not
        // silently ignored in cases where the stack does not have the
        // is_partial_complete() function. However, this assert can
        // be avoided by using the has_partial_decoding_tracker
        assert(0);
        return false;
    }

}


