// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_RECODER_H
#define KODO_RLNC_FULL_VECTOR_RECODER_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include "../linear_block_vector.h"

namespace kodo
{

    /// Implementes a simple uniform random encoding scheme
    /// where the payload_id carries all coding coefficients
    /// i.e. the "encoding vector"
    template< template <class> class RandomGenerator,
              template <class> class Decoder,
              class SuperCoder>
    class full_vector_recoder : public SuperCoder
    {
    public:

        /// The field we use
        typedef typename SuperCoder::field_type field_type;

        /// The value_type used to store the field elements
        typedef typename field_type::value_type value_type;

        /// The random generator used
        typedef RandomGenerator<value_type> random_generator;

        /// The decoder type
        typedef Decoder<field_type> decoder_type;

        /// The decoder that is used during recoding
        typedef typename decoder_type::pointer decoder_ptr;

        /// The encoding vector
        typedef typename decoder_type::vector_type vector_type;


    public:

        /// @see final_coder::construct(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                uint32_t max_vector_length = vector_type::length(max_symbols);
                m_recoding_vector.resize(max_vector_length);
            }

        void set_decoder(decoder_ptr decoder)
            {
                assert(decoder);
                m_decoder = decoder;

                m_symbols = m_decoder->symbols();
                m_vector_size = vector_type::size(m_symbols);
            }

        /// The recode function is special for Network Coding
        /// algorithms.
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);
                assert(m_decoder);

                std::fill_n(symbol_data, m_decoder->symbol_size(), 0);
                std::fill_n(symbol_id, m_decoder->vector_size(), 0);

                uint32_t rank = m_decoder->rank();

                if(rank == 0)
                {
                    /// We do not have any symbols in our buffer
                    /// we have to simply output a zero encoding vector
                    /// and symbol
                    return m_vector_size;
                }

                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                value_type *vector
                    = reinterpret_cast<value_type*>(symbol_id);

                uint32_t recode_vector_length = vector_type::length(rank);
                m_random.generate(&m_recoding_vector[0], recode_vector_length);

                uint32_t coefficient_index = 0;

                for(uint32_t i = 0; i < m_symbols; ++i)
                {
                    if(!m_decoder->symbol_exists(i))
                        continue;

                    value_type coefficient =
                        vector_type::coefficient(coefficient_index,
                                                 &m_recoding_vector[0]);

                    if(coefficient)
                    {
                        value_type *vector_i = m_decoder->vector( i );
                        value_type *symbol_i = m_decoder->symbol( i );

                        if(fifi::is_binary<field_type>::value)
                        {
                            SuperCoder::add(vector, vector_i,
                                            m_decoder->vector_length());

                            SuperCoder::add(symbol, symbol_i,
                                            m_decoder->symbol_length());
                        }
                        else
                        {
                            SuperCoder::multiply_add(vector, vector_i,
                                                     coefficient,
                                                     m_decoder->vector_length());

                            SuperCoder::multiply_add(symbol, symbol_i,
                                                     coefficient,
                                                     m_decoder->symbol_length());
                        }
                    }

                    ++coefficient_index;
                    if(coefficient_index == rank)
                        break;

                }

                return m_vector_size;

            }
    private:

        /// Temporary buffer storing the recoding vector (or in
        /// RLNC termonology the local encoding vector)
        std::vector<value_type> m_recoding_vector;

        /// Random generator use to produce random coefficients
        random_generator m_random;

        uint32_t m_symbols;

        ///
        uint32_t m_vector_size;

        /// Decoder
        decoder_ptr m_decoder;

    };
}

#endif

