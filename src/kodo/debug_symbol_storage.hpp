// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>
#include <iostream>

#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup debug
    /// @ingroup symbol_storage_layers
    ///
    /// @brief Print functions for symbol storage.
    ///
    /// This layer implements useful functions to print stored symbols.
    template<class SuperCoder>
    class debug_symbol_storage : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

    public:

        /// Prints the symbol matrix to the output stream
        /// @param out The output stream to print to
        void print_symbol_matrix(std::ostream& out)
        {
            uint32_t symbol_elements =
                fifi::length_to_elements<field_type>(
                    SuperCoder::symbol_length());

            print_symbol_matrix(out, symbol_elements);
        }

        /// Prints the symbol matrix to the output stream
        /// @param out The output stream to print to
        /// @param symbol_elements number of elements in a symbol
        void print_symbol_matrix(std::ostream& out, uint32_t symbol_elements)
        {
            assert(symbol_elements <=
                fifi::length_to_elements<field_type>(
                    SuperCoder::symbol_length()));

            for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {
                print_symbol(out, symbol_elements, i);
            }

            out << std::endl;
        }

        /// Print symbol on certain index
        /// @param out The output stream to print to
        /// @param symbol_index index of symbol to print
        void print_symbol(std::ostream& out, uint32_t symbol_index)
        {
            uint32_t symbol_elements =
                fifi::length_to_elements<field_type>(
                    SuperCoder::symbol_length());

            print_symbol(out, symbol_elements, symbol_index);
        }

        /// Print symbol on certain index
        /// @param out The output stream to print to
        /// @param symbol_elements number of elements in a symbol
        /// @param symbol_index index of symbol to print
        void print_symbol(std::ostream& out, uint32_t symbol_elements,
                          uint32_t symbol_index)
        {
            assert(symbol_index < SuperCoder::symbols());

            assert(symbol_elements <=
                fifi::length_to_elements<field_type>(
                    SuperCoder::symbol_length()));

            value_type* symbol =
                SuperCoder::symbol_value(symbol_index);

            for(uint32_t j = 0; j < symbol_elements; ++j)
            {
                value_type value =
                    fifi::get_value<field_type>(symbol, j);

                out << (uint32_t)value << " ";
            }

            out << std::endl;
        }
    };
}
