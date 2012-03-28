// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_FULL_VECTOR_ENCODER_H
#define KODO_FULL_VECTOR_ENCODER_H

#include <stdint.h>

#include "full_vector.h"

namespace kodo
{

    /// The full vector encoder 
    template<template <class> class GeneratorBlock, class SuperCoder>
    class full_vector_encoder : public SuperCoder
    {
    public:

        /// The field type
        typedef typename SuperCoder::field_type field_type;

        /// The value type
        typedef typename field_type::value_type value_type;

        /// The encoding vector
        typedef full_vector<field_type> vector_type;

        /// The vector generator type
        typedef GeneratorBlock<value_type> generator_block;

        /// Pointer to coder produced by the factories
        typedef typename SuperCoder::pointer pointer;

    public:

        /// The factory layer associated with this coder.
        /// Maintains the block generator needed for the encoding
        /// vectors.
        class factory : public SuperCoder::factory
        {
        public:
            
            /// @see final_coder_factory::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @see final_coder_factory::factory(..)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder = SuperCoder::factory::build(symbols, symbol_size);

                    uint32_t vector_length = vector_type::length( symbols );
                    
                    typename generator_block::pointer block =
                        m_generator_factory.build( vector_length );
                    
                    coder->m_generator = block;
                    
                    return coder;
                }

            /// @return the required payload buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    uint32_t max_vector_size =
                        vector_type::size( SuperCoder::factory::max_symbols() );

                    assert(max_vector_size > 0);
                    
                    return max_vector_size;
                }
            
        private:

            typename generator_block::factory m_generator_factory;
        };

        
    public:

        /// @see final_coder_factory::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                
                m_vector_size = vector_type::size( symbols );
                assert(m_vector_size > 0);

                m_count = 0;
            }

        /// Iterates over the symbols stored in the encoding symbol id part
        /// of the payload id, and calls the encode_symbol function.
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                /// Get the data for the encoding vector
                value_type *vector
                    = reinterpret_cast<value_type*>(symbol_id);

                /// Fill the encoding vector
                assert(m_generator);

                m_generator->fill(m_count, vector); 
                ++m_count;

                /// Cast the symbol to the correct field value_type
                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                encode_with_vector(symbol, vector);

                return m_vector_size;
            }        

        /// Using this function we may "encode" an uncoded symbol.
        /// This function basically copies a specific symbol to the
        /// symbol buffer unmodified, no headers or similar are added
        /// to the payload buffer. I.e. the user must track which symbol is
        /// fetched him/her-self. Can be used in special cases
        /// where a specific symbols data is needed.
        /// If an actual systematic encoder/decoder is needed check
        /// the kodo/systematic_encoder.h and kodo/systematic_decoder.h
        /// For this function the user provides the buffer alternatively
        /// if one simply wants to access the raw data of a symbol
        /// the symbol storage classes contains the raw_symbol(..) function
        /// which returns the buffer of a symbol.
        uint32_t encode_raw(uint32_t symbol_index, uint8_t *symbol_data)
            {
                assert(symbol_data != 0);
                assert(symbol_index < SuperCoder::symbols());
                
                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                /// Did you forget to set the data on the encoder?
                assert(SuperCoder::symbol(symbol_index) != 0);
                
                std::copy(SuperCoder::symbol(symbol_index),
                          SuperCoder::symbol(symbol_index) + SuperCoder::symbol_length(),
                          symbol);

                return 0;
            }
        
        /// @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return m_vector_size;
            }

    protected:

        /// Encodes a symbol according to the encoding vector
        /// @param symbol_data the destination buffer for the encoded symbol
        /// @param vector_data the encoding vector - note at this point the
        ///        encoding vector should already be initialized with coding
        ///        coefficients.
        void encode_with_vector(value_type *symbol_data, value_type *vector_data)
            {
                assert(symbol_data != 0);
                assert(vector_data != 0);

                vector_type encoding_vector(vector_data, SuperCoder::symbols());

                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    if(value_type coefficient = encoding_vector.coefficient(i))
                    {
                        const value_type *symbol_i = SuperCoder::symbol( i );

                        /// Did you forget to set the data on the encoder?
                        assert(symbol_i != 0);
                        
                        if(fifi::is_binary<field_type>::value)
                        {
                            SuperCoder::add(symbol_data, symbol_i,
                                            SuperCoder::symbol_length());
                        }
                        else
                        {
                            SuperCoder::multiply_add(symbol_data, symbol_i,
                                                     coefficient,
                                                     SuperCoder::symbol_length());
                        }
                    }
                }
            } 
        
    protected:
        
        /// The size of the encoding vector in bytes
        uint32_t m_vector_size;

        /// Keeping track of the number of packets sent
        uint32_t m_count;
        
        /// The encoding vector generator
        typename generator_block::pointer m_generator;
        
    };
}        

#endif

