// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <kodo/rlnc/full_vector_codes.h>
#include <kodo/rlnc/seed_codes.h>

#include <kodo/rs/reed_solomon_codes.h>

#include <sak/code_warmup.h>
#include <boost/make_shared.hpp>

/// The test setup parameters
struct test_setup
{
    /// The number of symbols in a block
    uint32_t m_symbols;

    /// The size of a symbol in bytes
    uint32_t m_symbol_size;

    /// The extra symbols to generate for a block
    /// Useful for e.g. rate-less codes where full
    /// decoding is not guaranteed after receiving
    /// all symbols once.
    uint32_t m_extra_symbols;

    /// The target time a benchmark should run in seconds
    double m_target_time;
};


/// A test block represents an encoder and decoder pair
template<class Encoder, class Decoder>
struct test_block
{

    /// Test block consisting of an encoder and decoder
    /// @param setup the test_setup
    /// @param encoder
    test_block(const test_setup &setup,
               typename Encoder::pointer encoder,
               typename Decoder::pointer decoder)
        : m_setup(setup),
          m_encoder(encoder),
          m_decoder(decoder)
        {
            // Prepare the data to be encoded
            m_data_in.resize(m_encoder->block_size());

            // Just for fun - fill the data with random data
            kodo::random_uniform<uint8_t> fill_data;
            fill_data.generate(&m_data_in[0], m_data_in.size());

            // Prepare storage to the encoded payloads
            assert(m_encoder->symbols() == setup.m_symbols);
            uint32_t payload_count = setup.m_symbols + setup.m_extra_symbols;

            m_payloads.resize(payload_count);
            for(uint32_t i = 0; i < payload_count; ++i)
            {
                m_payloads[i].resize( m_encoder->payload_size() );
            }

            m_temp_payload.resize( m_encoder->payload_size() );

        }

    template<class E>
    typename boost::enable_if_c<kodo::is_systematic_encoder<E>::value>::type
    systematic_off()
        {
            m_encoder->systematic_off();
        }

    template<class E>
    typename boost::enable_if_c<!kodo::is_systematic_encoder<E>::value>::type
    systematic_off()
        {
            // Do nothing it is not a systematic encoder
        }

    /// Run the encoder
    /// @return the number of symbols encoded
    uint32_t run_encode()
        {
            m_encoder->initialize(m_setup.m_symbols, m_setup.m_symbol_size);

            systematic_off<Encoder>();

            kodo::set_symbols(kodo::storage(m_data_in), m_encoder);

            uint32_t payload_count = m_payloads.size();

            for(uint32_t i = 0; i < payload_count; ++i)
            {
                std::vector<uint8_t> &payload = m_payloads[i];
                m_encoder->encode(&payload[0]);
            }

            return payload_count;
        }

    /// Run the decoder
    /// @return the number of symbols decoded
    uint32_t run_decode()
        {
            m_decoder->initialize(m_setup.m_symbols, m_setup.m_symbol_size);

            uint32_t payload_count = m_payloads.size();

            for(uint32_t i = 0; i < payload_count; ++i)
            {
                std::copy(m_payloads[i].begin(), m_payloads[i].end(),
                          m_temp_payload.begin());

                m_decoder->decode(&m_temp_payload[0]);

                if(m_decoder->is_complete())
                {
                    return i;
                }
            }

            return payload_count;
        }

    /// Test setup
    test_setup m_setup;

    /// The encoder to use
    typename Encoder::pointer m_encoder;

    /// The encoder to use
    typename Decoder::pointer m_decoder;

    /// The data encoded
    std::vector<uint8_t> m_data_in;

    /// Temporary payload to not destroy the already encoded payloads
    /// when decoding
    std::vector<uint8_t> m_temp_payload;

    /// Storage for encoded symbols
    std::vector< std::vector<uint8_t> > m_payloads;

};


/// Build a new test block
template<class Encoder, class Decoder>
boost::shared_ptr< test_block<Encoder,Decoder> >
make_block(const test_setup &setup,
           typename Encoder::factory &encoder_factory,
           typename Decoder::factory &decoder_factory)
{

    typename Encoder::pointer encoder =
        encoder_factory.build(setup.m_symbols, setup.m_symbol_size);

    typename Decoder::pointer decoder =
        decoder_factory.build(setup.m_symbols, setup.m_symbol_size);

    return boost::make_shared< test_block<Encoder, Decoder> >(
        setup, encoder, decoder);

}


/// Build a new test block
template<class Encoder, class Decoder>
boost::shared_ptr< test_block<Encoder,Decoder> >
make_block(const test_setup &setup)
{
    typename Encoder::factory encoder_factory(setup.m_symbols,
                                              setup.m_symbol_size);

    typename Decoder::factory decoder_factory(setup.m_symbols,
                                              setup.m_symbol_size);

    typename Encoder::pointer encoder =
        encoder_factory.build(setup.m_symbols, setup.m_symbol_size);

    typename Decoder::pointer decoder =
        decoder_factory.build(setup.m_symbols, setup.m_symbol_size);

    return boost::make_shared< test_block<Encoder, Decoder> >(
        setup, encoder, decoder);
}


/// Build a new test block
template<class Encoder, class Decoder>
boost::shared_ptr< test_block<Encoder,Decoder> >
make_block(const test_setup &setup,
           typename Encoder::pointer &encoder,
           typename Decoder::pointer &decoder)
{
    return boost::make_shared< test_block<Encoder, Decoder> >(
        setup, encoder, decoder);
}


