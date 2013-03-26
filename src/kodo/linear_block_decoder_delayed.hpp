// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_LINEAR_BLOCK_DECODER_DELAYED_HPP
#define KODO_LINEAR_BLOCK_DECODER_DELAYED_HPP

#include <cstdint>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

namespace kodo
{

    /// @ingroup codec_layers
    /// @brief Linear block decoder with delayer backwards substitution.
    ///
    /// The delayed backwards substitution can reduce the fill-in
    /// effect and can therefore improve the decoding throughput when
    /// decoding sparse symbols, in particular if the generation size
    /// is large.
    template<class SuperCoder>
    class linear_block_decoder_delayed : public SuperCoder
    {
    public:

        /// The field we use
        typedef typename SuperCoder::field_type field_type;

        /// The value_type used to store the field elements
        typedef typename field_type::value_type value_type;

    public:

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data, uint8_t *coefficients)
            {
                assert(symbol_data != 0);
                assert(coefficients != 0);

                value_type *s =
                    reinterpret_cast<value_type*>(symbol_data);

                value_type *c =
                    reinterpret_cast<value_type*>(coefficients);

                decode_coefficients(s, c);
            }

        /// @copydoc layer::decode_symbol(uint8_t*,uint32_t)
        void decode_symbol(uint8_t *symbol_data, uint32_t symbol_index)
            {
                assert(symbol_index < SuperCoder::symbols());
                assert(symbol_data != 0);

                if(m_uncoded[symbol_index])
                    return;

                const value_type *symbol
                    = reinterpret_cast<const value_type*>( symbol_data );

                if(m_coded[symbol_index])
                {
                    SuperCoder::swap_decode(symbol, symbol_index);
                }
                else
                {
                    // Stores the symbol and updates the corresponding
                    // encoding vector
                    SuperCoder::store_uncoded_symbol(symbol, symbol_index);

                    // We have increased the rank
                    ++m_rank;

                    m_uncoded[ symbol_index ] = true;

                    if(symbol_index > m_maximum_pivot)
                    {
                        m_maximum_pivot = symbol_index;
                    }

                }

                if(SuperCoder::is_complete())
                {
                    final_backward_substitute();
                }

            }

    protected:

        // Fetch the variables needed
        using SuperCoder::m_rank;
        using SuperCoder::m_maximum_pivot;
        using SuperCoder::m_coded;
        using SuperCoder::m_uncoded;

    protected:

        /// Performs the forward substitution, but waits with the final
        /// backwards substitution until full rank is achieved.
        /// @param symbol_data The buffer of the encoded symbol
        /// @param coefficients The coding coefficients used to encode the
        ///        symbol
        void decode_coefficients(value_type *symbol_data,
                                 value_type *coefficients)
            {
                assert(symbol_data != 0);
                assert(coefficients != 0);

                // See if we can find a pivot
                boost::optional<uint32_t> pivot_index
                    = SuperCoder::forward_substitute_to_pivot(
                        symbol_data, coefficients);

                if(!pivot_index)
                    return;

                if(!fifi::is_binary<field_type>::value)
                {
                    // Normalize symbol and vector
                    SuperCoder::normalize(
                        symbol_data, coefficients, *pivot_index);
                }

                // Now save the received symbol
                SuperCoder::store_coded_symbol(
                    symbol_data, coefficients,*pivot_index);

                // We have increased the rank
                ++m_rank;

                m_coded[ *pivot_index ] = true;

                if(*pivot_index > m_maximum_pivot)
                {
                    m_maximum_pivot = *pivot_index;
                }

                if(SuperCoder::is_complete())
                {
                    final_backward_substitute();
                }
            }

    protected:

        /// Performs the final backward substitution that transform the
        /// coding matrix from echelon form to reduce echelon form and
        /// hence fully decode the generation
        void final_backward_substitute()
            {
                assert(SuperCoder::is_complete());

                uint32_t symbols = SuperCoder::symbols();

                for(uint32_t i = symbols; i --> 0;)
                {
                    value_type *symbol_i =
                        SuperCoder::symbol_value(i);

                    value_type *vector_i =
                        SuperCoder::coefficients_value(i);

                    SuperCoder::backward_substitute(
                        symbol_i, vector_i, i);
                }
            }
    };
}

#endif

