// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_proxy_layer.cpp Unit tests for the proxy layer class

#include <cstdint>

#include <gtest/gtest.h>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/proxy_layer.hpp>

#include "basic_api_test_helper.hpp"

template<class SuperLayer>
class dummy_layer : public SuperLayer
{
public:

    class factory : public SuperLayer::factory
    {
    public:

        factory(uint32_t max_symbols, uint32_t max_symbol_size)
            : SuperLayer::factory(max_symbols, max_symbol_size)
        { }

    };

    void initialize(factory& /*the_factory*/)
    { }

    void construct(factory& /*the_factory*/)
    { }

};

/// Dummy class needed to terminate proxy stack
template<class Coder>
class proxy_test :
    public dummy_layer<
    kodo::proxy_layer<proxy_test<Coder>, Coder > >
{ };


/// Tests:
///  - That the proxy layer functions are correctly forwarded to the
///    main codec stack.
template<class Coder, class Proxy>
struct api_coder
{

    typedef typename Coder::factory coder_factory;
    typedef typename Proxy::factory proxy_factory;


    api_coder(uint32_t max_symbols, uint32_t max_symbol_size)
        : m_coder_factory(max_symbols, max_symbol_size),
          m_proxy_factory(max_symbols, max_symbol_size)
    {
        m_proxy_factory.set_factory_proxy(&m_coder_factory);
    }

    void run_general_api()
    {
        auto coder = m_coder_factory.build();

        m_proxy_factory.set_stack_proxy(coder.get());

        EXPECT_EQ(m_coder_factory.max_symbols(),
                  m_proxy_factory.max_symbols());

        EXPECT_EQ(m_coder_factory.max_symbol_size(),
                  m_proxy_factory.max_symbol_size());

        EXPECT_EQ(m_coder_factory.max_block_size(),
                  m_proxy_factory.max_block_size());

        EXPECT_EQ(m_coder_factory.max_header_size(),
                  m_proxy_factory.max_header_size());

        EXPECT_EQ(m_coder_factory.max_id_size(),
                  m_proxy_factory.max_id_size());

        EXPECT_EQ(m_coder_factory.max_payload_size(),
                  m_proxy_factory.max_payload_size());

        EXPECT_EQ(m_coder_factory.max_coefficients_size(),
                  m_proxy_factory.max_coefficients_size());

        EXPECT_EQ(m_coder_factory.symbols(),
                  m_proxy_factory.symbols());

        EXPECT_EQ(m_coder_factory.symbol_size(),
                  m_proxy_factory.symbol_size());

        auto proxy = m_proxy_factory.build();

        //------------------------------------------------------------------
        // SYMBOL STORAGE API
        //------------------------------------------------------------------

        EXPECT_EQ(coder->block_size(), proxy->block_size());

        std::vector<uint8_t> data_in(coder->block_size(), 'x');
        std::vector<uint8_t> data_out(coder->block_size(), 'y');

        coder->set_symbols(sak::storage(data_in));
        proxy->copy_symbols(sak::storage(data_out));

        EXPECT_TRUE(sak::equal(sak::storage(data_in), sak::storage(data_out)));

        EXPECT_EQ(coder->symbols(), proxy->symbols());
        EXPECT_EQ(coder->symbol_size(), proxy->symbol_size());
        EXPECT_EQ(coder->symbol_length(), proxy->symbol_length());

        std::vector<uint8_t> symbol_one(coder->symbol_size(), 'x');
        std::vector<uint8_t> symbol_two(coder->symbol_size(), 'y');

        uint32_t index = rand() % coder->symbols();
        coder->set_symbol(index, sak::storage(symbol_one));
        coder->copy_symbol(index, sak::storage(symbol_two));

        EXPECT_TRUE(sak::equal(sak::storage(symbol_one),
                               sak::storage(symbol_two)));

        EXPECT_EQ(coder->symbol(index), proxy->symbol(index));
        EXPECT_EQ(coder->symbol_value(index), proxy->symbol_value(index));

        EXPECT_EQ(coder->symbols_available(), proxy->symbols_available());
        EXPECT_EQ(coder->symbols_initialized(), proxy->symbols_initialized());

        EXPECT_EQ(coder->is_symbols_initialized(),
                  proxy->is_symbols_initialized());

        EXPECT_EQ(coder->is_symbols_available(),
                  proxy->is_symbols_available());

        EXPECT_EQ(coder->is_symbol_initialized(index),
                  proxy->is_symbol_initialized(index));

        EXPECT_EQ(coder->is_symbol_available(index),
                  proxy->is_symbol_available(index));


        //------------------------------------------------------------------
        // FINITE FIELD API
        //------------------------------------------------------------------

        typedef typename Coder::value_type value_type;

        std::vector<value_type> src(coder->symbol_length(), 'a');

        std::vector<value_type> dest_coder(coder->symbol_length(), 'b');
        std::vector<value_type> dest_proxy(coder->symbol_length(), 'b');

        value_type coefficient = 'c';

        coder->multiply(&dest_coder[0], coefficient,
                          coder->symbol_length());

        proxy->multiply(&dest_proxy[0], coefficient,
                        proxy->symbol_length());

        EXPECT_TRUE(sak::equal(sak::storage(dest_coder),
                               sak::storage(dest_proxy)));

        coder->multiply_add(&dest_coder[0], &src[0], coefficient,
                          coder->symbol_length());

        proxy->multiply_add(&dest_proxy[0], &src[0], coefficient,
                        proxy->symbol_length());

        EXPECT_TRUE(sak::equal(sak::storage(dest_coder),
                               sak::storage(dest_proxy)));

        coder->add(&dest_coder[0], &src[0],
                     coder->symbol_length());

        proxy->add(&dest_proxy[0], &src[0],
                        proxy->symbol_length());

        EXPECT_TRUE(sak::equal(sak::storage(dest_coder),
                               sak::storage(dest_proxy)));

        coder->multiply_subtract(&dest_coder[0], &src[0], coefficient,
                          coder->symbol_length());

        proxy->multiply_subtract(&dest_proxy[0], &src[0], coefficient,
                        proxy->symbol_length());

        EXPECT_TRUE(sak::equal(sak::storage(dest_coder),
                               sak::storage(dest_proxy)));

        coder->subtract(&dest_coder[0], &src[0],
                         coder->symbol_length());

        proxy->subtract(&dest_proxy[0], &src[0],
                        proxy->symbol_length());

        EXPECT_TRUE(sak::equal(sak::storage(dest_coder),
                               sak::storage(dest_proxy)));

        value_type invert_coder = coder->invert('b');
        value_type invert_proxy = proxy->invert('b');

        EXPECT_EQ(invert_coder, invert_proxy);
    }

