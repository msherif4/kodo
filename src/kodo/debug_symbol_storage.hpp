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
    /// @brief Print functions for symbol storage. Using this layer
    ///        the content of the symbol storage can be printed.
    ///
    /// This layer provides two different ways of printing the symbol
    /// data:
    /// 1) Using the print_storage() and print_symbol() functions, the
    ///    data will be printed using the numeric value of the bytes stored
    /// 2) Using the print_storage_value() and print_symbol_value() functions,
    ///    the data will be printed using the numeric value of the finite field
    ///    used
    ///
    /// 0 I:  23 35 222 35 0 10
    /// 1 A:  33 22 19 01 10 15
    /// 2 ?:  -
    ///
    /// The lines should be interpret as in the following example:
    ///
    ///   1 A:  33 22 19 01 10 15
    ///   ^ ^   ^
    ///   | |   |
    ///   | |   |
    ///   | |   +--+  The values of the symbol storage (format depends on
    ///   | |          the print function used either bytes or finite field)
    ///   | +------+  The symbol state
    ///   +--------+  The symbol's pivot position
    ///
    /// A symbol's storage can be in 3 states:
    ///
    /// A: The storage is available, but has not necessarily been initialized.
    /// I: The storage is available and has been explicitly been initialized
    ///    by the user through a set_symbol() or set_symbols() call.
    /// ?: No storage is available for this symbol.
    ///
    template<class SuperCoder>
    class debug_symbol_storage : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

    public:

        /// Print all symbols stored in the storage where the symbol data
        /// is shown in bytes.
        /// @param out The output stream to print to
        void print_storage(std::ostream& out) const
        {
            for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {
                print_symbol(out, i);
            }
        }

        /// Print all symbols stored in the storage where the symbol data
        /// is shown in bytes.
        /// @param out The output stream to print to
        void print_storage_value(std::ostream& out) const
        {
            for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {
                print_symbol_value(out, i);
            }
        }

        /// Print the bytes of a symbol at a specific index to the
        /// output stream
        /// @param out The output stream to print to
        /// @param index The index of symbol to print
        void print_symbol(std::ostream& out, uint32_t index) const
        {
            assert(index < SuperCoder::symbols());

            print_symbol_info(out, index);

            if(SuperCoder::is_symbol_available(index))
            {
                const uint8_t* symbol = SuperCoder::symbol(index);

                for(uint32_t j = 0; j < SuperCoder::symbol_size(); ++j)
                {
                    out << (uint32_t) symbol[j] << " ";
                }
            }

            out << std::endl;
        }

        /// Print finite field elements of a specific symbol's data to the
        /// output stream
        /// @param out The output stream to print to
        /// @param index The index of symbol to print
        void print_symbol_value(std::ostream& out, uint32_t index) const
        {
            assert(index < SuperCoder::symbols());

            print_symbol_info(out, index);

            if(SuperCoder::is_symbol_available(index))
            {

                // We calculate the number of finite field elements that are
                // stored in the symbols' data. In this case
                // layer::symbol_length() does not yield the correct result
                // for fields where multiple field elements are packed into
                // the same value_type
                uint32_t symbol_elements =
                    fifi::size_to_elements<field_type>(
                        SuperCoder::symbol_size());

                const value_type* symbol = SuperCoder::symbol_value(index);

                for(uint32_t j = 0; j < symbol_elements; ++j)
                {
                    value_type value = fifi::get_value<field_type>(symbol, j);

                    static_assert(sizeof(uint32_t) >= sizeof(value_type),
                                  "value_type will overflow in this print");

                    out << (uint32_t) value << " ";
                }
            }

            out << std::endl;
        }

    protected:

        /// Print the bytes of a symbol at a specific index to the
        /// output stream
        /// @param out The output stream to print to
        /// @param index The index of symbol to print
        void print_symbol_info(std::ostream& out, uint32_t index) const
        {
            assert(index < SuperCoder::symbols());

            if(SuperCoder::is_symbol_available(index))
            {
                if(SuperCoder::is_symbol_initialized(index))
                {
                    out << index << " I:  ";
                }
                else
                {
                    out << index << " A:  ";
                }

            }
            else
            {
                out << index << " ?:  -";
            }
        }


    };
}
