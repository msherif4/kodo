// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_DECODER_H
#define KODO_RLNC_FULL_VECTOR_DECODER_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include "full_vector.h"

namespace kodo
{

    /// Implementes a simple uniform random encoding scheme
    /// where the payload_id carries all coding coefficients
    /// i.e. the "encoding vector"
    template<class SuperCoder>
    class full_vector_decoder : public SuperCoder
    {
    public:

        /// The field we use
        typedef typename SuperCoder::field_type field_type;

        /// The value_type used to store the field elements
        typedef typename field_type::value_type value_type;

        /// The encoding vector
        typedef full_vector<field_type> vector_type;
        
    public:

        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        public:
            
            /// @see final_coder_factory::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @return the required symbol id buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    uint32_t max_vector_size =
                        vector_type::size( SuperCoder::factory::max_symbols() );

                    assert(max_vector_size > 0);
                    
                    return  max_vector_size;
                }
        };

    public:

        /// Constructor
        full_vector_decoder()
            : m_rank(0),
              m_maximum_pivot(0)
            { }

        /// @see final_coder::construct(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                m_uncoded.resize(max_symbols, false);
                m_coded.resize(max_symbols, false);
            }

        /// @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                
                std::fill_n(m_uncoded.begin(), symbols, false);
                std::fill_n(m_coded.begin(), symbols, false);

                m_rank = 0;
                m_maximum_pivot = 0;
            }

        /// The decode function which consumes the payload
        /// @param payload, the input payload
        void decode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);
                                
                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);
                
                value_type *vector
                    = reinterpret_cast<value_type*>(symbol_id);
                
                decode_with_vector(vector, symbol);
            }

        /// The decode function for systematic packets i.e.
        /// specific uncoded symbols.
        void decode_raw(uint32_t symbol_index, const uint8_t *symbol_data)
            {
                assert(symbol_index < SuperCoder::symbols());
                assert(symbol_data != 0);
                
                if(m_uncoded[symbol_index])
                    return;

                const value_type *symbol
                    = reinterpret_cast<const value_type*>( symbol_data );

                if(m_coded[symbol_index])
                {
                    swap_decode(symbol_index, symbol);
                }
                else
                {
                    /// Stores the symbol and updates the corresponding
                    /// encoding vector
                    store_uncoded_symbol(symbol_index, symbol);

                    /// Backwards substitution
                    value_type *vector = SuperCoder::vector(symbol_index);
                    backward_substitute(symbol_index, vector, symbol);
                    
                    /// We have increased the rank if we have finished the
                    /// backwards substitution
                    ++m_rank;
                    
                    m_uncoded[ symbol_index ] = true;
                    
                    if(symbol_index > m_maximum_pivot)
                    {
                        m_maximum_pivot = symbol_index;
                    }
                    
                }
            }

        /// @return true if the decoding is complete
        bool is_complete() const
            {
                return m_rank == SuperCoder::symbols();
            }

        /// @return the rank of the decoder
        uint32_t rank() const
            {
                return m_rank;
            }

        /// @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return SuperCoder::vector_size();
            }
        
    protected:
        
        /// Decodes a symbol based on the vector
        /// @param symbol_data, buffer containing the encoding symbol
        /// @param vector_data, buffer containing the encoding vector
        void decode_with_vector(value_type *vector_data, value_type *symbol_data)
            {
                assert(symbol_data != 0);
                assert(vector_data != 0);                    
                
                /// See if we can find a pivot
                boost::optional<uint32_t> pivot_id
                    = forward_substitute_to_pivot(vector_data, symbol_data);

                if(!pivot_id)
                    return;
                
                if(!fifi::is_binary<field_type>::value)
                {
                    /// Normalize symbol and vector
                    normalize(*pivot_id, vector_data, symbol_data);
                }

                /// Reduce the symbol further
                forward_substitute_from_pivot(*pivot_id, vector_data, symbol_data);

                /// Now with the found pivot reduce the existing symbols
                backward_substitute(*pivot_id, vector_data, symbol_data);

                /// Now save the received symbol
                store_coded_symbol(*pivot_id, vector_data, symbol_data);

                /// We have increased the rank
                ++m_rank;

                m_coded[ *pivot_id ] = true;
                
                if(*pivot_id > m_maximum_pivot)
                {
                    m_maximum_pivot = *pivot_id;
                }
            }

        /// When adding a raw symbol (i.e. uncoded) with a specific pivot id and
        /// the decoder already contains a coded symbol in that position this
        /// function performs the proper swap between the two symbols.
        /// @param pivot_id, the pivot position of the raw symbol
        /// @param symbol_data, the data for the raw symbol
        void swap_decode(uint32_t pivot_id, const value_type *symbol_data)
            {
                assert(m_coded[pivot_id] == true);
                assert(m_uncoded[pivot_id] == false);

                m_coded[pivot_id] = false;
                
                value_type *symbol_i = SuperCoder::symbol(pivot_id);
                value_type *vector_i = SuperCoder::vector(pivot_id);

                vector_type encoding_vector(vector_i, SuperCoder::symbols());
                value_type value = encoding_vector.coefficient( pivot_id );

                assert(value == 1);

                /// Subtract the new pivot symbol
                encoding_vector.set_coefficient(pivot_id, 0);

                SuperCoder::subtract(symbol_i, symbol_data,
                                     SuperCoder::symbol_length());

                /// Now continue our new coded symbol we know that it must
                /// if found it will contain a pivot id > that the current.
                decode_with_vector(vector_i, symbol_i);

                /// The previous vector may still be in memory
                std::fill_n(vector_i, SuperCoder::vector_length(), 0);
                    
                store_uncoded_symbol(pivot_id, symbol_data);

                m_uncoded[pivot_id] = true;

                /// No need to backwards substitute since we are
                /// replacing an existing symbol. I.e. backwards
                /// substitution must already have been done.
            }
        
        /// Iterates the encoding vector from where a pivot has been identified
        /// and subtracts existing symbols
        /// @param pivot_id, the index of the found pivot element
        /// @param vector_data, the data constituting the encoding vector
        /// @param symbol_data, the data of the encoded symbol
        void normalize(uint32_t pivot_id,
                       value_type *vector_data,
                       value_type *symbol_data)
            {

                assert(vector_data != 0);
                assert(symbol_data != 0);
                
                assert(pivot_id < SuperCoder::symbols());
                
                assert(m_uncoded[pivot_id] == false);
                assert(m_coded[pivot_id] == false);
                
                vector_type encoding_vector(vector_data, SuperCoder::symbols());

                value_type coefficient = encoding_vector.coefficient( pivot_id );

                assert(coefficient > 0);

                value_type inverted_coefficient = SuperCoder::invert(coefficient);

                /// Update symbol and corresponding vector
                SuperCoder::multiply(vector_data, inverted_coefficient,
                                     SuperCoder::vector_length());
                
                SuperCoder::multiply(symbol_data, inverted_coefficient,
                                     SuperCoder::symbol_length());
                
            }
        
        /// Iterates the encoding vector and subtracts existing symbols until
        /// a pivot element is found.
        /// @param vector_data, the data constituting the encoding vector
        /// @param symbol_data, the data of the encoded symbol
        /// @return the pivot index if found.
        boost::optional<uint32_t> forward_substitute_to_pivot(value_type *vector_data,
                                                              value_type *symbol_data)
            {
                assert(vector_data != 0);
                assert(symbol_data != 0);
                                
                vector_type encoding_vector(vector_data, SuperCoder::symbols());
                
                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {

                    value_type current_coefficient
                        = encoding_vector.coefficient( i );
                    
                    if( current_coefficient )
                    {
                        /// If symbol exists 
                        if( symbol_exists( i ) )
                        {
                            value_type *vector_i = SuperCoder::vector( i );
                            value_type *symbol_i = SuperCoder::symbol( i );
                            
                            if(fifi::is_binary<field_type>::value)
                            {
                                SuperCoder::subtract(vector_data, vector_i,
                                                     SuperCoder::vector_length());
                                
                                SuperCoder::subtract(symbol_data, symbol_i,
                                                     SuperCoder::symbol_length());
                            }
                            else
                            {
                                SuperCoder::multiply_subtract(vector_data, vector_i,
                                                              current_coefficient,
                                                              SuperCoder::vector_length());
                                
                                SuperCoder::multiply_subtract(symbol_data, symbol_i,
                                                              current_coefficient,
                                                              SuperCoder::symbol_length());
                            }
                        }
                        else
                        {
                            return boost::optional<uint32_t>( i );
                        }
                    }
                }

                return boost::none;
            }

        /// Iterates the encoding vector from where a pivot has been identified
        /// and subtracts existing symbols
        /// @param pivot_id, the index of the found pivot element
        /// @param vector_data, the data constituting the encoding vector
        /// @param symbol_data, the data of the encoded symbol
        void forward_substitute_from_pivot(uint32_t pivot_id,
                                           value_type *vector_data,
                                           value_type *symbol_data)
            {
                /// We have received an encoded symbol - described
                /// by the symbol group. We now normalize the
                /// the encoding vector according to the symbol id.
                /// I.e. we make sure the pivot position has a "1"
                assert(vector_data != 0);
                assert(symbol_data != 0);
                                
                assert(pivot_id < SuperCoder::symbols());
                
                assert(m_uncoded[pivot_id] == false);
                assert(m_coded[pivot_id] == false);

                vector_type encoding_vector(vector_data, SuperCoder::symbols());
                
                /// If this pivot was smaller than the maximum pivot we have
                /// we also need to potentially backward substitute the higher
                /// pivot values into the new packet
                for(uint32_t i = pivot_id + 1; i <= m_maximum_pivot; ++i)
                {
                    /// Do we have a non-zero value here?
                    value_type value = encoding_vector.coefficient(i);

                    if( !value )
                    {
                        continue;
                    }

                    if( symbol_exists(i) )
                    {
                        value_type *vector_i = SuperCoder::vector(i);
                        value_type *symbol_i = SuperCoder::symbol(i);

                        if(fifi::is_binary<field_type>::value)
                        {
                            SuperCoder::subtract(vector_data, vector_i,
                                                 SuperCoder::vector_length());
                            
                            SuperCoder::subtract(symbol_data, symbol_i,
                                                 SuperCoder::symbol_length());
                        }
                        else
                        {
                            SuperCoder::multiply_subtract(vector_data, vector_i,
                                                          value,
                                                          SuperCoder::vector_length());
                            
                            SuperCoder::multiply_subtract(symbol_data, symbol_i,
                                                          value,
                                                          SuperCoder::symbol_length());
                            
                        }
                    }
                }
            }

        /// Backward substitute the found symbol into the
        /// existing symbols.
        /// @param pivot_id, the pivot index of the symbol in the
        ///        buffers vector_data and symbol_data
        /// @param vector_data, buffer containing the encoding vector
        /// @param symbol_data, buffer containing the encoding symbol
        void backward_substitute(uint32_t pivot_id,
                                 const value_type *vector_data,
                                 const value_type *symbol_data)
            {
                assert(vector_data != 0);
                assert(symbol_data != 0);
                
                assert(pivot_id < SuperCoder::symbols());
                
                assert(m_uncoded[pivot_id] == false);
                assert(m_coded[pivot_id] == false);
                
                /// We found a "1" that nobody else had as pivot, we now
                /// substract this packet from other coded packets
                /// - if they have a "1" on our pivot place
                for(uint32_t i = 0; i <= m_maximum_pivot; ++i)
                {
                    if( m_uncoded[i] )
                    {
                        /// We know that we have no non-zero elements
                        /// outside the pivot position.
                        continue;
                    }

                    if( m_coded[i] )
                    {
                        value_type *vector_i = SuperCoder::vector(i);
                        vector_type encoding_vector(vector_i, SuperCoder::symbols());

                        value_type value = encoding_vector.coefficient( pivot_id );

                        if( value )
                        {                            
                            value_type *symbol_i = SuperCoder::symbol(i);
                            
                            if(fifi::is_binary<field_type>::value)
                            {
                                SuperCoder::subtract(vector_i, vector_data,
                                                     SuperCoder::vector_length());
                                
                                SuperCoder::subtract(symbol_i, symbol_data,
                                                     SuperCoder::symbol_length());
                            }
                            else
                            {
                                
                                /// Update symbol and corresponding vector
                                SuperCoder::multiply_subtract(vector_i, vector_data,
                                                              value,
                                                              SuperCoder::vector_length());
                                
                                SuperCoder::multiply_subtract(symbol_i, symbol_data,
                                                              value,
                                                              SuperCoder::symbol_length());
                            }
                        }
                    }
                }                
            }

        /// Store an encoded symbol and encoding vector with the specified
        /// pivot found.
        /// @param pivot_id, the pivot index
        /// @param vector_data, buffer containing the encoding vector
        /// @param symbol_data, buffer containing the encoding symbol
        void store_coded_symbol(uint32_t pivot_id,
                                const value_type *vector_data,
                                const value_type *symbol_data)
            {
                assert(m_uncoded[pivot_id] == false);
                assert(m_coded[pivot_id] == false);
                assert(vector_data != 0);
                assert(symbol_data != 0);
                
                /// Copy it into the vector storage                                
                value_type *vector_dest = SuperCoder::vector( pivot_id );
                value_type *symbol_dest = SuperCoder::symbol( pivot_id );

                std::copy(vector_data,
                          vector_data + SuperCoder::vector_length(),
                          vector_dest);
                
                std::copy(symbol_data,
                          symbol_data + SuperCoder::symbol_length(),
                          symbol_dest);

                // We have increased the rank if we have finished do the
                // backwards substitution
                // ++m_rank;

                // m_coded[ pivot_id ] = true;
                
                // if(pivot_id > m_maximum_pivot)
                // {
                //     m_maximum_pivot = pivot_id;
                // }
            }

        /// Stores an uncoded or fully decoded symbol
        /// @param pivot_id, the pivot index of the symbol
        /// @param symbol_data, the data for the symbol 
        void store_uncoded_symbol(uint32_t pivot_id,
                                  const value_type *symbol_data)
            {
                assert(symbol_data != 0);
                assert(m_uncoded[pivot_id] == false);
                assert(m_coded[pivot_id] == false);
                
                /// Copy it into the symbol storage
                value_type *vector_dest = SuperCoder::vector( pivot_id );
                value_type *symbol_dest = SuperCoder::symbol( pivot_id );
                
                std::copy(symbol_data,
                          symbol_data + SuperCoder::symbol_length(),
                          symbol_dest);

                /// Update the corresponding vector
                vector_type encoding_vector(vector_dest, SuperCoder::symbols());
                encoding_vector.set_coefficient(pivot_id, 1);

                
                // We have increased the rank
                // ++m_rank;
                // m_uncoded[ pivot_id ] = true;
                
                // if(pivot_id > m_maximum_pivot)
                // {
                //     m_maximum_pivot = pivot_id;
                // }
            }

        /// @return true if the symbol with the specified id
        ///         has already been received in the decoder
        bool symbol_exists(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());
                return m_coded[ index ] || m_uncoded[ index ];
            }
        
    protected:

        /// The current rank of the decoder
        uint32_t m_rank;
        
        /// Stores the current maximum pivot index
        uint32_t m_maximum_pivot;
                
        /// Tracks whether a symbol is contained which
        /// is fully decoded
        std::vector<bool> m_uncoded;
        
        /// Tracks whether a symbol is partially decoded
        std::vector<bool> m_coded;
        
    };
}

#endif

