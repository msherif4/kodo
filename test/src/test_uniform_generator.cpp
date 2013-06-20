// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file coefficient_generator.hpp Unit tests for the uniform coefficient
///       generators

#include "coefficient_generator_helper.hpp"

namespace kodo
{

    // Uniform generator
    template<class Field>
    class uniform_generator_stack :
        public uniform_generator<
               fake_codec_layer<
               coefficient_info<
               fake_symbol_storage<
               storage_block_info<
               finite_field_info<Field,
               final_coder_factory<
               uniform_generator_stack<Field>
               > > > > > > >
    { };

    template<class Field>
    class uniform_generator_stack_pool :
        public uniform_generator<
               fake_codec_layer<
               coefficient_info<
               fake_symbol_storage<
               storage_block_info<
               finite_field_info<Field,
               final_coder_factory_pool<
               uniform_generator_stack_pool<Field>
               > > > > > > >
    { };

}

/// Run the tests typical coefficients stack
TEST(TestCoefficientGenerator, test_uniform_generator_stack)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // API tests:
    run_test<
        kodo::uniform_generator_stack,
        api_generate>(symbols, symbol_size);

    run_test<
        kodo::uniform_generator_stack_pool,
        api_generate>(symbols, symbol_size);
}

