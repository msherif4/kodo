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
    symbols.push_back(1024);
    return symbols;
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


            for(uint32_t i = 0; i < symbols.size(); ++i)
            {
                gauge::config_set cs;
                cs.set_value<uint32_t>("symbols", symbols[i]);
            }
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


    std::string unit_text() const
        {
            return "vector/s";
        }

    void setup()
        {
            gauge::config_set cs = get_current_configuration();

            uint32_t symbols = cs.get_value<uint32_t>("symbols");
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
    void run_test()
        {
            gauge::config_set cs = get_current_configuration();

            uint32_t symbols = cs.get_value<uint32_t>("symbols");

            // The clock is running
            RUN{
                // Do something :)
            }
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

