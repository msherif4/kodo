// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_LINEAR_BLOCK_VECTOR_HPP
#define KODO_LINEAR_BLOCK_VECTOR_HPP

#include <stdint.h>

#include <fifi/field_types.hpp>

namespace kodo
{
    /// Wraps the memory of the coding coefficients,
    /// corresponding to a specific encoded symbol.
    /// This representation assumes that each original
    /// symbol used to produce one encoded symbol is
    /// represented by a coding coefficient.
    template<class Field>
    class linear_block_vector
    {
    public:

        /// The field used
        typedef typename Field::value_type value_type;

    public:

        /// @param index the index of the coffecient to return
        /// @param vector buffer where the coefficients are stored
        /// @return the coefficient for a specific symbol index
        static value_type coefficient(uint32_t index,
                                      const value_type *vector);

        /// Sets the coefficient for a specific symbol index
        /// @param index the index of the symbol coefficient
        /// @param vector buffer where the coefficient should be set
        /// @param coefficient to assign
        static void set_coefficient(uint32_t index,
                                    value_type *vector,
                                    value_type coefficient);

        /// @return the size of vector in value_type elements
        /// Needed to store the encoding vector for a certain number
        /// of symbols in a specific field.
        /// Used when several elements are packed into
        /// a single value_type, currently only used for the
        /// binary field.
        static uint32_t length(uint32_t symbols);

        /// @return the size of a vector in bytes
        static uint32_t size(uint32_t symbols);

    };

    template<class Field>
    inline typename Field::value_type
    linear_block_vector<Field>::coefficient(uint32_t index,
                                            const value_type *vector)
    {
        assert(vector != 0);
        return vector[index];
    }

    template<>
    inline fifi::binary::value_type
    linear_block_vector<fifi::binary>::coefficient(uint32_t index,
                                                   const value_type *vector)
    {
        assert(vector != 0);

        typedef fifi::binary::value_type value_type;

        uint32_t array_index = index / std::numeric_limits<value_type>::digits;
        uint32_t offset = index % std::numeric_limits<value_type>::digits;

        return (vector[array_index] >> offset) & 0x1;
    }

    template<class Field>
    inline void
    linear_block_vector<Field>::set_coefficient(uint32_t index,
                                                value_type *vector,
                                                typename Field::value_type coefficient)
    {
        assert(vector != 0);
        vector[index] = coefficient;
    }

    template<>
    inline void linear_block_vector<fifi::binary>::set_coefficient(uint32_t index,
                                                                   value_type *vector,
                                                    fifi::binary::value_type coefficient)
    {
        assert(vector != 0);
        assert(coefficient < 2); // only {0,1} allowed

        uint32_t array_index = index / std::numeric_limits<fifi::binary::value_type>::digits;
        uint32_t offset = index % std::numeric_limits<fifi::binary::value_type>::digits;

        fifi::binary::value_type mask = 1 << offset;

        if(coefficient)
        {
            vector[array_index] |= mask;
        }
        else
        {
            vector[array_index] &= ~mask;
        }

    }



    template<class Field>
    inline uint32_t
    linear_block_vector<Field>::length(uint32_t symbols)
    {
        return symbols;
    }

    template<>
    inline uint32_t
    linear_block_vector<fifi::binary>::length(uint32_t symbols)
    {
        assert(symbols > 0);

        // Note: std::numeric_limits<value_type>::digits
        // returns the number of bits in the template parameter

        // ceil(x/y) = ((x - 1) / y) + 1
        return ((symbols - 1) / std::numeric_limits<fifi::binary::value_type>::digits) + 1;
    }

    template<class Field>
    uint32_t
    linear_block_vector<Field>::size(uint32_t symbols)
    {
        return length(symbols) * sizeof(value_type);
    }
}

#endif

