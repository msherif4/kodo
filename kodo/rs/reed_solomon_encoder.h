// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#ifndef KODO_RS_REED_SOLOMON_ENCODER_H
#define KODO_RS_REED_SOLOMON_ENCODER_H

#include <stdint.h>
#include <sak/convert_endian.h>


namespace kodo
{

    // Basic Reed-Solomon encoder 
    template<template <class> class GeneratorMatrix, class SuperCoder>
    class reed_solomon_encoder : public SuperCoder
    {
    public:

        // The field type
        typedef typename SuperCoder::field_type field_type;

        // The value type
        typedef typename field_type::value_type value_type;

        // The generator matrix type
        typedef GeneratorMatrix<field_type> generator_matrix;

        // Pointer to coder produced by the factories
        typedef typename SuperCoder::pointer pointer;

    public:

        // The factory layer associated with this coder.
        // Maintains the block generator needed for the encoding
        // vectors.
        class factory : public SuperCoder::factory
        {
        private:

            // Access to the finite field implementation used
            // stored in the finite_field_math layer
            using SuperCoder::factory::m_field;
            
        public:
            
            // @see final_coder_factory::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                {
                    // A Reed-Solomon code cannot support more symbols
                    // than 2^m - 1 where m is the length of the finite
                    // field elements in bytes
                    assert(max_symbols < field_type::order);
                }

            // @see final_coder_factory::factory(..)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder = SuperCoder::factory::build(symbols, symbol_size);
                    
                    boost::shared_ptr<generator_matrix> matrix =
                        boost::make_shared<generator_matrix>(false, symbols, m_field);
                    
                    coder->m_matrix = matrix;
                    
                    return coder;
                }

            // @return the required payload buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    return sizeof(value_type);
                }
        };

        
    public:

        // @see final_coder_factory::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_count = 0;
            }

        // Iterates over the symbols stored in the encoding symbol id part
        // of the payload id, and calls the encode_symbol function.
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);
                assert(m_count < field_type::order);
                
                // Store the encoding id
                sak::big_endian::put<value_type>(m_count, symbol_id);

                // Cast the symbol to the correct field value_type
                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                encode_with_id(symbol, m_count);

                // Now increment the count so that we use other
                // coefficients next time
                ++m_count;

                
                return sizeof(value_type);
            }

        // Resets the internal count - which will make the encoder start from
        // the beginning of th generator matrix
        void reset_count()
            {
                m_count = 0;
            }

        // Returns the coder 'count' i.e. the number of symbols already encoded
        uint32_t count() const
            {
                return m_count;
            }

        // Using this function we may "encode" an uncoded symbol.
        // This function basically copies a specific symbol to the
        // symbol buffer unmodified, no headers or similar are added
        // to the payload buffer. I.e. the user must track which symbol is
        // fetched him/her-self. Can be used in special cases
        // where a specific symbols data is needed.
        // If an actual systematic encoder/decoder is needed check
        // the kodo/systematic_encoder.h and kodo/systematic_decoder.h
        // For this function the user provides the buffer alternatively
        // if one simply wants to access the raw data of a symbol
        // the symbol storage classes contains the raw_symbol(..) function
        // which returns the buffer of a symbol.
        uint32_t encode_raw(uint32_t symbol_index, uint8_t *symbol_data)
            {
                assert(symbol_data != 0);
                assert(symbol_index < SuperCoder::symbols());
                
                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                // Did you forget to set the data on the encoder?
                assert(SuperCoder::symbol(symbol_index) != 0);
                
                std::copy(SuperCoder::symbol(symbol_index),
                          SuperCoder::symbol(symbol_index) + SuperCoder::symbol_length(),
                          symbol);

                return 0;
            }
        
        // @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return sizeof(value_type);
            }

    protected:

        // Encodes a symbol according to the encoding vector
        // @param symbol_data, the destination buffer for the encoded symbol
        // @param encoding_id, the encoding id - this corresponds to a set of
        //                     coefficients from the generator matrix
        void encode_with_id(value_type *symbol_data, uint32_t encoding_id)
            {
                assert(symbol_data != 0);
                assert(encoding_id < field_type::order);

                const value_type *encoding_coefficients =
                    m_matrix->coefficients(encoding_id);

                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    if(value_type coefficient = encoding_coefficients[i])
                    {
                        const value_type *symbol_i = SuperCoder::symbol( i );

                        // Did you forget to set the data on the encoder?
                        assert(symbol_i != 0);

                        SuperCoder::multiply_add(symbol_data, symbol_i,
                                                 coefficient,
                                                 SuperCoder::symbol_length());

                    }
                }
            } 
        
    protected:
        
        // Keeping track of the number of packets sent
        uint32_t m_count;
        
        // The generator matrix
        boost::shared_ptr<generator_matrix> m_matrix;
        
    };


    
    

}        

#endif