    void run_encoder_api()
    {
        auto coder = m_coder_factory.build();
        m_proxy_factory.set_stack_proxy(coder.get());

        auto proxy = m_proxy_factory.build();

        std::vector<uint8_t> data(coder->block_size());
        coder->set_symbols(sak::storage(data));

        std::vector<uint8_t> coefficients(coder->coefficients_size(), '3');

        std::vector<uint8_t> coder_symbol(coder->symbol_size(), 'z');
        std::vector<uint8_t> proxy_symbol(proxy->symbol_size(), 'z');

        coder->encode_symbol(&coder_symbol[0], &coefficients[0]);
        proxy->encode_symbol(&proxy_symbol[0], &coefficients[0]);

        EXPECT_TRUE(sak::equal(sak::storage(coder_symbol),
                               sak::storage(proxy_symbol)));

        uint32_t index = rand() % coder->symbols();

        coder->encode_symbol(&coder_symbol[0], index);
        proxy->encode_symbol(&proxy_symbol[0], index);

        EXPECT_TRUE(sak::equal(sak::storage(coder_symbol),
                               sak::storage(proxy_symbol)));

        EXPECT_EQ(coder->rank(), proxy->rank());

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            EXPECT_EQ(coder->symbol_pivot(i), proxy->symbol_pivot(i));
        }

    }

    void run_decoder_api()
    {
        auto coder = m_coder_factory.build();
        m_proxy_factory.set_stack_proxy(coder.get());

        auto proxy = m_proxy_factory.build();

        std::vector<uint8_t> data(coder->block_size());
        coder->set_symbols(sak::storage(data));

        std::vector<uint8_t> coefficients_coder(coder->coefficients_size(), '3');
        std::vector<uint8_t> coefficients_proxy(coder->coefficients_size(), '3');

        coefficients_coder[0] = 1;
        coefficients_proxy[0] = 0;

        std::vector<uint8_t> coder_symbol(coder->symbol_size(), 'z');
        std::vector<uint8_t> proxy_symbol(proxy->symbol_size(), 'z');

        coder->decode_symbol(&coder_symbol[0], &coefficients_coder[0]);
        proxy->decode_symbol(&proxy_symbol[0], &coefficients_proxy[0]);

        EXPECT_EQ(coder->rank(), proxy->rank());
        EXPECT_EQ(coder->rank(), 2U);

        coder->decode_symbol(&coder_symbol[0], 2U);
        proxy->decode_symbol(&proxy_symbol[0], 2U);

        EXPECT_EQ(coder->rank(), proxy->rank());
        EXPECT_EQ(coder->rank(), 3U);

        EXPECT_FALSE(coder->is_complete());
        EXPECT_FALSE(proxy->is_complete());

        for(uint32_t i = 0; i < coder->symbols(); ++i)
        {
            EXPECT_EQ(coder->symbol_pivot(i), proxy->symbol_pivot(i));
        }
    }


    void run_coefficients_api()
    {
        auto coder = m_coder_factory.build();
        m_proxy_factory.set_stack_proxy(coder.get());

        auto proxy = m_proxy_factory.build();

        EXPECT_EQ(coder->coefficients_size(),
                  proxy->coefficients_size());

        EXPECT_EQ(coder->coefficients_length(),
                  proxy->coefficients_length());

        uint32_t index = rand() % coder->symbols();

        EXPECT_EQ(coder->coefficients_value(index),
                  proxy->coefficients_value(index));

        EXPECT_EQ(coder->coefficients(index),
                  proxy->coefficients(index));
    }

private:

    // The factory of the coder
    coder_factory m_coder_factory;

    // The factory of the proxy
    proxy_factory m_proxy_factory;

};


/// Run the tests typical coefficients stack
TEST(TestProxyLayer, encoder)
{
    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // The codec stack
    typedef kodo::full_rlnc_encoder<fifi::binary8> encoder;
    typedef proxy_test<encoder> proxy;


    // API tests:
    typedef api_coder<encoder,proxy> test;

    test t(symbols, symbol_size);
    t.run_general_api();
    t.run_encoder_api();
}

/// Run the tests typical coefficients stack
TEST(TestProxyLayer, decoder)
{
    // Should be at least 4, since we assume this in the
    // run_decoder_api() function
    uint32_t symbols = std::max(4U, rand_symbols());
    uint32_t symbol_size = rand_symbol_size();

    // The codec stack
    typedef kodo::full_rlnc_decoder<fifi::binary8> decoder;
    typedef proxy_test<decoder> proxy;


    // API tests:
    typedef api_coder<decoder,proxy> test;

    test t(symbols, symbol_size);
    t.run_general_api();
    t.run_decoder_api();
    t.run_coefficients_api();
}


