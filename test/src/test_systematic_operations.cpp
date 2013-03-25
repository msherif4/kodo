// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cstdint>

#include <gtest/gtest.h>

#include <kodo/rlnc/full_vector_codes.hpp>

namespace kodo
{
    // We just create a version of the full rlnc vector code
    // without the systematic layer
    template<class Field>
    class test_nonsystematic_stack
        : public // Payload Codec API
                 payload_encoder<
                 // Codec Header API
                 symbol_id_encoder<
                 // Symbol ID API
                 plain_symbol_id_writer<
                 // Coefficient Generator API
                 uniform_generator<
                 // Codec API
                 zero_symbol_encoder<
                 linear_block_encoder<
                 // Coefficient Storage API
                 coefficient_info<
                 // Symbol Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field Math API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 test_nonsystematic_stack<Field>
                     > > > > > > > > > > > > >
    { };

}

TEST(TestSystematicOperations, is_systematic_encoder)
{
    uint32_t symbols = 10;
    uint32_t symbol_size = 16;

    {
        typedef kodo::test_nonsystematic_stack<fifi::binary>::factory
            encoder_factory;
        typedef kodo::test_nonsystematic_stack<fifi::binary>::pointer
            encoder_pointer;

        encoder_factory factory(symbols, symbol_size);
        encoder_pointer ptr = factory.build(symbols, symbol_size);

        EXPECT_FALSE(kodo::is_systematic_encoder(ptr));
    }

    {
        typedef kodo::full_rlnc_encoder<fifi::binary>::factory encoder_factory;
        typedef kodo::full_rlnc_encoder<fifi::binary>::pointer encoder_pointer;

        encoder_factory factory(symbols, symbol_size);
        encoder_pointer ptr = factory.build(symbols, symbol_size);

        EXPECT_TRUE(kodo::is_systematic_encoder(ptr));
        EXPECT_TRUE(kodo::is_systematic_on(ptr));

        kodo::set_systematic_off(ptr);

        EXPECT_TRUE(kodo::is_systematic_encoder(ptr));
        EXPECT_FALSE(kodo::is_systematic_on(ptr));

        kodo::set_systematic_on(ptr);

        EXPECT_TRUE(kodo::is_systematic_encoder(ptr));
        EXPECT_TRUE(kodo::is_systematic_on(ptr));
    }
}

