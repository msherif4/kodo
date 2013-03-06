// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_DEBUG_LINEAR_BLOCK_DECODER_HPP
#define KODO_DEBUG_LINEAR_BLOCK_DECODER_HPP

#include <cstdint>

#include <fifi/fifi_utils.hpp>

namespace kodo
{
    // Debug layer which allows printing encoding vectors
    // of a RLNC decoder.
    template<class SuperCoder>
    class debug_linear_block_decoder : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

        /// Access to status about uncoded symbols in the linear block
        /// decoder.
        using SuperCoder::m_uncoded;

        /// Access to status about the coded symbols in the linear block
        /// decoder.
        using SuperCoder::m_coded;

    public:

        /// Prints the decoding matrix to the output stream
        /// @param out The output stream to print to
        void print_decoding_matrix(std::ostream &out)
            {
                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    if( m_uncoded[i] )
                    {
                        out << i << " U:\t";
                    }
                    else if( m_coded[i] )
                    {
                        out << i << " C:\t";
                    }
                    else
                    {
                        out << i << " ?:\t";
                    }

                    value_type *coefficients_i =
                        SuperCoder::coefficients_value(i);

                    for(uint32_t j = 0; j < SuperCoder::symbols(); ++j)
                    {

                        value_type value =
                            fifi::get_value<field_type>(coefficients_i, j );

                        out << (uint32_t)value << " ";
                    }

                    out << std::endl;
                }

                out << std::endl;
            }
    };
}

#endif

