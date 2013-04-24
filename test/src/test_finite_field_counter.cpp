// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_finite_field_counter.cpp Unit tests for the
///       kodo::finite_field_counter class

#include <cstdint>

#include <gtest/gtest.h>

#include <fifi/field_types.hpp>

#include <kodo/operations_counter.hpp>
#include <kodo/finite_field_counter.hpp>

#include "operations_counter_helper.hpp"

namespace kodo
{

    // Dummy class to provide needed API
    template<class Field>
    class dummy_finite_field
    {
    public:

        /// @copydoc layer::field_type
        typedef Field field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

    public:

        /// Dummy factory
        struct factory
        {};

    public:

        /// @copydoc layer::initialize(factory&)
        void initialize(factory& the_factory)
        {
            (void) the_factory;
        }

        /// @copydoc layer::multiply(value_type*,value_type,uint32_t)
        void multiply(value_type *symbol_dest, value_type coefficient,
                      uint32_t symbol_length)
        {
            (void) symbol_dest;
            (void) coefficient;
            (void) symbol_length;
        }

        /// @copydoc layer::multipy_add(value_type *, const value_type*,
        ///                             value_type, uint32_t)
        void multiply_add(value_type *symbol_dest,
                          const value_type *symbol_src,
                          value_type coefficient, uint32_t symbol_length)
        {
            (void) symbol_dest;
            (void) symbol_src;
            (void) coefficient;
            (void) symbol_length;
        }

        /// @copydoc layer::add(value_type*, const value_type *, uint32_t)
        void add(value_type *symbol_dest, const value_type *symbol_src,
                 uint32_t symbol_length)
        {
            (void) symbol_dest;
            (void) symbol_src;
            (void) symbol_length;
        }

        /// @copydoc layer::multiply_subtract(
        ///              value_type*, const value_type*,
        ///              value_type, uint32_t)
        void multiply_subtract(value_type *symbol_dest,
                               const value_type *symbol_src,
                               value_type coefficient,
                               uint32_t symbol_length)
        {
            (void) symbol_dest;
            (void) symbol_src;
            (void) coefficient;
            (void) symbol_length;
        }

        /// @copydoc layer::subtract(
        ///              value_type*,const value_type*, uint32_t)
        void subtract(value_type *symbol_dest, const value_type *symbol_src,
                      uint32_t symbol_length)
        {
            (void) symbol_dest;
            (void) symbol_src;
            (void) symbol_length;
        }

        /// @copydoc layer::invert(value_type)
        value_type invert(value_type value)
        {
            return value;
        }

    };

    /// Dummy stack including the finite field counter
    template<class Field>
    class counter_test_stack :
        public finite_field_counter<
               dummy_finite_field<Field> >
    { };

}

/// Run the tests for the finite field counter
TEST(TestFiniteFieldCounter, construct)
{
    kodo::counter_test_stack<fifi::binary> stack;

    kodo::operations_counter counter =
        stack.get_operations_counter();

    test_values(counter, 0U);
}

/// Helper function invoking the member functions of the
/// finite field stack
template<class Stack>
void invoke_counters(Stack &stack)
{
    typename Stack::value_type *dummy_ptr = 0;
    typename Stack::value_type dummy_coefficient = 0;
    uint32_t dummy_length = 0;

    stack.multiply(dummy_ptr, dummy_coefficient, dummy_length);
    stack.multiply_add(dummy_ptr, dummy_ptr,
                       dummy_coefficient, dummy_length);

    stack.add(dummy_ptr, dummy_ptr, dummy_length);

    stack.multiply_subtract(dummy_ptr, dummy_ptr,
                            dummy_coefficient, dummy_length);

    stack.subtract(dummy_ptr, dummy_ptr, dummy_length);

    dummy_coefficient = stack.invert(dummy_coefficient);
}

/// Run the tests for the finite field counter
TEST(TestFiniteFieldCounter, invoke_counters)
{
    kodo::counter_test_stack<fifi::binary> stack;

    invoke_counters(stack);

    auto counter = stack.get_operations_counter();

    test_values(counter, 1U);

    stack.reset_operations_counter();

    counter = stack.get_operations_counter();
    test_values(counter, 0U);

    invoke_counters(stack);
    invoke_counters(stack);
    invoke_counters(stack);

    counter = stack.get_operations_counter();

    test_values(counter, 3U);

    kodo::counter_test_stack<fifi::binary>::factory f;
    stack.initialize(f);

    counter = stack.get_operations_counter();

    test_values(counter, 0U);
}



