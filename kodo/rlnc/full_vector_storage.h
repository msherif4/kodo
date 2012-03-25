// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_STORAGE_H
#define KODO_RLNC_FULL_VECTOR_STORAGE_H

#include <stdint.h>
#include <boost/shared_array.hpp>

#include "full_vector.h"

namespace kodo
{

    // Implementes a simple uniform random encoding scheme
    // where the payload_id carries all coding coefficients
    // i.e. the "encoding vector"
    template<class SuperCoder>
    class full_vector_storage : public SuperCoder
    {
    public:

        // The field we use
        typedef typename SuperCoder::field_type field_type;

        // The value_type used to store the field elements
        typedef typename field_type::value_type value_type;

        // The encoding vector type
        typedef full_vector<field_type> vector_type;
       
    public:

        // Constructor
        full_vector_storage()
            : m_vector_length(0),
              m_vector_size(0)
            { }
        
        // @see final_coder::construct(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                uint32_t max_vector_length =
                    vector_type::length(max_symbols);

                assert(max_vector_length > 0);
                assert(max_symbols > 0);

                // Note, we have as many vectors as we have symbols
                // therefore the maximum number of vectors also
                // equal the maximum number of symbols
                m_vector_storage.resize(max_symbols * max_vector_length, 0); 
            }

        // @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                m_vector_length = vector_type::length(symbols);
                m_vector_size = vector_type::size(symbols);
                
                assert(m_vector_size > 0);
                
                // Zero initialize the needed storage for the encoding
                // vectors (we have as many vectors as symbols)
                std::fill(m_vector_storage.begin(),
                          m_vector_storage.end(), 0);
                
            }
        
        // @return the specified vector
        value_type* vector(uint32_t index)
            {
                assert(index < SuperCoder::symbols());
                return &m_vector_storage[ index * m_vector_length ];
            }

        // @return the specified vector
        const value_type* vector(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());
                return &m_vector_storage[ index * m_vector_length ];
            }

        // @return the length of the vector in value_type elements
        uint32_t vector_length() const
            {
                return m_vector_length;
            }

        // @return the size of the vector in bytes
        uint32_t vector_size() const
            {
                return m_vector_size;
            }

    private:

        // Stores the encoding vectors
        std::vector<value_type> m_vector_storage;

        // The length of a vector in value_type elements
        uint32_t m_vector_length;

        // The size of a vector in bytes
        uint32_t m_vector_size;
    };
    

    
}


#endif

