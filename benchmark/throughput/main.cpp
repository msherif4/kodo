// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <boost/make_shared.hpp>

#include <gauge/gauge.hpp>
#include <gauge/console_printer.hpp>
#include <gauge/python_printer.hpp>
#include <gauge/csv_printer.hpp>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/rlnc/seed_codes.hpp>
#include <kodo/rs/reed_solomon_codes.hpp>

#include "codes.hpp"

/// A test block represents an encoder and decoder pair
template<class Encoder, class Decoder>
struct throughput_benchmark : public gauge::time_benchmark
{

    typedef typename Encoder::factory encoder_factory;
    typedef typename Encoder::pointer encoder_ptr;

    typedef typename Decoder::factory decoder_factory;
    typedef typename Decoder::pointer decoder_ptr;

    void init()
    {
        m_factor = 2;
        gauge::time_benchmark::init();
    }

    void start()
    {
        m_encoded_symbols = 0;
        m_decoded_symbols = 0;
        gauge::time_benchmark::start();
    }

    void stop()
    {
        gauge::time_benchmark::stop();
    }

    double measurement()
    {
        // Get the time spent per iteration
        double time = gauge::time_benchmark::measurement();

        gauge::config_set cs = get_current_configuration();
        std::string type = cs.get_value<std::string>("type");
        uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

        // The number of bytes {en|de}coded
        uint64_t total_bytes = 0;

        if(type == "decoder")
        {
            total_bytes = m_decoded_symbols * symbol_size;
        }
        else if(type == "encoder")
        {
            total_bytes = m_encoded_symbols * symbol_size;
        }
        else
        {
            assert(0);
        }

        // The bytes per iteration
        uint64_t bytes =
            total_bytes / gauge::time_benchmark::iteration_count();

        return bytes / time; // MB/s for each iteration
    }

    void store_run(gauge::table& results)
    {
        results.set_value("throughput", measurement());
    }

    bool accept_measurement()
    {
        gauge::config_set cs = get_current_configuration();

        std::string type = cs.get_value<std::string>("type");

        if(type == "decoder")
        {
            // If we are benchmarking a decoder we only accept
            // the measurement if the decoding was successful
            if(!m_decoder->is_complete())
            {
                // We did not generate enough payloads to decode successfully,
                // so we will generate more payloads for next run
                m_factor++;

                return false;
            }
        }

        return gauge::time_benchmark::accept_measurement();
    }

    std::string unit_text() const
    {
        return "MB/s";
    }

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t> >();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t> >();
        auto types = options["type"].as<std::vector<std::string> >();

        assert(symbols.size() > 0);
        assert(symbol_size.size() > 0);
        assert(types.size() > 0);

        for(uint32_t i = 0; i < symbols.size(); ++i)
        {
            for(uint32_t j = 0; j < symbol_size.size(); ++j)
            {
                for(uint32_t u = 0; u < types.size(); ++u)
                {
                    gauge::config_set cs;
                    cs.set_value<uint32_t>("symbols", symbols[i]);
                    cs.set_value<uint32_t>("symbol_size", symbol_size[j]);
                    cs.set_value<std::string>("type", types[u]);

                    add_configuration(cs);
                }
            }
        }
    }

    void setup()
    {
        gauge::config_set cs = get_current_configuration();

        uint32_t symbols = cs.get_value<uint32_t>("symbols");
        uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

        // Make the factories fit perfectly otherwise there seems to
        // be problems with memory access i.e. when using a factory
        // with max symbols 1024 with a symbols 16
        m_decoder_factory = std::make_shared<decoder_factory>(
            symbols, symbol_size);

        m_encoder_factory = std::make_shared<encoder_factory>(
            symbols, symbol_size);

        m_decoder_factory->set_symbols(symbols);
        m_decoder_factory->set_symbol_size(symbol_size);

        m_encoder_factory->set_symbols(symbols);
        m_encoder_factory->set_symbol_size(symbol_size);

        m_encoder = m_encoder_factory->build();
        m_decoder = m_decoder_factory->build();

        // Prepare the data to be encoded
        m_encoded_data.resize(m_encoder->block_size());

        for(uint8_t &e : m_encoded_data)
        {
            e = rand() % 256;
        }

        m_encoder->set_symbols(sak::storage(m_encoded_data));

        // Prepare storage to the encoded payloads
        uint32_t payload_count = symbols * m_factor;

        m_payloads.resize(payload_count);
        for(uint32_t i = 0; i < payload_count; ++i)
        {
            m_payloads[i].resize( m_encoder->payload_size() );
        }

        m_temp_payload.resize( m_encoder->payload_size() );
    }

    void encode_payloads()
    {
        m_encoder->set_symbols(sak::storage(m_encoded_data));

        // We switch any systematic operations off so we code
        // symbols from the beginning
        if(kodo::is_systematic_encoder(m_encoder))
            kodo::set_systematic_off(m_encoder);

        uint32_t payload_count = m_payloads.size();

        for(uint32_t i = 0; i < payload_count; ++i)
        {
            std::vector<uint8_t> &payload = m_payloads[i];
            m_encoder->encode(&payload[0]);

            ++m_encoded_symbols;
        }
    }

    void decode_payloads()
    {
        uint32_t payload_count = m_payloads.size();

        for(uint32_t i = 0; i < payload_count; ++i)
        {
            std::copy(m_payloads[i].begin(),
                      m_payloads[i].end(),
                      m_temp_payload.begin());

            m_decoder->decode(&m_temp_payload[0]);

            ++m_decoded_symbols;

            if(m_decoder->is_complete())
            {
                return;
            }
        }
    }

    /// Run the encoder
    void run_encode()
    {
        gauge::config_set cs = get_current_configuration();

        uint32_t symbols = cs.get_value<uint32_t>("symbols");
        uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

        m_encoder_factory->set_symbols(symbols);
        m_encoder_factory->set_symbol_size(symbol_size);

        // The clock is running
        RUN{
            // We have to make sure the encoder is in a "clean" state
            m_encoder->initialize(*m_encoder_factory);

            encode_payloads();
        }
    }

    /// Run the decoder
    void run_decode()
    {
        // Encode some data
        encode_payloads();

        gauge::config_set cs = get_current_configuration();

        uint32_t symbols = cs.get_value<uint32_t>("symbols");
        uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

        m_decoder_factory->set_symbols(symbols);
        m_decoder_factory->set_symbol_size(symbol_size);

        // The clock is running
        RUN{
            // We have to make sure the decoder is in a "clean" state
            // i.e. no symbols already decoded.
            m_decoder->initialize(*m_decoder_factory);

            // Decode the payloads
            decode_payloads();
        }
    }


    void run_benchmark()
    {
        gauge::config_set cs = get_current_configuration();

        std::string type = cs.get_value<std::string>("type");

        if(type == "encoder")
        {
            run_encode();
        }
        else if(type == "decoder")
        {
            run_decode();
        }
        else
        {
            assert(0);
        }

    }

