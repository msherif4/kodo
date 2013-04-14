// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>
#include <stack>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <gauge/gauge.hpp>
#include <gauge/console_printer.hpp>
#include <gauge/python_printer.hpp>

#include <kodo/partial_shallow_symbol_storage.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>

std::vector<uint32_t> setup_symbols()
{
    std::vector<uint32_t> symbols;
    symbols.push_back(16);
    symbols.push_back(32);
    symbols.push_back(64);
    symbols.push_back(128);
    symbols.push_back(1024);
    return symbols;
}

std::vector<uint32_t> setup_symbol_size()
{
    std::vector<uint32_t> symbol_size;
    symbol_size.push_back(1600);

    return symbol_size;
}

/// Returns which operation to measure
std::vector<std::string> setup_operations()
{
    std::vector<std::string> operations;
    operations.push_back("dest[i] = dest[i] + src[i]");
    operations.push_back("dest[i] = dest[i] - src[i]");
    operations.push_back("dest[i] = dest[i] * constant");
    operations.push_back("dest[i] = dest[i] + (constant * src[i])");
    operations.push_back("dest[i] = dest[i] - (constant * src[i])");
    operations.push_back("invert(value)");

    return operations;
}

/// Returns which operation to measure
std::vector<std::string> setup_types()
{
    std::vector<std::string> types;
    types.push_back("encoder");
    types.push_back("decoder");

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

        /// Counter for dest[i] = dest[i] * constant
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

    operations_counter operator-(const operations_counter &a,
                                 const operations_counter &b)
    {
        operations_counter res;

        res.m_multiply = a.m_multiply - b.m_multiply;

        res.m_multiply_add = a.m_multiply_add - b.m_multiply_add;

        res.m_add = a.m_add - b.m_add;

        res.m_multiply_subtract =
            a.m_multiply_subtract - b.m_multiply_subtract;

        res.m_subtract = a.m_subtract - b.m_subtract;

        res.m_invert = a.m_invert - b.m_invert;
        return res;
    }

    bool operator>=(const operations_counter &a,
                    const operations_counter &b)
    {
        if(a.m_multiply >= b.m_multiply)
            return true;
        if(a.m_multiply < b.m_multiply)
            return false;
        if(a.m_multiply_add >= b.m_multiply_add)
            return true;
        if(a.m_multiply_add < b.m_multiply_add)
            return false;
        if(a.m_add >= b.m_add)
            return true;
        if(a.m_add < b.m_add)
            return false;
        if(a.m_multiply_subtract >= b.m_multiply_subtract)
            return true;
        if(a.m_multiply_subtract < b.m_multiply_subtract)
            return false;
        if(a.m_subtract >= b.m_subtract)
            return true;
        if(a.m_subtract < b.m_subtract)
            return false;
        if(a.m_invert >= b.m_invert)
            return true;
        if(a.m_invert < b.m_invert)
            return false;

        return false;
    }

    /// This layer "intercepts" all calls to the finite_field_math
    /// layer counting the different operations
    template<class SuperCoder>
    class finite_field_math_counter : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// @copydoc layer::factory
        typedef typename SuperCoder::factory factory;

    public:

        /// @copydoc layer::initialize(factory&)
        void initialize(factory& the_factory)
            {
                SuperCoder::initialize(the_factory);

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

        void multiply_add(value_type *symbol_dest,
                          const value_type *symbol_src,
                          value_type coefficient, uint32_t symbol_length)
            {
                ++m_counter.m_multiply_add;
                SuperCoder::multiply_add(symbol_dest, symbol_src,
                                        coefficient,
                                        symbol_length);
            }

        void add(value_type *symbol_dest, const value_type *symbol_src,
                 uint32_t symbol_length)
            {
                ++m_counter.m_add;
                SuperCoder::add(symbol_dest, symbol_src, symbol_length);
            }

        void multiply_subtract(value_type *symbol_dest,
                               const value_type *symbol_src,
                               value_type coefficient,
                               uint32_t symbol_length)
            {
                ++m_counter.m_multiply_subtract;
                SuperCoder::multiply_subtract(symbol_dest, symbol_src,
                                              coefficient, symbol_length);
            }

        void subtract(value_type *symbol_dest, const value_type *symbol_src,
                      uint32_t symbol_length)
            {
                ++m_counter.m_subtract;
                SuperCoder::subtract(symbol_dest, symbol_src,
                                     symbol_length);
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
        : public
          // Payload Codec API
          payload_encoder<
          // Codec Header API
          systematic_encoder<
          symbol_id_encoder<
          // Symbol ID API
          plain_symbol_id_writer<
          // Coefficient Generator API
          uniform_generator<
          // Codec API
          storage_aware_encoder<
          encode_symbol_tracker<
          zero_symbol_encoder<
          linear_block_encoder<
          // Coefficient Storage API
          coefficient_info<
          // Symbol Storage API
          partial_shallow_symbol_storage<
          storage_bytes_used<
          storage_block_info<
          // Finite Field API
          finite_field_math_counter<
          finite_field_math<typename fifi::default_field<Field>::type,
          finite_field_info<Field,
          // Factory API
          final_coder_factory_pool<
          // Final type
          full_rlnc_encoder_count<Field>
              > > > > > > > > > > > > > > > > >
    { };

    template<class Field>
    class full_rlnc_decoder_count
        : public
          // Payload API
          payload_decoder<
          // Codec Header API
          systematic_decoder<
          symbol_id_decoder<
          // Symbol ID API
          plain_symbol_id_reader<
          // Codec API
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
          finite_field_math_counter<
          finite_field_math<typename fifi::default_field<Field>::type,
          finite_field_info<Field,
          // Factory API
          final_coder_factory_pool<
          // Final type
          full_rlnc_decoder_count<Field>
              > > > > > > > > > > > > > > >
    { };

}


struct config_less_than
    : public std::binary_function<gauge::config_set, gauge::config_set, bool>
{
    bool operator()(const gauge::config_set &n1, const gauge::config_set &n2) const
    {
        if(n1.get_value<uint32_t>("symbols") < n2.get_value<uint32_t>("symbols"))
            return true;
        if(n1.get_value<uint32_t>("symbols") > n2.get_value<uint32_t>("symbols"))
            return false;
        if(n1.get_value<uint32_t>("symbol_size") < n2.get_value<uint32_t>("symbol_size"))
            return true;
        if(n1.get_value<uint32_t>("symbol_size") > n2.get_value<uint32_t>("symbol_size"))
            return false;
        if(n1.get_value<std::string>("operation") < n2.get_value<std::string>("operation"))
            return true;
        if(n1.get_value<std::string>("operation") > n2.get_value<std::string>("operation"))
            return false;
        if(n1.get_value<std::string>("type") < n2.get_value<std::string>("type"))
            return true;
        if(n1.get_value<std::string>("type") > n2.get_value<std::string>("type"))
            return false;

        return false;
    }
};

/// When we encode / decode data we actually record many counters although
/// the benchmark only asks for one-at-a-time to avoid running the same
/// the same configuration for the encoder / decoder again and again for
/// the different operations this small class "remembers" the results.
/// So that if the configuration did not change we may simply return the
/// unread results
class result_memory
{
public:
    typedef std::stack<double> result_stack;
    typedef std::map<gauge::config_set, result_stack, config_less_than> result_map;

    void store_result(uint32_t symbols,
                      uint32_t symbol_size,
                      const std::string &operation,
                      const std::string &type,
                      double result)
        {
            // Create the config
            gauge::config_set cs;
            cs.set_value<uint32_t>("symbols", symbols);
            cs.set_value<uint32_t>("symbol_size", symbol_size);
            cs.set_value<std::string>("operation", operation);
            cs.set_value<std::string>("type", type);

            m_results[cs].push(result);
            assert(m_results[cs].size() > 0);
        }

    bool has_result(const gauge::config_set &config) const
        {
            auto it = m_results.find(config);

            if(it == m_results.end())
                return false;
            else
                return it->second.size() > 0;
        }

    double measurement(const gauge::config_set &config)
        {
            assert(has_result(config));

            result_stack &r = m_results[config];

            double result = r.top();
            r.pop();

            return result;
        }

    result_map m_results;

};

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

            // Should never be run if we already have the result since
            // it is called from the RUN macro
            assert(!m_result_memory.has_result(cs));

            m_old_encoder_counter = m_encoder->get_operations_counter();
            m_old_decoder_counter = m_decoder->get_operations_counter();

            m_encoded_symbols = 0;
            m_decoded_symbols = 0;
        }

    /// Stops a measurement and saves the counter
    void stop()
        {
            gauge::config_set cs = get_current_configuration();

            // Should never be run if we already have the result since
            // it is called from the RUN macro
            assert(!m_result_memory.has_result(cs));

            m_new_encoder_counter = m_encoder->get_operations_counter();
            m_new_decoder_counter = m_decoder->get_operations_counter();
        }

    /// Prepares the measurement for every run
    void setup()
        {
            gauge::config_set cs = get_current_configuration();

            if(m_result_memory.has_result(cs))
                return;

            uint32_t symbols = cs.get_value<uint32_t>("symbols");
            uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

            m_decoder_factory->set_symbols(symbols);
            m_decoder_factory->set_symbol_size(symbol_size);

            m_encoder_factory->set_symbols(symbols);
            m_encoder_factory->set_symbol_size(symbol_size);

            m_encoder = m_encoder_factory->build();
            m_decoder = m_decoder_factory->build();

            m_payload_buffer.resize(m_encoder->payload_size(), 0);
            m_encoded_data.resize(m_encoder->block_size(), 'x');

            m_encoder->set_symbols(sak::storage(m_encoded_data));
        }

    /// Returns the measurement - checks which operation we want to
    /// monitor and then returns the count for that operation.
    /// @return the number of operations performed
    double measurement()
        {
            gauge::config_set cs = get_current_configuration();

            if(m_result_memory.has_result(cs))
            {
                return m_result_memory.measurement(cs);
            }

            uint32_t symbols = cs.get_value<uint32_t>("symbols");
            uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

            double result = 0;

            assert(m_new_encoder_counter >= m_old_encoder_counter);
            assert(m_new_decoder_counter >= m_old_decoder_counter);

            // Store all results in the result memory
            kodo::operations_counter encoder_diff =
                m_new_encoder_counter - m_old_encoder_counter;

            kodo::operations_counter decoder_diff =
                m_new_decoder_counter - m_old_decoder_counter;

            result = double(encoder_diff.m_add) / double(m_encoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] + src[i]",
                                         "encoder", result);

            result = double(decoder_diff.m_add) / double(m_decoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] + src[i]",
                                         "decoder", result);

            result = double(encoder_diff.m_subtract) / double(m_encoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] - src[i]",
                                         "encoder", result);

            result = double(decoder_diff.m_subtract) / double(m_decoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] - src[i]",
                                         "decoder", result);

            result = double(encoder_diff.m_multiply) / double(m_encoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] * constant",
                                         "encoder", result);

            result = double(decoder_diff.m_multiply) / double(m_decoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] * constant",
                                         "decoder", result);

            result = double(encoder_diff.m_multiply_add) / double(m_encoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] + (constant * src[i])",
                                         "encoder", result);

            result = double(decoder_diff.m_multiply_add) / double(m_decoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] + (constant * src[i])",
                                         "decoder", result);

            result = double(encoder_diff.m_multiply_subtract) / double(m_encoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] - (constant * src[i])",
                                         "encoder", result);

            result = double(decoder_diff.m_multiply_subtract) / double(m_decoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "dest[i] = dest[i] - (constant * src[i])",
                                         "decoder", result);

            result = double(encoder_diff.m_invert) / double(m_encoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "invert(value)",
                                         "encoder", result);

            result = double(decoder_diff.m_invert) / double(m_decoded_symbols);
            m_result_memory.store_result(symbols, symbol_size,
                                         "invert(value)",
                                         "decoder", result);

            // We SHOULD :) now have stored the result we are looking for
            assert(m_result_memory.has_result(cs));

            return m_result_memory.measurement(cs);

        }

    /// Where the actual measurement is performed
    void run()
        {

            // Check with the result memory whether we already have
            // results for this configuration
            gauge::config_set cs = get_current_configuration();

            if(m_result_memory.has_result(cs))
                return;

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
        { return "operations per symbol"; }

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
    kodo::operations_counter m_old_encoder_counter;

    /// The counter after the measurement
    kodo::operations_counter m_new_encoder_counter;

    /// The counter before the measurement starts
    kodo::operations_counter m_old_decoder_counter;

    /// The counter after the measurement
    kodo::operations_counter m_new_decoder_counter;

    /// Result memory
    result_memory m_result_memory;
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

