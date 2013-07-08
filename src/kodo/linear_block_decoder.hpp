// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include <sak/storage.hpp>

#include <fifi/is_binary.hpp>
#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup codec_layers
    /// @brief Implements basic linear block decoder.
    ///
    /// The linear block decoder
    /// expects that an encoded symbol is described by a vector of
    /// coefficients. Using these coefficients the block decoder subtracts
    /// incoming symbols until the original data has been recreated.
    template<class SuperCoder>
    class linear_block_decoder : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

        /// @copydoc layer::factory
        typedef typename SuperCoder::factory factory;

    public:

        /// Constructor
        linear_block_decoder()
            : m_rank(0),
              m_maximum_pivot(0)
        { }

        /// @copydoc layer::construct(Factory&)
        template<class Factory>
        void construct(Factory &the_factory)
        {
            SuperCoder::construct(the_factory);

            m_uncoded.resize(the_factory.max_symbols(), false);
            m_coded.resize(the_factory.max_symbols(), false);
        }

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            SuperCoder::initialize(the_factory);

            std::fill_n(m_uncoded.begin(), the_factory.symbols(), false);
            std::fill_n(m_coded.begin(), the_factory.symbols(), false);

            m_rank = 0;
            m_maximum_pivot = 0;
        }

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data,
                           uint8_t *symbol_coefficients)
        {
            assert(symbol_data != 0);
            assert(symbol_coefficients != 0);

            value_type *symbol
                = reinterpret_cast<value_type*>(symbol_data);

            value_type *coefficients
                = reinterpret_cast<value_type*>(symbol_coefficients);

            decode_coefficients(symbol, coefficients);
        }

        /// @copydoc layer::decode_symbol(uint8_t*, uint32_t)
        void decode_symbol(uint8_t *symbol_data,
                           uint32_t symbol_index)
        {
            assert(symbol_index < SuperCoder::symbols());
            assert(symbol_data != 0);

            if(m_uncoded[symbol_index])
            {
                return;
            }

            const value_type *symbol
                = reinterpret_cast<value_type*>( symbol_data );

            if(m_coded[symbol_index])
            {
                swap_decode(symbol, symbol_index);
            }
            else
            {
                // Stores the symbol and updates the corresponding
                // encoding vector
                store_uncoded_symbol(symbol, symbol_index);

                // Backwards substitution
                value_type *coefficients =
                    SuperCoder::coefficients_value(symbol_index);

                backward_substitute(symbol, coefficients, symbol_index);

                // We have increased the rank if we have finished the
                // backwards substitution
                ++m_rank;

                m_uncoded[ symbol_index ] = true;

                if(symbol_index > m_maximum_pivot)
                {
                    m_maximum_pivot = symbol_index;
                }

            }
        }

        /// @copydoc layer::is_complete() const
        bool is_complete() const
        {
            return m_rank == SuperCoder::symbols();
        }

        /// @copydoc layer::rank() const
        uint32_t rank() const
        {
            return m_rank;
        }

        /// @copydoc layer::symbol_pivot(uint32_t) const
        bool symbol_pivot(uint32_t index) const
        {
            assert(index < SuperCoder::symbols());
            return m_coded[index] || m_uncoded[index];
        }

        /// @todo Add unit test
        /// @copydoc layer::symbol_pivot(uint32_t) const
        bool symbol_coded(uint32_t index) const
        {
            assert(symbol_pivot(index));
            return m_coded[index];
        }

    protected:

        /// Decodes a symbol based on the coefficients
        /// @param symbol_data buffer containing the encoding symbol
        /// @param symbol_id buffer containing the encoding vector
        void decode_coefficients(value_type *symbol_data,
                                 value_type *symbol_coefficients)
        {
            assert(symbol_data != 0);
            assert(symbol_coefficients != 0);

            // See if we can find a pivot
            auto pivot_index
                = forward_substitute_to_pivot(
                    symbol_data, symbol_coefficients);

            if(!pivot_index)
                return;

            if(!fifi::is_binary<field_type>::value)
            {
                // Normalize symbol and vector
                normalize(
                    symbol_data,symbol_coefficients,*pivot_index);
            }

            // Reduce the symbol further
            forward_substitute_from_pivot(
                symbol_data, symbol_coefficients, *pivot_index);

            // Now with the found pivot reduce the existing symbols
            backward_substitute(
                symbol_data, symbol_coefficients, *pivot_index);

            // Now save the received symbol
            store_coded_symbol(
                symbol_data, symbol_coefficients, *pivot_index);

            // We have increased the rank
            ++m_rank;

            m_coded[ *pivot_index ] = true;

            if(*pivot_index > m_maximum_pivot)
            {
                m_maximum_pivot = *pivot_index;
            }
        }

        /// When adding a raw symbol (i.e. uncoded) with a specific
        /// pivot id and the decoder already contains a coded symbol
        /// in that position this function performs the proper swap
        /// between the two symbols.
        /// @param symbol_data the data for the raw symbol
        /// @param pivot_index the pivot position of the raw symbol
        void swap_decode(const value_type *symbol_data,
                         uint32_t pivot_index)
        {
            assert(m_coded[pivot_index] == true);
            assert(m_uncoded[pivot_index] == false);

            m_coded[pivot_index] = false;

            value_type *symbol_i =
                SuperCoder::symbol_value(pivot_index);

            value_type *vector_i =
                SuperCoder::coefficients_value(pivot_index);

            value_type value =
                fifi::get_value<field_type>(vector_i, pivot_index);

            assert(value == 1);

            // Subtract the new pivot symbol
            fifi::set_value<field_type>(vector_i, pivot_index, 0);

            SuperCoder::subtract(symbol_i, symbol_data,
                                 SuperCoder::symbol_length());

            // Now continue our new coded symbol we know that it must
            // if found it will contain a pivot id > that the current.
            decode_coefficients(symbol_i, vector_i);

            // The previous vector may still be in memory
            std::fill_n(vector_i, SuperCoder::coefficients_length(), 0);

            // Stores the symbol and sets the pivot in the vector
            store_uncoded_symbol(symbol_data, pivot_index);

            m_uncoded[pivot_index] = true;

            // No need to backwards substitute since we are
            // replacing an existing symbol. I.e. backwards
            // substitution must already have been done.
        }

        /// Iterates the encoding vector from where a pivot has been
        /// identified and subtracts existing symbols
        /// @param symbol_data the data of the encoded symbol
        /// @param symbol_id the data constituting the encoding vector
        /// @param pivot_index the index of the found pivot element
        void normalize(value_type *symbol_data,
                       value_type *symbol_id,
                       uint32_t pivot_index)
        {

            assert(symbol_id != 0);
            assert(symbol_data != 0);

            assert(pivot_index < SuperCoder::symbols());

            assert(m_uncoded[pivot_index] == false);
            assert(m_coded[pivot_index] == false);

            value_type coefficient =
                fifi::get_value<field_type>(symbol_id, pivot_index);

            assert(coefficient > 0);

            value_type inverted_coefficient =
                SuperCoder::invert(coefficient);

            // Update symbol and corresponding vector
            SuperCoder::multiply(symbol_id, inverted_coefficient,
                                 SuperCoder::coefficients_length());

            SuperCoder::multiply(symbol_data, inverted_coefficient,
                                 SuperCoder::symbol_length());

        }

        /// Iterates the encoding vector and subtracts existing symbols
        /// until a pivot element is found.
        /// @param symbol_data the data of the encoded symbol
        /// @param symbol_id the data constituting the encoding vector
        /// @return the pivot index if found.
        boost::optional<uint32_t> forward_substitute_to_pivot(
            value_type *symbol_data,
            value_type *symbol_id)
        {
            assert(symbol_id != 0);
            assert(symbol_data != 0);

            for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {

                value_type current_coefficient
                    = fifi::get_value<field_type>(symbol_id, i);

                if( current_coefficient )
                {
                    // If symbol exists
                    if( symbol_pivot( i ) )
                    {
                        value_type *vector_i =
                            SuperCoder::coefficients_value( i );

                        value_type *symbol_i =
                            SuperCoder::symbol_value( i );

                        if(fifi::is_binary<field_type>::value)
                        {
                            SuperCoder::subtract(
                                symbol_id, vector_i,
                                SuperCoder::coefficients_length());

                            SuperCoder::subtract(
                                symbol_data, symbol_i,
                                SuperCoder::symbol_length());
                        }
                        else
                        {
                            SuperCoder::multiply_subtract(
                                symbol_id, vector_i,
                                current_coefficient,
                                SuperCoder::coefficients_length());

                            SuperCoder::multiply_subtract(
                                symbol_data, symbol_i,
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

        /// Iterates the encoding vector from where a pivot has been
        /// identified and subtracts existing symbols
        /// @param symbol_data the data of the encoded symbol
        /// @param symbol_id the data constituting the encoding vector
        /// @param pivot_index the index of the found pivot element
        void forward_substitute_from_pivot(value_type *symbol_data,
                                           value_type *symbol_id,
                                           uint32_t pivot_index)
        {
            // We have received an encoded symbol - described
            // by the symbol group. We now normalize the
            // the encoding vector according to the symbol id.
            // I.e. we make sure the pivot position has a "1"
            assert(symbol_id != 0);
            assert(symbol_data != 0);

            assert(pivot_index < SuperCoder::symbols());

            assert(m_uncoded[pivot_index] == false);
            assert(m_coded[pivot_index] == false);

            // If this pivot index was smaller than the maximum pivot
            // index we have, we might also need to backward
            // substitute the higher pivot values into the new packet
            for(uint32_t i = pivot_index + 1; i <= m_maximum_pivot; ++i)
            {
                // Do we have a non-zero value here?

                value_type value =
                    fifi::get_value<field_type>(symbol_id, i);


                if( !value )
                {
                    continue;
                }

                if( symbol_pivot(i) )
                {
                    value_type *vector_i =
                        SuperCoder::coefficients_value(i);

                    value_type *symbol_i =
                        SuperCoder::symbol_value(i);

                    if(fifi::is_binary<field_type>::value)
                    {
                        SuperCoder::subtract(
                            symbol_id, vector_i,
                            SuperCoder::coefficients_length());

                        SuperCoder::subtract(
                            symbol_data, symbol_i,
                            SuperCoder::symbol_length());
                    }
                    else
                    {
                        SuperCoder::multiply_subtract(
                            symbol_id, vector_i, value,
                            SuperCoder::coefficients_length());

                        SuperCoder::multiply_subtract(
                            symbol_data, symbol_i, value,
                            SuperCoder::symbol_length());
                    }
                }
            }
        }

        /// Backward substitute the found symbol into the
        /// existing symbols.
        /// @param symbol_data buffer containing the encoding symbol
        /// @param symbol_id buffer containing the encoding vector
        /// @param pivot_index the pivot index of the symbol in the
        ///        buffers symbol_id and symbol_data
        void backward_substitute(const value_type *symbol_data,
                                 const value_type *symbol_id,
                                 uint32_t pivot_index)
        {
            assert(symbol_id != 0);
            assert(symbol_data != 0);

            assert(pivot_index < SuperCoder::symbols());

            // We found a "1" that nobody else had as pivot, we now
            // substract this packet from other coded packets
            // - if they have a "1" on our pivot place
            for(uint32_t i = 0; i <= m_maximum_pivot; ++i)
            {
                if( m_uncoded[i] )
                {
                    // We know that we have no non-zero elements
                    // outside the pivot position.
                    continue;
                }

                if(i == pivot_index)
                {
                    // We cannot backward substitute into ourself
                    continue;
                }

                if( m_coded[i] )
                {
                    value_type *vector_i =
                        SuperCoder::coefficients_value(i);

                    value_type value =
                        fifi::get_value<field_type>(
                            vector_i, pivot_index);

                    if( value )
                    {

                        value_type *symbol_i =
                            SuperCoder::symbol_value(i);

                        if(fifi::is_binary<field_type>::value)
                        {
                            SuperCoder::subtract(
                                vector_i, symbol_id,
                                SuperCoder::coefficients_length());

                            SuperCoder::subtract(
                                symbol_i, symbol_data,
                                SuperCoder::symbol_length());
                        }
                        else
                        {

                            // Update symbol and corresponding vector
                            SuperCoder::multiply_subtract(
                                vector_i, symbol_id, value,
                                SuperCoder::coefficients_length());

                            SuperCoder::multiply_subtract(
                                symbol_i, symbol_data, value,
                                SuperCoder::symbol_length());
                        }
                    }
                }
            }
        }

        /// Store an encoded symbol and encoding vector with the specified
        /// pivot found.
        /// @param symbol_data buffer containing the encoding symbol
        /// @param symbol_coefficients buffer containing the symbol
        ///        coefficients
        /// @param pivot_index the pivot index
        void store_coded_symbol(const value_type *symbol_data,
                                const value_type *symbol_coefficients,
                                uint32_t pivot_index)
        {
            assert(m_uncoded[pivot_index] == false);
            assert(m_coded[pivot_index] == false);
            assert(symbol_coefficients != 0);
            assert(symbol_data != 0);
            assert(SuperCoder::is_symbol_available(pivot_index));

            auto coefficient_storage =
                sak::storage(symbol_coefficients,
                             SuperCoder::coefficients_size());

            SuperCoder::set_coefficients(
                pivot_index, coefficient_storage);

            // Copy it into the symbol storage
            sak::mutable_storage dest =
                sak::storage(SuperCoder::symbol(pivot_index),
                             SuperCoder::symbol_size());

            sak::const_storage src =
                sak::storage(symbol_data, SuperCoder::symbol_size());

            sak::copy_storage(dest, src);
        }

        /// Stores an uncoded or fully decoded symbol
        /// @param symbol_data the data for the symbol
        /// @param pivot_index the pivot index of the symbol
        void store_uncoded_symbol(const value_type *symbol_data,
                                  uint32_t pivot_index)
        {
            assert(symbol_data != 0);
            assert(m_uncoded[pivot_index] == false);
            assert(m_coded[pivot_index] == false);
            assert(SuperCoder::is_symbol_available(pivot_index));

            // Update the corresponding vector
            value_type *vector_dest =
                SuperCoder::coefficients_value( pivot_index );

            // Zero out the memory first
            std::fill_n(vector_dest, SuperCoder::coefficients_length(), 0);

            fifi::set_value<field_type>(vector_dest, pivot_index, 1U);

            // Copy it into the symbol storage
            sak::mutable_storage dest =
                sak::storage(SuperCoder::symbol(pivot_index),
                             SuperCoder::symbol_size());

            sak::const_storage src =
                sak::storage(symbol_data, SuperCoder::symbol_size());

            sak::copy_storage(dest, src);

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

