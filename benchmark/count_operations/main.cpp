// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <kodo/rlnc/full_vector_codes.h>

#include <gauge/gauge.h>
#include <gauge/console_printer.h>
#include <gauge/python_printer.h>


std::vector<uint32_t> setup_symbols()
{
    std::vector<uint32_t> symbols = {16, 32, 64, 128, 1024};
    return symbols;
}

std::vector<uint32_t> setup_symbol_size()
{
    std::vector<uint32_t> symbol_size = {100, 1000, 2000};
    return symbol_size;
}

/// Returns which operation to measure
std::vector<std::string> setup_operations()
{
    std::vector<std::string> operations =
        {
            "dest[i] = dest[i] + src[i]",
            "dest[i] = dest[i] - src[i]",
            "dest[i] = dest[i] * src[i]",
            "dest[i] = dest[i] + (constant * src[i])",
            "dest[i] = dest[i] - (constant * src[i])",
            "invert(value)"
        };

    return operations;
}

/// Returns which operation to measure
std::vector<std::string> setup_types()
{
    std::vector<std::string> types =
        {
            "encoder",
            "decoder"
        };

    return types;
}


namespace kodo
{
    struct operations_counter
    {
        operations_counter()
            : m_multiply(0),
              m_multiply_add(0),
              m_add(0),
              m_multiply_subtract(0),
              m_subtract(0),
              m_invert(0)
            { }

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

    /// This layer "intercepts" all calls to the finite_field_math
    /// layer counting the different operations
    template<class SuperCoder>
    class finite_field_math_counter : public SuperCoder
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

                // Reset the counter
                m_counter = operations_counter();
            }


        /// @see finite_field_math::multiply(...)
        void multiply(value_type *symbol_dest, value_type coefficient,
                      uint32_t symbol_length)
            {
                ++m_counter.m_multiply;
                SuperCoder::multiply(symbol_dest, coefficient, symbol_length);
            }

        void multiply_add(value_type *symbol_dest, const value_type *symbol_src,
                          value_type coefficient, uint32_t symbol_length)
            {
                ++m_counter.m_multiply_add;
                SuperCoder::multiply_add(symbol_dest, symbol_src, coefficient,
                                        symbol_length);
            }

        void add(value_type *symbol_dest, const value_type *symbol_src,
                 uint32_t symbol_length)
            {
                ++m_counter.m_add;
                SuperCoder::add(symbol_dest, symbol_src, symbol_length);
            }

        void multiply_subtract(value_type *symbol_dest, const value_type *symbol_src,
                               value_type coefficient, uint32_t symbol_length)
            {
                ++m_counter.m_multiply_subtract;
                SuperCoder::multiply_subtract(symbol_dest, symbol_src,
                                              coefficient, symbol_length);
            }

        void subtract(value_type *symbol_dest, const value_type *symbol_src,
                      uint32_t symbol_length)
            {
                ++m_counter.m_subtract;
                SuperCoder::subtract(symbol_dest, symbol_src, symbol_length);
            }

        value_type invert(value_type value)
            {
                ++m_counter.m_invert;
                return SuperCoder::invert(value);
            }

        operations_counter get_operations_counter() const
            {
                return m_counter;
            }

    private:

        /// Operations counter
        operations_counter m_counter;

    };


    template<class Field>
    class full_rlnc_encoder_count
        : public payload_encoder<
                 systematic_encoder<
                 zero_symbol_encoder<
                 full_vector_encoder<
                 linear_block_vector_generator<block_uniform_no_position,
                 linear_block_encoder<
                 finite_field_math_counter<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_shallow_partial<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<full_rlnc_encoder_count<Field>, Field>
                     > > > > > > > > > > >
    {};

    /// A RLNC decoder. The decoder decodes according to a
    /// full encoding vector.
    template<class Field>
    class full_rlnc_decoder_count
        : public payload_decoder<
                 systematic_decoder<
                 full_vector_decoder<
                 linear_block_decoder<
                 linear_block_vector_storage<
                 finite_field_math_counter<
                 finite_field_math<fifi::default_field_impl,
                 symbol_storage_deep<
                 has_bytes_used<
                 has_block_info<
                 final_coder_factory_pool<full_rlnc_decoder_count<Field>, Field>
                     > > > > > > > > > >
    {};

}

