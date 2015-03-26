// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_coefficient_info.cpp Unit tests for the
///       coefficient_info class

#include <cstdint>
#include <sstream>

#include <gtest/gtest.h>
#include <fifi/field_types.hpp>
#include <kodo/coefficient_info.hpp>
#include <kodo/coefficient_storage.hpp>
#include <kodo/debug_coefficient_storage.hpp>
#include <kodo/storage_block_info.hpp>
namespace kodo
{
    /// Helper class to test the coefficient info API.
    template<class FieldType>
    class dummy_layer
    {
    public:

        typedef FieldType field_type;
        typedef uint8_t pointer;
    
    public:

        class factory
        {
        public:

            factory(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                (void)max_symbols;
                (void)max_symbol_size;
            }

        };

        template<class Factory>
        void initialize(Factory& the_factory)
        {
            (void)the_factory;
        }

        template<class Factory>
        void construct(Factory& the_factory)
        {
            (void)the_factory;
        }
    };

    template<class FieldType>
    class test_debug_coefficient_storage :
        public debug_coefficient_storage<
               coefficient_storage<
               coefficient_info<
               storage_block_info<
               dummy_layer<
               FieldType> > > > >
    { };
}

TEST(TestDebugCoefficientStorage, api)
{
    uint32_t symbols = 3;
    uint32_t symbols_size = 16;
    {
        typedef kodo::test_debug_coefficient_storage<fifi::binary> stack;

        stack::factory f(symbols, symbols_size);

        stack debug;
        debug.construct(f);
        debug.initialize(f);

        stack::value_type* c = debug.coefficients_value(0);
        fifi::set_value<stack::field_type>(c, 0, 1);
        c = debug.coefficients_value(1);
        fifi::set_value<stack::field_type>(c, 1, 1);
        c = debug.coefficients_value(2);
        fifi::set_value<stack::field_type>(c, 2, 1);

        std::stringstream output;
        debug.print_coefficients_value(output);
        EXPECT_EQ(
            "\t0\t1\t2\t\n0:\t1\t0\t0\t\n1:\t0\t1\t0\t\n2:\t0\t0\t1\t\n",
            output.str());
    }

    {
        typedef kodo::test_debug_coefficient_storage<fifi::binary8> stack;

        stack::factory f(symbols, symbols_size);

        stack debug;
        debug.construct(f);
        debug.initialize(f);

        stack::value_type* c = debug.coefficients_value(0);
        fifi::set_value<stack::field_type>(c, 0, 255);
        c = debug.coefficients_value(1);
        fifi::set_value<stack::field_type>(c, 1, 255);
        c = debug.coefficients_value(2);
        fifi::set_value<stack::field_type>(c, 2, 255);

        std::stringstream output;
        debug.print_coefficients_value(output);
        EXPECT_EQ(
            "\t0\t1\t2\t\n0:\t255\t0\t0\t\n1:\t0\t255\t0\t\n2:\t0\t0\t255\t\n",
            output.str());
    }

    {
        typedef kodo::test_debug_coefficient_storage<fifi::binary32> stack;

        stack::factory f(symbols, symbols_size);

        stack debug;
        debug.construct(f);
        debug.initialize(f);

        stack::value_type* c = debug.coefficients_value(0);
        fifi::set_value<stack::field_type>(c, 0, 13371337);
        c = debug.coefficients_value(1);
        fifi::set_value<stack::field_type>(c, 1, 13371337);
        c = debug.coefficients_value(2);
        fifi::set_value<stack::field_type>(c, 2, 13371337);

        std::stringstream output;
        debug.print_coefficients_value(output);
        EXPECT_EQ(
            "\t0\t1\t2\t\n0:\t13371337\t0\t0\t\n1:\t0\t13371337\t0\t\n2:\t0\t0\t13371337\t\n",
            output.str());
    }
}
