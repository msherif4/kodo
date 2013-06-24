// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <boost/make_shared.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/bernoulli_distribution.hpp>

#include <gauge/gauge.hpp>
#include <gauge/console_printer.hpp>
#include <gauge/python_printer.hpp>
#include <gauge/csv_printer.hpp>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/rlnc/seed_codes.hpp>
#include <kodo/rs/reed_solomon_codes.hpp>
#include <kodo/has_deep_symbol_storage.hpp>

#include "codes.hpp"

// Helper function to convert to string
template<class T>
inline std::string to_string(T t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}


/// A test block represents an encoder and decoder pair
template<class Encoder, class Decoder>
struct decoding_probability_benchmark : public gauge::benchmark
{
public:

    typedef typename Encoder::factory encoder_factory;
    typedef typename Encoder::pointer encoder_ptr;

    typedef typename Decoder::factory decoder_factory;
    typedef typename Decoder::pointer decoder_ptr;

    static_assert(kodo::has_deep_symbol_storage<Decoder>::value,
                  "The decoder should bring its own memory");

    decoding_probability_benchmark()
    {
        // Seed the random generator controlling the erasures
        m_random_generator.seed((uint64_t)time(0));
    }

    void start()
    { }

    void stop()
    { }

    void store_run(gauge::table& results)
    {
        assert(m_encoder->block_size() ==
               m_decoder->block_size());

        assert(m_symbols_used > 0);
        assert(m_symbols_used >= m_encoder->symbols());

        results.set_value("used", m_symbols_used);

        for(uint32_t i = 0; i < m_rank_used.size(); ++i)
        {
            results.set_value("rank " + to_string(i), m_rank_used[i]);
        }

    }

    std::string unit_text() const
    {
        return "symbols";
    }

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t> >();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t> >();
        auto erasure = options["erasure"].as<std::vector<double> >();
        auto systematic = options["systematic"].as<bool>();

        assert(symbols.size() > 0);
        assert(symbol_size.size() > 0);
        assert(erasure.size() > 0);

        m_max_symbols = *std::max_element(symbols.begin(),
                                          symbols.end());

        m_max_symbol_size = *std::max_element(symbol_size.begin(),
                                              symbol_size.end());

        // Make the factories fit perfectly otherwise there seems to
        // be problems with memory access i.e. when using a factory
        // with max symbols 1024 with a symbols 16
        m_decoder_factory = std::make_shared<decoder_factory>(
            m_max_symbols, m_max_symbol_size);

        m_encoder_factory = std::make_shared<encoder_factory>(
            m_max_symbols, m_max_symbol_size);

