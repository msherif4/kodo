// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <stdint.h>

#include <gtest/gtest.h>

#include <kodo/file_encoder.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>

#include <boost/filesystem.hpp>


namespace kodo
{
        /// @copydoc full_vector_encoder
    template<class Field>
    class test_rlnc_encoder
        : public payload_encoder<
                 systematic_encoder<
                 zero_symbol_encoder<
                 full_vector_encoder<
                 linear_block_vector_generator<block_cache_lookup_uniform,
                 linear_block_encoder<
                 finite_field_math<fifi::default_field_impl,
                 deep_symbol_storage<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<full_rlnc_encoder<Field>, Field>
                     > > > > > > > > > >
    {};

}


TEST(TestFileEncoder, test_storage_encoder)
{

    {
        boost::filesystem::path temp = boost::filesystem::unique_path();

        std::ofstream outfile;
        
        outfile.open (temp.native());
        
        uint32_t size = 500;
        for(uint32_t i = 0; i < size; ++i)
        {
            outfile << rand() % 255;
        }
        
        outfile.close();
        
        typedef kodo::test_rlnc_encoder<fifi::binary>
            encoder;
        
        typedef kodo::file_encoder<encoder>
            rlnc_file_encoder;
        
        rlnc_file_encoder::factory f(10,10);
        rlnc_file_encoder file_encoder(f, temp.native());

        std::cout << file_encoder.encoders() << std::endl;
        
    }

}




