// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#ifndef KODO_RLNC_SEED_ENCODER_H
#define KODO_RLNC_SEED_ENCODER_H

#include <stdint.h>
#include <sak/convert_endian.h>

#include "full_vector.h"
#include "full_vector_encoder.h"

namespace kodo
{

    // The purpose of the seed encoder is to avoid sending a full
    // encoding vector which for high fields may consume a significant
    // number of bytes.
    // The seed encoder instead only prepends the seed/id used to generate
    // the encoding vector. A decoder may the utilize this seed/id to recreate
    // the encoding vector on the receiving side.
    template<template <class> class GeneratorBlock, class SuperCoder>
    class seed_encoder : public full_vector_encoder<GeneratorBlock, SuperCoder>
    {
    public:

        // The base class of this layer
        typedef full_vector_encoder<GeneratorBlock, SuperCoder> Super;

        // The field type
        typedef typename Super::field_type field_type;

        // The value type
        typedef typename Super::value_type value_type;

        // The encoding vector
        typedef typename Super::vector_type vector_type;

        // The vector generator type
        typedef typename Super::generator_block generator_block;

        // Pointer to coder produced by the factories
        typedef typename Super::pointer pointer;

        // The seed_id type
        typedef uint32_t seed_id;

        // We using the block generator form the full_vector_encoder
        using Super::m_generator;
        
    public:

        // The factory layer associated with this coder.
        // In this case only needed to provide the max_payload_size()
        // function.
        class factory : public Super::factory
        {
        public:
            
            // @see final_coder_factory::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : Super::factory(max_symbols, max_symbol_size)
                { }

            // @return the required payload buffer size in bytes
            uint32_t max_symbol_id_size() const
                {                    
                    return sizeof(seed_id);
                }
        };

    public:

        // @see final_coder_factory(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                Super::construct(max_symbols, max_symbol_size);

                uint32_t max_vector_length =
                    vector_type::length(max_symbols);

                m_vector_data.resize(max_vector_length);
            }

        // @see final_coder_factory::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                Super::initialize(symbols, symbol_size);
                
                m_vector_size = vector_type::size( symbols );
                assert(m_vector_size > 0);

                m_seed_id = 0;
            }

        // Iterates over the symbols stored in the encoding symbol id part
        // of the payload id, and calls the encode_symbol function.
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                // Put in the current seed_id
                sak::big_endian::put<seed_id>(m_seed_id, symbol_id);
                
                // Fill the encoding vector
                assert(m_generator);

                m_generator->fill(m_seed_id, &m_vector_data[0]); 
                ++m_seed_id;
                
                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                Super::encode_with_vector(symbol, &m_vector_data[0]);

                return symbol_id_size();
            }        

        // Raw encoder
        // @to-do remove?
        uint32_t encode_raw(uint32_t symbol_id, uint8_t *payload)
            {
                return Super::encode_raw(symbol_id, payload);
            }
        
        // @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return sizeof(seed_id);
            }
        
    private:

        // The encoding vector buffer
        std::vector<value_type> m_vector_data;
        
        // The size of the encoding vector in bytes
        uint32_t m_vector_size;

        // Keeping track of the number of packets sent
        seed_id m_seed_id;
                
    };


    
    

}        

#endif
