// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <kodo/rlnc/full_vector_codes.h>

#include <gauge/gauge.h>
#include <gauge/console_printer.h>
#include <gauge/python_printer.h>


namespace kodo
{

    /// This layer "intercepts" all calls to the finite_field_math
    /// layer counting the different operations
    template<class SuperCoder>
    class count_finite_field_operations : public SuperCoder
    {
    public:
        /// The field type used
        typedef typename SuperCoder::field_type field_type;

        /// The value type
        typedef typename field_type::value_type value_type;


    public:

        /// @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                /// Reset the state
                m_multiply = 0;
                m_multiply_add = 0;
                m_add = 0;
                m_multiply_subtract = 0;
                m_subtract = 0;
                m_invert = 0;
            }


        /// @see finite_field_math::multiply(...)
        void multiply(value_type *symbol_dest, value_type coefficient,
                      uint32_t symbol_length)
            {
                ++m_multiply;
                SuperCoder::multiply(symbol_dest, coefficient, symbol_length);
            }

        void multiply_add(value_type *symbol_dest, const value_type *symbol_src,
                          value_type coefficient, uint32_t symbol_length)
            {
                ++m_multiply_add;
                SuperCoder::multiply_add(symbol_dest, symbol_src, coefficient,
                                         symbol_length);
            }

        void add(value_type *symbol_dest, const value_type *symbol_src,
                 uint32_t symbol_length)
            {
                ++m_add;
                SuperCoder::add(symbol_dest, symbol_src, symbol_length);
            }

        void multiply_subtract(value_type *symbol_dest, const value_type *symbol_src,
                               value_type coefficient, uint32_t symbol_length)
            {
                ++m_multiply_subtract;
                SuperCoder::multiply_subtract(symbol_dest, symbol_src,
                                              coefficient, symbol_length);
            }

        void subtract(value_type *symbol_dest, const value_type *symbol_src,
                      uint32_t symbol_length)
            {
                ++m_subtract;
                SuperCoder::subtract(symbol_dest, symbol_src, symbol_length);
            }

        value_type invert(value_type value)
            {
                ++m_invert;
                SuperCoder::invert(value);
            }

    private:

        /// Counter for dest[i] = dest[i] * src[i]
        uint32_t m_multiply;

        /// Counter for dest[i] = dest[i] + (constant * src[i])
        uint32_t m_multiply_add;

        /// Counter for dest[i] = dest[i] + src[i]
        uint32_t m_add;

        /// Counter for dest[i] = dest[i] - (constant * src[i])
        uint32_t m_multiply_subtract;

        /// Counter for dest[i] = dest[i] - src[i]
        uint32_t m_subtract;

        /// Counter for invert(value)
        uint32_t m_invert;
    };


    template<class Field>
    class full_rlnc_encoder_count
        : public payload_encoder<
                 systematic_encoder<
                 zero_symbol_encoder<
                 full_vector_encoder<
                 linear_block_vector_generator<block_uniform_no_position,
                 linear_block_encoder<
                 count_finite_field_operations<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_shallow_partial<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<full_rlnc_encoder_count<Field>, Field>
                     > > > > > > > > > > >
    {};

}


int main(int argc, const char* argv[])
{
    kodo::full_rlnc_encoder_count<fifi::binary>::factory d(2,2);

    srand(static_cast<uint32_t>(time(0)));

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::console_printer>());

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::python_printer>("out.py"));

    gauge::runner::run_benchmarks(argc, argv);
    return 0;
}