template<class Encoder, class Decoder>
class operations_benchmark : public gauge::benchmark
{
public:

    typedef typename Encoder::factory encoder_factory;
    typedef typename Encoder::pointer encoder_ptr;

    typedef typename Decoder::factory decoder_factory;
    typedef typename Decoder::pointer decoder_ptr;

    operations_benchmark()
        {
            auto symbols = setup_symbols();
            auto symbol_size = setup_symbol_size();

            uint32_t max_symbols =
                *std::max_element(symbols.begin(), symbols.end());

            uint32_t max_symbol_size =
                *std::max_element(symbol_size.begin(), symbol_size.end());

            m_decoder_factory = std::make_shared<decoder_factory>(max_symbols,
                                                                  max_symbol_size);

            m_encoder_factory = std::make_shared<encoder_factory>(max_symbols,
                                                                  max_symbol_size);

            auto operations = setup_operations();
            auto types = setup_types();

            for(uint32_t i = 0; i < symbols.size(); ++i)
            {
                for(uint32_t j = 0; j < symbol_size.size(); ++j)
                {
                    for(uint32_t k = 0; k < operations.size(); ++k)
                    {
                        for(uint32_t u = 0; u < types.size(); ++u)
                        {
                            gauge::config_set cs;
                            cs.set_value<uint32_t>("symbols", symbols[i]);
                            cs.set_value<uint32_t>("symbol_size", symbol_size[j]);
                            cs.set_value<std::string>("operation", operations[k]);
                            cs.set_value<std::string>("type", types[u]);
//                        cs.set_value<uint32_t>("element_size", sizeof(value_type));
                            add_configuration(cs);
                        }
                    }
                }
            }
        }

    /// Starts a measurement and saves the counter
    void start()
        {
            gauge::config_set cs = get_current_configuration();

            std::string type = cs.get_value<std::string>("type");

            if(type == "encoder")
            {
                m_old_counter = m_encoder->get_operations_counter();
            }
            else if(type == "decoder")
            {
                m_old_counter = m_decoder->get_operations_counter();
            }
            else
            {
                assert(0);
            }

            m_decoded_symbols = 0;
            m_encoded_symbols = 0;
        }

    /// Stops a measurement and saves the counter
    void stop()
        {
            gauge::config_set cs = get_current_configuration();

            std::string type = cs.get_value<std::string>("type");

            if(type == "encoder")
            {
                m_new_counter = m_encoder->get_operations_counter();
            }
            else if(type == "decoder")
            {
                m_new_counter = m_decoder->get_operations_counter();
            }
            else
            {
                assert(0);
            }

        }

    /// Prepares the measurement for every run
    void setup()
        {
            gauge::config_set cs = get_current_configuration();

            uint32_t symbols = cs.get_value<uint32_t>("symbols");
            uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

            m_encoder = m_encoder_factory->build(symbols, symbol_size);
            m_decoder = m_decoder_factory->build(symbols, symbol_size);

            m_payload_buffer.resize(m_encoder->payload_size(), 0);
            m_encoded_data.resize(m_encoder->block_size(), 'x');

            kodo::set_symbols(kodo::storage(m_encoded_data), m_encoder);
        }