protected:

    /// The decoder factory
    std::shared_ptr<decoder_factory> m_decoder_factory;

    /// The encoder factory
    std::shared_ptr<encoder_factory> m_encoder_factory;

    /// The encoder to use
    encoder_ptr m_encoder;

    /// The number of symbols encoded
    uint32_t m_encoded_symbols;

    /// The encoder to use
    decoder_ptr m_decoder;

    /// The number of symbols decoded
    uint32_t m_decoded_symbols;

    /// The data encoded
    std::vector<uint8_t> m_encoded_data;

    /// Temporary payload to not destroy the already encoded payloads
    /// when decoding
    std::vector<uint8_t> m_temp_payload;

    /// Storage for encoded symbols
    std::vector< std::vector<uint8_t> > m_payloads;

    /// Multiplication factor for payload_count
    uint32_t m_factor;

};


/// A test block represents an encoder and decoder pair
template<class Encoder, class Decoder>
struct sparse_throughput_benchmark :
    public throughput_benchmark<Encoder,Decoder>
{
public:

    /// The type of the base benchmark
    typedef throughput_benchmark<Encoder,Decoder> Super;

    /// We need access to the encoder built to adjust the density
    using Super::m_encoder;

public:

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t> >();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t> >();
        auto types = options["type"].as<std::vector<std::string> >();
        auto density = options["density"].as<std::vector<double> >();

        assert(symbols.size() > 0);
        assert(symbol_size.size() > 0);
        assert(types.size() > 0);
        assert(density.size() > 0);

        for(const auto& s : symbols)
        {
            for(const auto& p : symbol_size)
            {
                for(const auto& t : types)
                {
                    for(const auto& d: density)
                    {
                        gauge::config_set cs;
                        cs.set_value<uint32_t>("symbols", s);
                        cs.set_value<uint32_t>("symbol_size", p);
                        cs.set_value<std::string>("type", t);
                        cs.set_value<double>("density", d);

                        Super::add_configuration(cs);
                    }
                }
            }
        }
    }

    void setup()
    {
        Super::setup();

        gauge::config_set cs = Super::get_current_configuration();

        double density = cs.get_value<double>("density");
        m_encoder->set_density(density);
    }

};



/// Using this macro we may specify options. For specifying options
/// we use the boost program options library. So you may additional
/// details on how to do it in the manual for that library.
BENCHMARK_OPTION(throughput_options)
{
    gauge::po::options_description options;

    std::vector<uint32_t> symbols;
    symbols.push_back(16);
    symbols.push_back(32);
    symbols.push_back(64);
    symbols.push_back(128);

    auto default_symbols =
        gauge::po::value<std::vector<uint32_t> >()->default_value(
            symbols, "")->multitoken();

    std::vector<uint32_t> symbol_size;
    symbol_size.push_back(1600);

    auto default_symbol_size =
        gauge::po::value<std::vector<uint32_t> >()->default_value(
            symbol_size, "")->multitoken();

    std::vector<std::string> types;
    types.push_back("encoder");
    types.push_back("decoder");

    auto default_types =
        gauge::po::value<std::vector<std::string> >()->default_value(
            types, "")->multitoken();

    options.add_options()
        ("symbols", default_symbols, "Set the number of symbols");

    options.add_options()
        ("symbol_size", default_symbol_size, "Set the symbol size in bytes");

    options.add_options()
        ("type", default_types, "Set type [encoder|decoder]");

    gauge::runner::instance().register_options(options);
}