template<class TestBlockPtr>//Encoder, class Decoder>
void run(const std::string &name, //const test_setup &setup,
         TestBlockPtr block)
         //boost::shared_ptr< test_block<Encoder,Decoder> > block)
{

    std::cout << "running " << name << " symbols = "
              << block->m_setup.m_symbols << std::endl;

    // Estimate speed of encoder / decoder
    // Warm up
    sak::code_warmup warmup;

    while(!warmup.done())
    {
        block->run_encode();
        warmup.next_iteration();
    }

    // The number of iterations needed to run the target time seconds
    // we expect the encoder to always be faster than the decoder, which means
    // it will always require more iterations.
    uint64_t iterations = warmup.iterations(block->m_setup.m_target_time);

    std::cout << "needed encode iterations " << iterations << std::endl;


    {
        boost::timer::cpu_timer timer;
        timer.start();

        uint64_t symbols_consumed = 0;

        for(uint32_t i = 0; i < iterations; ++i)
            symbols_consumed += block->run_encode();

        timer.stop();

        long double total_sec = sak::seconds_elapsed(timer);

        // Amount of data processed
        long double bytes = static_cast<long double>(
            block->m_setup.m_symbol_size * symbols_consumed);

        long double megs = bytes / 1000000.0;
        long double megs_per_second = megs / total_sec;

        std::cout << "Encode symbols consumed " << symbols_consumed << std::endl;
        std::cout << "Encode test time " << total_sec << " [s]" << std::endl;
        std::cout << "Encode MB/s = " << megs_per_second << std::endl;
    }

    {
        boost::timer::cpu_timer timer;
        timer.start();

        uint64_t symbols_consumed = 0;

        for(uint32_t i = 0; i < iterations; ++i)
            symbols_consumed += block->run_decode();

        timer.stop();

        long double total_sec = sak::seconds_elapsed(timer);

        // Amount of data processed
        long double bytes = static_cast<long double>(
            block->m_setup.m_symbol_size * symbols_consumed);

        long double megs = bytes / 1000000.0;
        long double megs_per_second = megs / total_sec;

        std::cout << "Decode symbols consumed " << symbols_consumed << std::endl;
        std::cout << "Decode test time " << total_sec << " [s]" << std::endl;
        std::cout << "Decode MB/s = " << megs_per_second << std::endl;
    }
}


void benchmark(const test_setup &setup)
{
    run("full_rlnc_2",
        make_block<
        kodo::full_rlnc_encoder<fifi::binary>,
        kodo::full_rlnc_decoder<fifi::binary> >(setup));

    run("full_rlnc_8",
        make_block<
        kodo::full_rlnc_encoder<fifi::binary8>,
        kodo::full_rlnc_decoder<fifi::binary8> >(setup));

    run("full_rlnc_16",
        make_block<
        kodo::full_rlnc_encoder<fifi::binary16>,
        kodo::full_rlnc_decoder<fifi::binary16> >(setup));

    run("seed_rlnc_2",
        make_block<
        kodo::seed_rlnc_encoder<fifi::binary>,
        kodo::seed_rlnc_decoder<fifi::binary> >(setup));

    run("seed_rlnc_8",
        make_block<
        kodo::seed_rlnc_encoder<fifi::binary8>,
        kodo::seed_rlnc_decoder<fifi::binary8> >(setup));

    run("seed_rlnc_16",
        make_block<
        kodo::seed_rlnc_encoder<fifi::binary16>,
        kodo::seed_rlnc_decoder<fifi::binary16> >(setup));

    run("rs_2",
        make_block<
        kodo::rs_encoder<fifi::binary>,
        kodo::rs_decoder<fifi::binary> >(setup));

    run("rs_8",
        make_block<
        kodo::rs_encoder<fifi::binary8>,
        kodo::rs_decoder<fifi::binary8> >(setup));

    run("rs_16",
        make_block<
        kodo::rs_encoder<fifi::binary16>,
        kodo::rs_decoder<fifi::binary16> >(setup));

    // If custom initialization is needed the following approach may
    // be used.
    // {
    //     typedef kodo::full_rlnc_encoder<fifi::binary> encoder_type;
    //     typedef kodo::full_rlnc_decoder<fifi::binary> decoder_type;
    //
    //     encoder_type::factory encoder_factory(setup.m_symbols,
    //                                           setup.m_symbol_size);
    //
    //     decoder_type::factory decoder_factory(setup.m_symbols,
    //                                           setup.m_symbol_size);
    //
    //     for(uint32_t i = 0; i < 10; ++i)
    //     {
    //
    //         encoder_type::pointer encoder =
    //             encoder_factory.build(setup.m_symbols, setup.m_symbol_size);
    //
    //         encoder->set_some_option(i);
    //
    //         decoder_type::pointer decoder =
    //             decoder_factory.build(setup.m_symbols, setup.m_symbol_size);
    //
    //         run("full_rlnc_8",
    //             make_block<encoder_type, decoder_type>(setup, encoder, decoder));
    //     }
    // }
}

int main()
{

    {
        test_setup setup;
        setup.m_symbols = 16;
        setup.m_symbol_size = 1600;
        setup.m_extra_symbols = 16;
        setup.m_target_time = 5.0;

        benchmark(setup);
    }

    {
        test_setup setup;
        setup.m_symbols = 32;
        setup.m_symbol_size = 1600;
        setup.m_extra_symbols = 16;
        setup.m_target_time = 5.0;

        benchmark(setup);
    }

    return 0;
}