    /// Returns the measurement - checks which operation we want to
    /// monitor and then returns the count for that operation.
    /// @return the number of operations performed
    double measurement()
        {
            gauge::config_set cs = get_current_configuration();

            std::string operation = cs.get_value<std::string>("operation");
            std::string type = cs.get_value<std::string>("type");

            uint32_t old_count = 0;
            uint32_t new_count = 0;
            uint32_t symbols_coded = 0;

            if(type == "encoder")
            {
                symbols_coded = m_encoded_symbols;
            }
            else if(type == "decoder")
            {
                symbols_coded = m_decoded_symbols;
            }
            else
            {
                assert(0);
            }


            if(operation == "dest[i] = dest[i] + src[i]")
            {
                old_count = m_old_counter.m_add;
                new_count = m_new_counter.m_add;
            }
            else if(operation == "dest[i] = dest[i] - src[i]")
            {
                old_count = m_old_counter.m_subtract;
                new_count = m_new_counter.m_subtract;
            }
            else if(operation == "dest[i] = dest[i] * src[i]")
            {
                old_count = m_old_counter.m_multiply;
                new_count = m_new_counter.m_multiply;
            }
            else if(operation == "dest[i] = dest[i] + (constant * src[i])")
            {
                old_count = m_old_counter.m_multiply_add;
                new_count = m_new_counter.m_multiply_add;
            }
            else if(operation == "dest[i] = dest[i] - (constant * src[i])")
            {
                old_count = m_old_counter.m_multiply_subtract;
                new_count = m_new_counter.m_multiply_subtract;
            }
            else if(operation == "invert(value)")
            {
                old_count = m_old_counter.m_invert;
                new_count = m_new_counter.m_invert;
            }
            else
            {
                assert(0);
            }

            assert(new_count >= old_count);
            uint32_t diff = new_count - old_count;
            double result = double(diff) / double(symbols_coded);
            return result;
        }

    /// Where the actual measurement is performed
    void run()
        {

            // We switch any systematic operations off so we code
            // symbols from the beginning
            if(kodo::is_systematic_encoder(m_encoder))
                kodo::set_systematic_off(m_encoder);

            RUN{

                while( !m_decoder->is_complete() )
                {
                    // Encode a packet into the payload buffer
                    m_encoder->encode( &m_payload_buffer[0] );

                    ++m_encoded_symbols;

                    if(rand() % 2)
                    {
                        // Pass that packet to the decoder
                        m_decoder->decode( &m_payload_buffer[0] );

                        ++m_decoded_symbols;
                    }
                }
            }
        }

    /// The unit we measure in
    std::string unit_text() const
        { return "operations"; }

protected:

    /// The decoder factory
    std::shared_ptr<decoder_factory> m_decoder_factory;

    /// The encoder factory
    std::shared_ptr<encoder_factory> m_encoder_factory;

    /// The decoder
    decoder_ptr m_decoder;

    /// Number of symbols passed to the decoder
    uint32_t m_decoded_symbols;

    /// The Encoder
    encoder_ptr m_encoder;

    /// Number of symbols passed to the decoder
    uint32_t m_encoded_symbols;

    /// The payload buffer
    std::vector<uint8_t> m_payload_buffer;

    /// The payload buffer
    std::vector<uint8_t> m_encoded_data;

    /// The counter before the measurement starts
    kodo::operations_counter m_old_counter;

    /// The counter after the measurement
    kodo::operations_counter m_new_counter;
};

typedef operations_benchmark<
    kodo::full_rlnc_encoder_count<fifi::binary>,
    kodo::full_rlnc_decoder_count<fifi::binary> > setup_rlnc_count;

BENCHMARK_F(setup_rlnc_count, RLNC, Binary, 5)
{
    run();
}

typedef operations_benchmark<
    kodo::full_rlnc_encoder_count<fifi::binary8>,
    kodo::full_rlnc_decoder_count<fifi::binary8> > setup_rlnc_count8;

BENCHMARK_F(setup_rlnc_count8, RLNC, Binary8, 5)
{
    run();
}

typedef operations_benchmark<
    kodo::full_rlnc_encoder_count<fifi::binary16>,
    kodo::full_rlnc_decoder_count<fifi::binary16> > setup_rlnc_count16;

BENCHMARK_F(setup_rlnc_count16, RLNC, Binary16, 5)
{
    run();
}


int main(int argc, const char* argv[])
{

    srand(static_cast<uint32_t>(time(0)));

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::console_printer>());

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::python_printer>("out.py"));

    gauge::runner::run_benchmarks(argc, argv);
    return 0;
}


