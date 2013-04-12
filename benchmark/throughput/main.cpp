// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <boost/make_shared.hpp>

#include <gauge/gauge.hpp>
#include <gauge/console_printer.hpp>
#include <gauge/python_printer.hpp>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/rlnc/seed_codes.hpp>
#include <kodo/rs/reed_solomon_codes.hpp>

std::vector<uint32_t> setup_symbols()
{
    std::vector<uint32_t> symbols;
    symbols.push_back(16);
    symbols.push_back(32);
    symbols.push_back(64);
    symbols.push_back(128);
//    symbols.push_back(1024);
    return symbols;
}

std::vector<uint32_t> setup_symbol_size()
{
    std::vector<uint32_t> symbol_size;
    symbol_size.push_back(1600);

    return symbol_size;
}

/// Returns which operation to measure
std::vector<std::string> setup_types()
{
    std::vector<std::string> types;
    types.push_back("encoder");
    types.push_back("decoder");

    return types;
}

/// A test block represents an encoder and decoder pair
template<class Encoder, class Decoder>
struct throughput_benchmark : public gauge::time_benchmark
{

    typedef typename Encoder::factory encoder_factory;
    typedef typename Encoder::pointer encoder_ptr;

    typedef typename Decoder::factory decoder_factory;
    typedef typename Decoder::pointer decoder_ptr;


    throughput_benchmark()
        {
            auto symbols = setup_symbols();
            auto symbol_size = setup_symbol_size();

            // uint32_t max_symbols =
            //     *std::max_element(symbols.begin(), symbols.end());

            // uint32_t max_symbol_size =
            //     *std::max_element(symbol_size.begin(), symbol_size.end());

            // m_decoder_factory = std::make_shared<decoder_factory>(
            //     max_symbols, max_symbol_size);

            // m_encoder_factory = std::make_shared<encoder_factory>(
            //     max_symbols, max_symbol_size);

            auto types = setup_types();

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
                    return false;
                }
            }

            return gauge::time_benchmark::accept_measurement();
        }

    std::string unit_text() const
        {
            return "MB/s";
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
            uint32_t payload_count = symbols * 10;

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

            // The clock is running
            RUN{
                // We have to make sure the encoder is in a "clean" state
                m_encoder->initialize(symbols, symbol_size);

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

            // The clock is running
            RUN{
                // We have to make sure the decoder is in a "clean" state
                // i.e. no symbols already decoded.
                m_decoder->initialize(symbols, symbol_size);

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

};

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


//     run("seed_rlnc_2",
//         make_block<
//         kodo::seed_rlnc_encoder<fifi::binary>,
//         kodo::seed_rlnc_decoder<fifi::binary> >(setup));

//     run("seed_rlnc_8",
//         make_block<
//         kodo::seed_rlnc_encoder<fifi::binary8>,
//         kodo::seed_rlnc_decoder<fifi::binary8> >(setup));

//     run("seed_rlnc_16",
//         make_block<
//         kodo::seed_rlnc_encoder<fifi::binary16>,
//         kodo::seed_rlnc_decoder<fifi::binary16> >(setup));

// //    run("rs_2",
// //        make_block<
// //        kodo::rs_encoder<fifi::binary>,
// //        kodo::rs_decoder<fifi::binary> >(setup));

// //    run("rs_8",
// //        make_block<
// //        kodo::rs_encoder<fifi::binary8>,
// //        kodo::rs_decoder<fifi::binary8> >(setup));

// //    run("rs_16",
// //        make_block<
// //        kodo::rs_encoder<fifi::binary16>,
// //        kodo::rs_decoder<fifi::binary16> >(setup));


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

