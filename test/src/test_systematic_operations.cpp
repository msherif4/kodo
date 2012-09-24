// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <stdint.h>

#include <gtest/gtest.h>

#include <kodo/rlnc/full_vector_codes.hpp>

namespace kodo
{
    // We just create a version of the full rlnc vector code
    // without the systematic layer
    template<class Field>
    class full_rlnc_encoder_no_system
        : public payload_encoder<
                 zero_symbol_encoder<
                 full_vector_encoder<
                 linear_block_vector_generator<block_uniform_no_position,
                 linear_block_encoder<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_shallow_partial<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<full_rlnc_encoder_no_system<Field>, Field>
                     > > > > > > > > >
    {};

}

TEST(TestSystematicOperations, is_systematic_encoder)
{
    uint32_t symbols = 10;
    uint32_t symbol_size = 16;

    {
        typedef kodo::full_rlnc_encoder_no_system<fifi::binary>::factory
            encoder_factory;
        typedef kodo::full_rlnc_encoder_no_system<fifi::binary>::pointer
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

