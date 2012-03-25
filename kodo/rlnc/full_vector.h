// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_H
#define KODO_RLNC_FULL_VECTOR_H

#include <stdint.h>
#include <fifi/field_types.h>

namespace kodo
{

    // Basic encoding vector structure, used to access
    // the symbols included and the coding coefficients
    // The full_encoding_vector assumes that the
    // symbol coefficients are stored in one continuous
    // buffer.
    template<class Field>
    class full_vector
    {
    public:

        // The field used
        typedef typename Field::value_type value_type;

    public:

        // Creates a new basic encoding vector
        // @param vector, pointer to the encoding vector data
        // @param symbols, the number of field symbols i.e. the
        //        generation size 
        full_vector(value_type *vector, uint32_t symbols)
            : m_vector(vector),
              m_symbols(symbols)
            {
                assert(m_vector != 0);
                assert(m_symbols > 0);
            }

        // @return the coefficient for a specific symbol index
        value_type coefficient(uint32_t index) const;

        // Sets the coefficient for a specific symbol index
        // @param index, the index of the symbol coefficient
        // @param coefficient, the coefficient to assign
        void set_coefficient(uint32_t index, value_type coefficient);
        
        // @return pointer to the stored encoding vector data
        value_type* data()
            {
                return m_vector;
            }

        // @return pointer to the stored encoding vector data
        const value_type* data() const
            {
                return m_vector;
            }

        // @return the number of symbols represented by the vector
        uint32_t symbols() const
            {
                return m_symbols;
            }
        
        // @return the size of vector in value_type elements
        // Needed to store the encoding vector for a certain number
        // of symbols in a specific field.
        // Used when several elements are packed into
        // a single value_type, currently only used for the
        // binary field.
        static uint32_t length(uint32_t symbols);

        // @return the size of a vector in bytes
        static uint32_t size(uint32_t symbols)
            {
                return length(symbols) * sizeof(value_type);
            }
        
    private:

        // The encoding vector data
        value_type *m_vector;

        // The number of symbols in the encoding vector
        uint32_t m_symbols;
        
    };

    template<class Field>
    inline typename Field::value_type
    full_vector<Field>::coefficient(uint32_t index) const
    {
        return m_vector[index];
    }

    template<>
    inline fifi::binary::value_type
    full_vector<fifi::binary>::coefficient(uint32_t index) const
    {
        typedef fifi::binary::value_type value_type;
        
        uint32_t array_index = index / std::numeric_limits<value_type>::digits;
        uint32_t offset = index % std::numeric_limits<value_type>::digits;

        return (m_vector[array_index] >> offset) & 0x1;
    }

    template<class Field>
    inline void full_vector<Field>::set_coefficient(uint32_t index,
                                             typename Field::value_type coefficient)
    {
        m_vector[index] = coefficient;
    }

    template<>
    inline void full_vector<fifi::binary>::set_coefficient(uint32_t index,
                                                    fifi::binary::value_type coefficient)
    {
        assert(m_vector != 0);
        assert(coefficient < 2); // only {0,1} allowed

        uint32_t array_index = index / std::numeric_limits<fifi::binary::value_type>::digits;
        uint32_t offset = index % std::numeric_limits<fifi::binary::value_type>::digits;

        fifi::binary::value_type mask = 1 << offset;

        if(coefficient)
        {
            m_vector[array_index] |= mask;
        }
        else
        {
            m_vector[array_index] &= ~mask;
        }
        
    }

    
    template<class Field>
    inline uint32_t
    full_vector<Field>::length(uint32_t symbols)
    {
        return symbols;
    }
    
    template<>
    inline uint32_t
    full_vector<fifi::binary>::length(uint32_t symbols)
    {
        assert(symbols > 0);
        
        // Note: std::numeric_limits<value_type>::digits
        // returns the number of bits in the template parameter
        
        // ceil(x/y) = ((x - 1) / y) + 1
        return ((symbols - 1) / std::numeric_limits<fifi::binary::value_type>::digits) + 1;
    }


    

    
}        

#endif

