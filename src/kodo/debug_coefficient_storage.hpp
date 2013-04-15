// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <iostream>

#include <fifi/fifi_utils.hpp>

namespace kodo
{
    /// @ingroup debug
    /// @ingroup coefficient_storage_layers
    ///
    /// @brief Print functions for coefficient storage
    ///
    /// This layer implements useful functions to print stored coefficients
    template<class SuperCoder>
    class debug_coefficient_storage : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

    public:

        /// Prints the decoding matrix to the output stream
        /// @param out The output stream to print to
        void print_coefficients_storage(std::ostream& out)
        {
            for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {
                /// @todo uncomment this if API implements functions to
                ///       access uncoded/coded coefficients
                //if( m_uncoded[i] )
                //{
                //    out << i << " U:\t";
                //}
                //else if( m_coded[i] )
                //{
                //    out << i << " C:\t";
                //}
                //else
                //{
                //    out << i << " ?:\t";
                //}

                print_coefficients(out, i);
            }

            out << std::endl;
        }

        /// Prints a vector of coefficients
        /// @param out The output stream to print to
        /// @param coefficients_index The index of the coefficients vector
        ///                           to print
        void print_coefficients(std::ostream& out, uint32_t coefficients_index)
        {
            value_type* coefficients_i =
                SuperCoder::coefficients_value(coefficients_index);

            for(uint32_t j = 0; j < SuperCoder::symbols(); ++j)
            {
                value_type value =
                    fifi::get_value<field_type>(coefficients_i, j);

                out << (uint32_t)value << " ";
            }

            out << std::endl;
        }

    };

}

