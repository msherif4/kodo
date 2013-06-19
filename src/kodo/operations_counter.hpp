// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

namespace kodo
{

    /// Helper class which is used by the finite_field_counter
    /// layer to count the number of operations performed.
    struct operations_counter
    {

        /// Constructs a new counter and zero initializes the
        /// counter.
        operations_counter()
            : m_multiply(0),
              m_multiply_add(0),
              m_add(0),
              m_multiply_subtract(0),
              m_subtract(0),
              m_invert(0)
            { }

        /// Counter for dest[i] = dest[i] * constant
        uint32_t m_multiply;

        /// Counter for dest[i] = dest[i] + (constant * src[i])
        uint32_t m_multiply_add;

        /// Counter for dest[i] = dest[i] + src[i]
        uint32_t m_add;

        /// Counter for dest[i] = dest[i] - (constant * src[i])
        uint32_t m_multiply_subtract;

        /// Counter for dest[i] = dest[i] - src[i]
        uint32_t m_subtract;

        /// Counter for invert(value)
        uint32_t m_invert;

    };

    /// Subtract two operations counters ala. a - b
    /// @param a The operations counter to be reduced
    /// @param b The operations counter subtracted from a
    inline operations_counter operator-(const operations_counter &a,
                                        const operations_counter &b)
    {
        operations_counter res;

        // Added asserts to detect underflow

        assert(a.m_multiply >= b.m_multiply);
        res.m_multiply = a.m_multiply - b.m_multiply;

        assert(a.m_multiply_add >= b.m_multiply_add);
        res.m_multiply_add = a.m_multiply_add - b.m_multiply_add;

        assert(a.m_add >= b.m_add);
        res.m_add = a.m_add - b.m_add;

        assert(a.m_multiply_subtract >= b.m_multiply_subtract);
        res.m_multiply_subtract =
            a.m_multiply_subtract - b.m_multiply_subtract;

        assert(a.m_subtract >= b.m_subtract);
        res.m_subtract = a.m_subtract - b.m_subtract;

        assert(a.m_invert >= b.m_invert);
        res.m_invert = a.m_invert - b.m_invert;
        return res;
    }

    /// Subtract two operations counters ala. a + b
    /// @param a The operations counter to be reduced
    /// @param b The operations counter subtracted from a
    inline operations_counter operator+(const operations_counter &a,
                                        const operations_counter &b)
    {
        operations_counter res;

        // Added asserts to detect overflow

        res.m_multiply = a.m_multiply + b.m_multiply;
        assert(res.m_multiply >= a.m_multiply);

        res.m_multiply_add = a.m_multiply_add + b.m_multiply_add;
        assert(res.m_multiply_add >= a.m_multiply_add);

        res.m_add = a.m_add + b.m_add;
        assert(res.m_add >= a.m_add);

        res.m_multiply_subtract =
            a.m_multiply_subtract + b.m_multiply_subtract;
        assert(res.m_multiply_subtract >= a.m_multiply_subtract);

        res.m_subtract = a.m_subtract + b.m_subtract;
        assert(res.m_subtract >= a.m_subtract);

        res.m_invert = a.m_invert + b.m_invert;
        assert(res.m_invert >= a.m_invert);

        return res;
    }

    /// Implement the greater than or equal operator such as a >= b
    /// @param a The first operations counter
    /// @param b The second operations counter
    inline bool operator>=(const operations_counter &a,
                           const operations_counter &b)
    {
        if(a.m_multiply >= b.m_multiply)
            return true;
        if(a.m_multiply < b.m_multiply)
            return false;
        if(a.m_multiply_add >= b.m_multiply_add)
            return true;
        if(a.m_multiply_add < b.m_multiply_add)
            return false;
        if(a.m_add >= b.m_add)
            return true;
        if(a.m_add < b.m_add)
            return false;
        if(a.m_multiply_subtract >= b.m_multiply_subtract)
            return true;
        if(a.m_multiply_subtract < b.m_multiply_subtract)
            return false;
        if(a.m_subtract >= b.m_subtract)
            return true;
        if(a.m_subtract < b.m_subtract)
            return false;
        if(a.m_invert >= b.m_invert)
            return true;
        if(a.m_invert < b.m_invert)
            return false;

        return false;
    }

}

