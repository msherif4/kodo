// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_SYSTEMATIC_ENCODER_H
#define KODO_SYSTEMATIC_ENCODER_H

#include <stdint.h>
#include <sak/convert_endian.h>

#include "systematic_base_coder.h" 

namespace kodo
{

    // Systematic encoder layer
    template<class SuperCoder>
    class systematic_encoder : public SuperCoder
    {
    public:
        
        // The field type
        typedef typename SuperCoder::field_type field_type;
        
        // The value type
        typedef typename field_type::value_type value_type;

        // The symbol count type
        typedef typename systematic_base_coder::counter_type
            counter_type;

        // The flag type
        typedef typename systematic_base_coder::flag_type
            flag_type;
        
    public:

        // The factory layer associated with this coder.
        // In this case only needed to provide the max_payload_size()
        // function.
        class factory : public SuperCoder::factory
        {
        public:
            
            // @see final_coder_factory::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            // @return the required symbol_id buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    return SuperCoder::factory::max_symbol_id_size() +
                        sizeof(flag_type) + sizeof(counter_type);
                }
        };

    public:
        
        // Constructor
        systematic_encoder()
            : m_count(0),
              m_systematic(true)
            { }

        // @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                
                // Reset the state
                m_count = 0;
                m_systematic = true;
            }
        
        // Iterates over the symbols stored in the encoding symbol id part
        // of the payload id, and calls the encode_symbol function.
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);
                
                if(m_systematic && ( m_count < SuperCoder::symbols() ))
                {
                    return encode_systematic(symbol_data,
                                             symbol_id);

                }
                else
                {
                    return encode_non_systematic(symbol_data,
                                                 symbol_id);                        
                        
                }
            }

        // @return, true if the encoder is in systematic mode
        bool is_systematic() const
            {
                return m_systematic;
            }

        // Set the encoder in systematic mode
        void systematic_on()
            {
                m_systematic = true;
            }

        // Turns off systematic mode
        void systematic_off()
            {
                m_systematic = false;
            }

        // @return the required symbol_id buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return SuperCoder::symbol_id_size() +
                    sizeof(flag_type) + sizeof(counter_type);
            }

    private:

        // Encodes a systematic packet
        uint32_t encode_systematic(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);
                
                // Flag systematic packet
                sak::big_endian::put<flag_type>(
                    systematic_base_coder::systematic_flag, symbol_id);
                
                // Set the symbol id
                sak::big_endian::put<counter_type>(
                    m_count, symbol_id + sizeof(flag_type));

                // Copy the symbol
                SuperCoder::encode_raw(m_count, symbol_data);

                ++m_count;

                return sizeof(flag_type) + sizeof(counter_type);
            }

        // Encodes a non-systematic packets
        uint32_t encode_non_systematic(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                // Flag non_systematic packet
                sak::big_endian::put<flag_type>(
                    systematic_base_coder::non_systematic_flag, symbol_id);
                
                uint32_t bytes_consumed =
                    SuperCoder::encode(symbol_data, symbol_id + sizeof(flag_type));
                
                return bytes_consumed + sizeof(flag_type);
            }
        
    private:

        // Keeps track of the number of symbol sent
        // allows us to switch to non-systematic
        // encoding after sending all source symbols
        // systematically
        counter_type m_count;

        // Allows the systematic mode to be disabled
        // at runtime
        bool m_systematic;
        
    };    
    
}

#endif
            
            
