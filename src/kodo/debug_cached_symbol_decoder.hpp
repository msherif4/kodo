// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>
#include <iostream>
#include <vector>

#include <fifi/fifi_utils.hpp>

#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup debug
    ///
    /// @brief This layer makes it easy to print the incoming coded symbol
    ///        and coding coefficients
    ///
    /// The debug layer supports printing both the coded symbol data through the
    /// print_cached_symbol_data() and the coding coefficients through the
    /// print_cached_symbol_coefficients().
    ///
    /// For the print_cached_symbol_data() function the output will be the
    /// value of the finite field elements stored in the symbol's data.
    ///
    /// For the print_cached_symbol_coefficients() there are two possible
    /// outputs:
    ///
    /// 1) For a coded symbol e.g. "C: 1 0 0" where C denotes that the symbol
    ///    is coded and the following is the numeric values of the coding
    ///    coefficients
    /// 2) For an uncoded symbol e.g. "U: symbol index = 4" in this case
    ///    U denotes that the symbol is uncoded and the symbol index denotes
    ///    which original source symbol the data represents.
    template<class SuperCoder>
    class debug_cached_symbol_decoder : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

    public:

        /// Prints the decoder's state to the output stream
        /// @param out, the output stream
        void print_cached_symbol_data(std::ostream &out) const
        {
            // We calculate the number of finite field elements that are
            // stored in the symbols' data. In this case layer::symbol_length()
            // does not yield the correct result for fields where multiple
            // field elements are packed into the same value_type
            uint32_t symbol_elements =
                fifi::size_to_elements<field_type>(
                    SuperCoder::symbol_size());

            const value_type *s = reinterpret_cast<const value_type*>(
                SuperCoder::cached_symbol_data());

            for(uint32_t j = 0; j < symbol_elements; ++j)
            {
                value_type value = fifi::get_value<field_type>(s, j);

                static_assert(sizeof(uint32_t) >= sizeof(value_type),
                              "value_type will overflow in this print");

                out << (uint32_t) value << " ";
            }

            out << std::endl;

        }

        /// Prints the cached symbol coefficients to the output stream
        /// @param out, the output stream
        void print_cached_symbol_coefficients(std::ostream &out) const
        {
            if(SuperCoder::cached_symbol_coded())
            {
                out << "C:  ";

                const value_type *c = reinterpret_cast<const value_type*>(
                    SuperCoder::cached_symbol_coefficients());

                for(uint32_t j = 0; j < SuperCoder::symbols(); ++j)
                {
                    value_type value = fifi::get_value<field_type>(c, j);

                    static_assert(sizeof(uint32_t) >= sizeof(value_type),
                                  "value_type will overflow in this print");

                    out << (uint32_t) value << " ";

                }
            }
            else
            {
                out << "U:  symbol index = "
                    << SuperCoder::cached_symbol_index() << std::endl;
            }

            out << std::endl;
        }

    };

}