BENCHMARK_OPTION(throughput_density_options)
{
    gauge::po::options_description options;

    std::vector<double> density;
    density.push_back(0.1);
    density.push_back(0.2);
    density.push_back(0.3);
    density.push_back(0.4);
    density.push_back(0.5);

    auto default_density =
        gauge::po::value<std::vector<double> >()->default_value(
            density, "")->multitoken();

    options.add_options()
        ("density", default_density, "Set the density of the sparse codes");

    gauge::runner::instance().register_options(options);
}


typedef throughput_benchmark<
    kodo::full_rlnc_encoder<fifi::binary>,
    kodo::full_rlnc_decoder<fifi::binary> > setup_rlnc_throughput;

BENCHMARK_F(setup_rlnc_throughput, FullRLNC, Binary, 5)
{
    run_benchmark();
}

typedef throughput_benchmark<
    kodo::full_rlnc_encoder<fifi::binary8>,
    kodo::full_rlnc_decoder<fifi::binary8> > setup_rlnc_throughput8;

BENCHMARK_F(setup_rlnc_throughput8, FullRLNC, Binary8, 5)
{
    run_benchmark();
}

typedef throughput_benchmark<
    kodo::full_rlnc_encoder<fifi::binary16>,
    kodo::full_rlnc_decoder<fifi::binary16> > setup_rlnc_throughput16;

BENCHMARK_F(setup_rlnc_throughput16, FullRLNC, Binary16, 5)
{
    run_benchmark();
}

typedef throughput_benchmark<
    kodo::full_rlnc_encoder<fifi::prime2325>,
    kodo::full_rlnc_decoder<fifi::prime2325> > setup_rlnc_throughput2325;

BENCHMARK_F(setup_rlnc_throughput2325, FullRLNC, Prime2325, 5)
{
    run_benchmark();
}

typedef throughput_benchmark<
   kodo::full_rlnc_encoder<fifi::binary>,
   kodo::full_delayed_rlnc_decoder<fifi::binary> >
   setup_delayed_rlnc_throughput;

BENCHMARK_F(setup_delayed_rlnc_throughput, FullDelayedRLNC, Binary, 5)
{
   run_benchmark();
}

typedef throughput_benchmark<
   kodo::full_rlnc_encoder<fifi::binary8>,
   kodo::full_delayed_rlnc_decoder<fifi::binary8> >
   setup_delayed_rlnc_throughput8;

BENCHMARK_F(setup_delayed_rlnc_throughput8, FullDelayedRLNC, Binary8, 5)
{
   run_benchmark();
}

typedef throughput_benchmark<
   kodo::full_rlnc_encoder<fifi::binary16>,
   kodo::full_delayed_rlnc_decoder<fifi::binary16> >
   setup_delayed_rlnc_throughput16;

BENCHMARK_F(setup_delayed_rlnc_throughput16, FullDelayedRLNC, Binary16, 5)
{
   run_benchmark();
}

typedef throughput_benchmark<
   kodo::full_rlnc_encoder<fifi::prime2325>,
   kodo::full_delayed_rlnc_decoder<fifi::prime2325> >
   setup_delayed_rlnc_throughput2325;

BENCHMARK_F(setup_delayed_rlnc_throughput2325, FullDelayedRLNC, Prime2325, 5)
{
   run_benchmark();
}

/// Sparse

typedef sparse_throughput_benchmark<
    kodo::sparse_full_rlnc_encoder<fifi::binary>,
    kodo::full_rlnc_decoder<fifi::binary> > setup_sparse_rlnc_throughput;

BENCHMARK_F(setup_sparse_rlnc_throughput, SparseFullRLNC, Binary, 5)
{
    run_benchmark();
}

typedef sparse_throughput_benchmark<
    kodo::sparse_full_rlnc_encoder<fifi::binary8>,
    kodo::full_rlnc_decoder<fifi::binary8> > setup_sparse_rlnc_throughput8;

BENCHMARK_F(setup_sparse_rlnc_throughput8, SparseFullRLNC, Binary8, 5)
{
    run_benchmark();
}

typedef sparse_throughput_benchmark<
    kodo::sparse_full_rlnc_encoder<fifi::binary16>,
    kodo::full_rlnc_decoder<fifi::binary16> > setup_sparse_rlnc_throughput16;

BENCHMARK_F(setup_sparse_rlnc_throughput16, SparseFullRLNC, Binary16, 5)
{
    run_benchmark();
}




int main(int argc, const char* argv[])
{

    srand(static_cast<uint32_t>(time(0)));

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::console_printer>());

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::python_printer>());

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::csv_printer>());

    gauge::runner::run_benchmarks(argc, argv);

    return 0;
}

