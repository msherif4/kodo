// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_debug_xyz.cpp Unit tests for the debug layers

#include <cstdint>

#include <gtest/gtest.h>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/debug_linear_block_decoder.hpp>

#include "helper_test_basic_api.hpp"

/// Here we define the stacks which should be tested.
namespace kodo
{
    /// A stack containing the debug layer just to ensure
    /// that is compiles
    template<class Field>
    class debug_decoder
        : public // Payload API
                 payload_recoder<recoding_stack,
                 payload_decoder<
                 // Codec Header API
                 systematic_decoder<
                 symbol_id_decoder<
                 // Symbol ID API
                 plain_symbol_id_reader<
                 // Codec API
                 debug_linear_block_decoder<
                 aligned_coefficients_decoder<
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 debug_decoder<Field>
                     > > > > > > > > > > > > > > > >
    { };

}

/// Run the tests typical coefficients stack
TEST(TestDebugXYZ, test_debug_linear_block_decoder)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    test_basic_api
    <
        kodo::full_rlnc_encoder<fifi::binary>,
        kodo::debug_decoder<fifi::binary>
    >(symbols, symbol_size);

}




