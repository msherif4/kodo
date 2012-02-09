// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#ifndef KODO_RLNC_FULL_VECTOR_RECODER_H
#define KODO_RLNC_FULL_VECTOR_RECODER_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include "full_vector.h"

namespace kodo
{

    // Implementes a simple uniform random encoding scheme
    // where the payload_id carries all coding coefficients
    // i.e. the "encoding vector"
    template< template <class> class RandomGenerator, class SuperCoder>
    class full_vector_recoder : public SuperCoder
    {
    public:

        // The field we use
        typedef typename SuperCoder::field_type field_type;

        // The value_type used to store the field elements
        typedef typename field_type::value_type value_type;

        // The encoding vector
        typedef typename SuperCoder::vector_type vector_type;

        // The random generator used
        typedef RandomGenerator<value_type> random_generator;

        // The rank of the current decoder
        using SuperCoder::m_rank;
        
    public:

        // @see final_coder::construct(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                uint32_t max_vector_length = vector_type::length(max_symbols);
                m_recoding_vector.resize(max_vector_length);
            }

        // The recode function is special for Network Coding
        // algorithms. It
        uint32_t recode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                std::fill_n(symbol_data, SuperCoder::symbol_size(), 0);
                std::fill_n(symbol_id, SuperCoder::vector_size(), 0);

                if(m_rank == 0)
                {
                    // We do not have any symbols in our buffer
                    // we have to simply output a zero encoding vector
                    // and symbol
                    return SuperCoder::vector_size();
                }

                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                value_type *vector
                    = reinterpret_cast<value_type*>(symbol_id);

                uint32_t recode_vector_length = vector_type::length(m_rank);
                m_random.generate(&m_recoding_vector[0], recode_vector_length);

                vector_type recoding_vector(&m_recoding_vector[0], m_rank);
                
                //
                uint32_t coefficient_index = 0;
                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    if(!SuperCoder::symbol_exists(i))
                        continue;
                    
                    value_type coefficient =
                        recoding_vector.coefficient(coefficient_index);


                    if(coefficient)
                    {
                        value_type *vector_i = SuperCoder::vector( i );
                        value_type *symbol_i = SuperCoder::symbol( i );

                        if(fifi::is_binary<field_type>::value)
                        {
                            SuperCoder::add(vector, vector_i,
                                            SuperCoder::vector_length());
                                
                            SuperCoder::add(symbol, symbol_i,
                                            SuperCoder::symbol_length());
                        }
                        else
                        {
                            SuperCoder::multiply_add(vector, vector_i,
                                                     coefficient,
                                                     SuperCoder::vector_length());
                                
                            SuperCoder::multiply_add(symbol, symbol_i,
                                                     coefficient,
                                                     SuperCoder::symbol_length());
                        }
                    }
                    
                    ++coefficient_index;
                    if(coefficient_index == m_rank)
                        break;

                }
                
                return SuperCoder::vector_size();
                
            }
    private:
        
        std::vector<value_type> m_recoding_vector;
        random_generator m_random;
               
    };    
}

#endif