        for(uint32_t i = 0; i < symbols.size(); ++i)
        {
            for(uint32_t j = 0; j < symbol_size.size(); ++j)
            {
                for(const auto& e : erasure)
                {
                    gauge::config_set cs;
                    cs.set_value<uint32_t>("symbols", symbols[i]);
                    cs.set_value<uint32_t>("symbol_size", symbol_size[j]);
                    cs.set_value<double>("erasure", e);
                    cs.set_value<bool>("systematic", systematic);
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
        bool systematic = cs.get_value<bool>("systematic");

        m_decoder_factory->set_symbols(symbols);
        m_decoder_factory->set_symbol_size(symbol_size);

        m_encoder_factory->set_symbols(symbols);
        m_encoder_factory->set_symbol_size(symbol_size);

        m_encoder = m_encoder_factory->build();
        m_decoder = m_decoder_factory->build();

        // We switch any systematic operations off so we code
        // symbols from the beginning
        if(kodo::is_systematic_encoder(m_encoder))
        {
            if(systematic)
            {
                kodo::set_systematic_on(m_encoder);
            }
            else
            {
                kodo::set_systematic_off(m_encoder);
            }
        }


        // Prepare the data to be encoded
        m_encoded_data.resize(m_encoder->block_size());

        for(uint8_t &e : m_encoded_data)
        {
            e = rand() % 256;
        }

        m_encoder->set_symbols(sak::storage(m_encoded_data));

        m_symbols_used = 0;
        m_rank_used.resize(symbols);
        std::fill(m_rank_used.begin(), m_rank_used.end(), 0);

        double erasure = cs.get_value<double>("erasure");
        m_distribution = boost::random::bernoulli_distribution<>(erasure);
    }

    /// Run the benchmark
    void run_benchmark()
    {
        assert(m_symbols_used == 0);
        assert(m_encoder);
        assert(m_decoder);

        std::vector<uint8_t> payload(m_encoder->payload_size());

        m_encoder->seed((uint32_t)time(0));

        // The clock is running
        RUN{

            while(!m_decoder->is_complete())
            {
                m_encoder->encode(&payload[0]);

                if(m_distribution(m_random_generator))
                    continue;

                ++m_symbols_used;
                ++m_rank_used[m_decoder->rank()];
                m_decoder->decode(&payload[0]);
            }
        }
    }

protected:

    /// The maximum number of symbols
    uint32_t m_max_symbols;

    /// The maximum symbol size
    uint32_t m_max_symbol_size;

    /// The decoder factory
    std::shared_ptr<decoder_factory> m_decoder_factory;

    /// The encoder factory
    std::shared_ptr<encoder_factory> m_encoder_factory;

    /// The encoder to use
    encoder_ptr m_encoder;

    /// The encoder to use
    decoder_ptr m_decoder;

    /// The data to encode
    std::vector<uint8_t> m_encoded_data;

    /// The number of symbols used to decode
    uint32_t m_symbols_used;

    /// The number of symbols used to decoder indexed by the rank of the
    /// decoder
    std::vector<uint32_t> m_rank_used;

    // The random generator
    boost::random::mt19937 m_random_generator;

    // The distribution wrapping the random generator
    boost::random::bernoulli_distribution<> m_distribution;


};

template<class Encoder, class Decoder>
struct sparse_decoding_probability_benchmark :
    public decoding_probability_benchmark<Encoder,Decoder>
{
public:

    /// The encoder and decoder factories
    typedef typename Encoder::factory encoder_factory;
    typedef typename Decoder::factory decoder_factory;

    /// The type of the base benchmark
    typedef decoding_probability_benchmark<Encoder,Decoder> Super;

    /// We need to access a couple of member variables from the
    /// base benchmark to setup the benchmark correctly
    using Super::m_encoder;
    using Super::m_decoder_factory;
    using Super::m_encoder_factory;
    using Super::m_max_symbols;
    using Super::m_max_symbol_size;

public:

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t> >();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t> >();
        auto erasure = options["erasure"].as<std::vector<double> >();
        auto density = options["density"].as<std::vector<double> >();
        auto systematic = options["systematic"].as<bool>();

        assert(symbols.size() > 0);
        assert(symbol_size.size() > 0);
        assert(erasure.size() > 0);
        assert(density.size() > 0);

        m_max_symbols = *std::max_element(symbols.begin(),
                                          symbols.end());

        m_max_symbol_size = *std::max_element(symbol_size.begin(),
                                              symbol_size.end());

        // Make the factories fit perfectly otherwise there seems to
        // be problems with memory access i.e. when using a factory
        // with max symbols 1024 with a symbols 16
        m_decoder_factory = std::make_shared<decoder_factory>(
            m_max_symbols, m_max_symbol_size);

        m_encoder_factory = std::make_shared<encoder_factory>(
            m_max_symbols, m_max_symbol_size);

        for(const auto& s : symbols)
        {
            for(const auto& p : symbol_size)
            {
                for(const auto& e : erasure)
                {
                    for(const auto& d: density)
                    {
                        gauge::config_set cs;
                        cs.set_value<uint32_t>("symbols", s);
                        cs.set_value<uint32_t>("symbol_size", p);
                        cs.set_value<double>("erasure", e);
                        cs.set_value<double>("density", d);
                        cs.set_value<bool>("systematic", systematic);
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

        assert(m_encoder);
        m_encoder->set_density(density);
    }

};


/// Using this macro we may specify options. For specifying options
/// we use the boost program options library. So you may additional
/// details on how to do it in the manual for that library.
BENCHMARK_OPTION(overhead_options)
{
    gauge::po::options_description options;

    std::vector<uint32_t> symbols;
    symbols.push_back(16);
    symbols.push_back(32);
    symbols.push_back(64);
    symbols.push_back(128);

    options.add_options()
        ("symbols", gauge::po::value<std::vector<uint32_t> >()->default_value(
            symbols, "")->multitoken(), "Set the number of symbols");

    std::vector<uint32_t> symbol_size;
    symbol_size.push_back(1500);

    options.add_options()
        ("symbol_size", gauge::po::value<std::vector<uint32_t> >()->default_value(
            symbol_size, "")->multitoken(), "Set the symbol size in bytes");

    std::vector<double> erasure;
    erasure.push_back(0.5);

    options.add_options()
        ("erasure", gauge::po::value<std::vector<double> >()->default_value(
            erasure, "")->multitoken(), "Set the symbol erasure probability");

    options.add_options()
        ("systematic", gauge::po::value<bool>()->default_value(
            true, ""), "Set the encoder systematic");

    gauge::runner::instance().register_options(options);
}

BENCHMARK_OPTION(overhead_density_options)
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

typedef decoding_probability_benchmark<
    kodo::full_rlnc_encoder<fifi::binary>,
    kodo::full_rlnc_decoder<fifi::binary> > setup_rlnc_overhead;

BENCHMARK_F(setup_rlnc_overhead, FullRLNC, Binary, 5)
{
    run_benchmark();
}

typedef decoding_probability_benchmark<
    kodo::full_rlnc_encoder<fifi::binary8>,
    kodo::full_rlnc_decoder<fifi::binary8> > setup_rlnc_overhead8;

BENCHMARK_F(setup_rlnc_overhead8, FullRLNC, Binary8, 5)
{
    run_benchmark();
}

typedef decoding_probability_benchmark<
    kodo::full_rlnc_encoder<fifi::binary16>,
    kodo::full_rlnc_decoder<fifi::binary16> > setup_rlnc_overhead16;

BENCHMARK_F(setup_rlnc_overhead16, FullRLNC, Binary16, 5)
{
    run_benchmark();
}

typedef decoding_probability_benchmark<
   kodo::full_rlnc_encoder<fifi::binary>,
   kodo::full_delayed_rlnc_decoder<fifi::binary> >
   setup_delayed_rlnc_overhead;

BENCHMARK_F(setup_delayed_rlnc_overhead, FullDelayedRLNC, Binary, 5)
{
   run_benchmark();
}

typedef decoding_probability_benchmark<
   kodo::full_rlnc_encoder<fifi::binary8>,
   kodo::full_delayed_rlnc_decoder<fifi::binary8> >
   setup_delayed_rlnc_overhead8;

BENCHMARK_F(setup_delayed_rlnc_overhead8, FullDelayedRLNC, Binary8, 5)
{
   run_benchmark();
}

typedef decoding_probability_benchmark<
   kodo::full_rlnc_encoder<fifi::binary16>,
   kodo::full_delayed_rlnc_decoder<fifi::binary16> >
   setup_delayed_rlnc_overhead16;

BENCHMARK_F(setup_delayed_rlnc_overhead16, FullDelayedRLNC, Binary16, 5)
{
   run_benchmark();
}

//------------------------------------------------------------------
// Unsystematic encoder and decoder
//------------------------------------------------------------------


typedef decoding_probability_benchmark<
   kodo::full_rlnc_encoder_unsystematic<fifi::binary>,
   kodo::full_rlnc_decoder_unsystematic<fifi::binary> >
   setup_full_rlnc_unsystematic;

BENCHMARK_F(setup_full_rlnc_unsystematic, FullRLNCUnsystematic, Binary, 5)
{
   run_benchmark();
}

typedef decoding_probability_benchmark<
   kodo::full_rlnc_encoder_unsystematic<fifi::binary8>,
   kodo::full_rlnc_decoder_unsystematic<fifi::binary8> >
   setup_full_rlnc_unsystematic8;

BENCHMARK_F(setup_full_rlnc_unsystematic8, FullRLNCUnsystematic, Binary8, 5)
{
   run_benchmark();
}

typedef decoding_probability_benchmark<
   kodo::full_rlnc_encoder_unsystematic<fifi::binary16>,
   kodo::full_rlnc_decoder_unsystematic<fifi::binary16> >
   setup_full_rlnc_unsystematic16;

BENCHMARK_F(setup_full_rlnc_unsystematic16, FullRLNCUnsystematic, Binary16, 5)
{
   run_benchmark();
}

/// Sparse

typedef sparse_decoding_probability_benchmark<
    kodo::sparse_full_rlnc_encoder<fifi::binary>,
    kodo::full_rlnc_decoder<fifi::binary> > setup_sparse_rlnc_decoding_probability;

BENCHMARK_F(setup_sparse_rlnc_decoding_probability, SparseFullRLNC, Binary, 5)
{
    run_benchmark();
}

typedef sparse_decoding_probability_benchmark<
    kodo::sparse_full_rlnc_encoder<fifi::binary8>,
    kodo::full_rlnc_decoder<fifi::binary8> > setup_sparse_rlnc_decoding_probability8;

BENCHMARK_F(setup_sparse_rlnc_decoding_probability8, SparseFullRLNC, Binary8, 5)
{
    run_benchmark();
}

typedef sparse_decoding_probability_benchmark<
    kodo::sparse_full_rlnc_encoder<fifi::binary16>,
    kodo::full_rlnc_decoder<fifi::binary16> > setup_sparse_rlnc_decoding_probability16;

BENCHMARK_F(setup_sparse_rlnc_decoding_probability16, SparseFullRLNC, Binary16, 5)
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

