// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#ifndef KODO_RLNC_SEED_DECODER_H
#define KODO_RLNC_SEED_DECODER_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <sak/convert_endian.h>

#include "full_vector.h"
#include "full_vector_decoder.h"

namespace kodo
{

    // Implementes a simple uniform random encoding scheme
    // where the payload_id carries all coding coefficients
    // i.e. the "encoding vector"
    template<template <class> class GeneratorBlock, class SuperCoder>
    class seed_decoder : public full_vector_decoder<SuperCoder>
    {
    public:

        // The base class of this layer
        typedef full_vector_decoder<SuperCoder> Super;

        // The field we use
        typedef typename Super::field_type field_type;

        // The value_type used to store the field elements
        typedef typename Super::value_type value_type;

        // The encoding vector
        typedef typename Super::vector_type vector_type;

        // Pointer to coder produced by the factories
        typedef typename Super::pointer pointer;
        
        // The vector generator type
        typedef GeneratorBlock<value_type> generator_block;

        // The seed_id type
        typedef uint32_t seed_id;
        
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

            // @see final_coder_factory::factory(..)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder = Super::factory::build(symbols, symbol_size);

                    uint32_t vector_length = vector_type::length( symbols );
                    
                    typename generator_block::pointer block =
                        m_generator_factory.build( vector_length );
                    
                    coder->m_generator = block;
                    
                    return coder;
                }


            // @return the required payload buffer size in bytes
            uint32_t max_symbol_id_size() const
                {                    
                    return sizeof(seed_id);
                }

        private:

            typename generator_block::factory m_generator_factory;
    
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

        
        // The decode function which consumes the payload
        // @param payload, the input payload
        void decode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                seed_id id =
                    sak::big_endian::get<seed_id>(symbol_id);

                // Fill the encoding vector
                assert(m_generator);

                m_generator->fill(id, &m_vector_data[0]); 
                
                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                Super::decode_with_vector(&m_vector_data[0], symbol);
            }

        // Raw payloads
        // void decode_raw(uint32_t symbol_id, const uint8_t *payload)
        //     {
        //         Super::decode_raw(symbol_id, payload);
        //     }
        
        // @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                // We were thinking about returning a full
                // vector here since that would allow
                // recoding. However, decoding a recoded
                // packet is not supported by seed decoder
                // so it would be confusing an wrong atm. 
                
                return sizeof(seed_id);
            }
        
    protected:

        // The encoding vector buffer
        std::vector<value_type> m_vector_data;
        
        // The encoding vector generator
        typename generator_block::pointer m_generator;
        
    };
    

    
}


#endif

