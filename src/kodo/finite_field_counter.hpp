// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include "operations_counter.hpp"

namespace kodo
{

    /// @ingroup debug
    /// This layer "intercepts" all calls to the finite_field_math
    /// layer counting the different operations
    template<class SuperCoder>
    class finite_field_counter : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// @copydoc layer::factory
        typedef typename SuperCoder::factory factory;

    public:

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            SuperCoder::initialize(the_factory);

            // Reset the counter
            m_counter = operations_counter();
        }

        /// @copydoc layer::multiply(value_type*,value_type,uint32_t)
        void multiply(value_type *symbol_dest, value_type coefficient,
                      uint32_t symbol_length)
        {
            ++m_counter.m_multiply;
            SuperCoder::multiply(symbol_dest, coefficient, symbol_length);
        }

        /// @copydoc layer::multipy_add(value_type *, const value_type*,
        ///                             value_type, uint32_t)
        void multiply_add(value_type *symbol_dest,
                          const value_type *symbol_src,
                          value_type coefficient, uint32_t symbol_length)
        {
            ++m_counter.m_multiply_add;
            SuperCoder::multiply_add(symbol_dest, symbol_src,
                                     coefficient,
                                     symbol_length);
        }

        /// @copydoc layer::add(value_type*, const value_type *, uint32_t)
        void add(value_type *symbol_dest, const value_type *symbol_src,
                 uint32_t symbol_length)
        {
            ++m_counter.m_add;
            SuperCoder::add(symbol_dest, symbol_src, symbol_length);
        }

        /// @copydoc layer::multiply_subtract(
        ///              value_type*, const value_type*,
        ///              value_type, uint32_t)
        void multiply_subtract(value_type *symbol_dest,
                               const value_type *symbol_src,
                               value_type coefficient,
                               uint32_t symbol_length)
        {
            ++m_counter.m_multiply_subtract;
            SuperCoder::multiply_subtract(symbol_dest, symbol_src,
                                          coefficient, symbol_length);
        }

        /// @copydoc layer::subtract(
        ///              value_type*,const value_type*, uint32_t)
        void subtract(value_type *symbol_dest, const value_type *symbol_src,
                      uint32_t symbol_length)
        {
            ++m_counter.m_subtract;
            SuperCoder::subtract(symbol_dest, symbol_src,
                                 symbol_length);
        }

        /// @copydoc layer::invert(value_type)
        value_type invert(value_type value)
        {
            ++m_counter.m_invert;
            return SuperCoder::invert(value);
        }

        /// @return The operation counter
        operations_counter get_operations_counter() const
        {
            return m_counter;
        }

        /// Reset the operation counter
        void reset_operations_counter()
        {
            m_counter = operations_counter();
        }

    private:

        /// Operations counter
        operations_counter m_counter;

    };

}

