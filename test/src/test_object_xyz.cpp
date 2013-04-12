// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_object_xyz.cpp Unit tests for object encoder and decoders

#include <ctime>

#include <gtest/gtest.h>

#include <kodo/object_decoder.hpp>
#include <kodo/object_encoder.hpp>
#include <kodo/rfc5052_partitioning_scheme.hpp>

#include "basic_api_test_helper.hpp"

/// Dummy API to replace an actual encoder or decoder stack
struct dummy_coder
{
public:

    typedef boost::shared_ptr<dummy_coder> pointer;

    class factory
    {
    public:

        /// @copydoc layer::factory::factory(uint32_t,uint32_t)
        factory(uint32_t max_symbols, uint32_t max_symbol_size)
            : m_max_symbols(max_symbols),
              m_max_symbol_size(max_symbol_size),
              m_symbols(max_symbols),
              m_symbol_size(max_symbol_size)
        {}

        /// @copydoc layer::factory::build()
        pointer build()
        {
            return boost::make_shared<dummy_coder>(m_symbols, m_symbol_size);
        }

        /// @copydoc layer::factory::max_symbols() const
        uint32_t max_symbols() const
        {
            return m_max_symbols;
        }

        /// @copydoc layer::factory::max_symbol_size() const
        uint32_t max_symbol_size() const
        {
            return m_max_symbol_size;
        }

        /// @copydoc layer::factory::symbols() const;
        uint32_t symbols() const
        {
            return m_symbols;
        }

        /// @copydoc layer::factory::symbol_size() const;
        uint32_t symbol_size() const
        {
            return m_symbol_size;
        }

        /// @copydoc layer::factory::set_symbols(uint32_t)
        void set_symbols(uint32_t symbols)
        {
            assert(symbols > 0);
            assert(symbols <= m_max_symbols);

            m_symbols = symbols;
        }

        /// @copydoc layer::factory::set_symbol_size(uint32_t)
        void set_symbol_size(uint32_t symbol_size)
        {
            assert(symbol_size > 0);
            assert(symbol_size <= m_max_symbol_size);

            m_symbol_size = symbol_size;
        }

        uint32_t m_max_symbols;
        uint32_t m_max_symbol_size;
        uint32_t m_symbols;
        uint32_t m_symbol_size;

    };

    dummy_coder(uint32_t symbols, uint32_t symbol_size)
        : m_symbols(symbols),
          m_symbol_size(symbol_size)
        {}

    /// @copydoc layer::set_bytes_used(uint32_t)
    void set_bytes_used(uint32_t bytes_used)
        {
            m_bytes_used = bytes_used;
        }

    /// @copydoc layer::bytes_used() const
    uint32_t bytes_used() const
        {
            return m_bytes_used;
        }

    /// @copydoc layer::block_size() const
    uint32_t block_size() const
        {
            return m_symbols * m_symbol_size;
        }

    /// @copydoc layer::symbols() const
    uint32_t symbols() const
        {
            return m_symbols;
        }

    /// @copydoc layer::symbol_size() const
    uint32_t symbol_size() const
        {
            return m_symbol_size;
        }

    /// Test function need to test whether the encoder
    /// is initialized with data from the right offset
    /// @param byte_offset The offset in bytes
    void set_byte_offset(uint32_t byte_offset)
        {
            m_byte_offset = byte_offset;
        }

    /// Test function returning the byte offset
    /// @return The byte offset of the encoder
    uint32_t byte_offset() const
        {
            return m_byte_offset;
        }

    uint32_t m_symbols;
    uint32_t m_symbol_size;
    uint32_t m_byte_offset;
    uint32_t m_bytes_used;

};

/// Dummy API to replace an ObjectData compatible class
class dummy_object_data
{
public:

    typedef dummy_coder::pointer pointer;

    dummy_object_data(uint32_t size)
        : m_size(size)
        {}


    /// @copydoc object_data::read(pointer, uint32_t, uint32_t)
    void read(pointer &coder, uint32_t offset, uint32_t size)
        {
            coder->set_bytes_used(size);
            coder->set_byte_offset(offset);
        }

    /// @copydoc object_data::size() const
    uint32_t size() const
        {
            return m_size;
        }

private:

    uint32_t m_size;

};

/// Runs the actual test with the object_encoder and object_decoder
template<
    class Encoder,
    class Decoder,
    class Partitioning,
    class ObjectData
    >
void invoke_object(uint32_t max_symbols,
                   uint32_t max_symbol_size,
                   uint32_t multiplier)
{

    typedef kodo::object_encoder<ObjectData, Encoder, Partitioning>
        object_encoder;

    typedef kodo::object_decoder<Decoder, Partitioning>
        object_decoder;

    typedef Partitioning partitioning;

    uint32_t object_size = rand_nonzero(max_symbols*max_symbol_size*multiplier);

    dummy_object_data data(object_size);
    partitioning p(max_symbols, max_symbol_size, object_size);

    typename Encoder::factory encoder_factory(max_symbols, max_symbol_size);
    typename Decoder::factory decoder_factory(max_symbols, max_symbol_size);

    object_encoder obj_encoder(encoder_factory, data);
    object_decoder obj_decoder(decoder_factory, obj_encoder.object_size());

    EXPECT_EQ(p.blocks(), obj_encoder.encoders());
    EXPECT_EQ(p.blocks(), obj_decoder.decoders());

    EXPECT_TRUE(obj_encoder.encoders() == obj_decoder.decoders());

    EXPECT_EQ(p.object_size(), obj_decoder.object_size());
    EXPECT_EQ(p.object_size(), obj_decoder.object_size());

    for(uint32_t i = 0; i < obj_encoder.encoders(); ++i)
    {
        typename Encoder::pointer encoder = obj_encoder.build(i);
        typename Decoder::pointer decoder = obj_decoder.build(i);

        EXPECT_EQ(p.symbols(i), encoder->symbols());
        EXPECT_EQ(p.symbols(i), decoder->symbols());

        EXPECT_EQ(p.symbol_size(i), encoder->symbol_size());
        EXPECT_EQ(p.symbol_size(i), decoder->symbol_size());

        EXPECT_EQ(p.block_size(i), encoder->block_size());
        EXPECT_EQ(p.block_size(i), decoder->block_size());

        EXPECT_EQ(p.byte_offset(i), encoder->byte_offset());

        EXPECT_EQ(p.bytes_used(i), encoder->bytes_used());
        EXPECT_EQ(p.bytes_used(i), decoder->bytes_used());
    }

}

void test_object_coders(uint32_t symbols,
                        uint32_t symbol_size,
                        uint32_t multiplier)
{
    invoke_object<
        dummy_coder,
        dummy_coder,
        kodo::rfc5052_partitioning_scheme,
        dummy_object_data>(symbols, symbol_size, multiplier);
}

/// Tests:
///  - object_encoder::encoders() const
///  - object_decoder::decoders() const
///  - object_encoder::build(uint32_t)
///  - object_decoder::build(uint32_t)
TEST(TestObjectCoder, construct_and_invoke_the_basic_api)
{
    test_object_coders(32, 1600, 2);
    test_object_coders(1, 1600, 2);

    uint32_t symbols = rand_symbols();
    uint32_t symbol_size = rand_symbol_size();

    // Multiplies the data to be encoded so that the object encoder
    // is expected to contain multiplier encoders.
    uint32_t multiplier = rand_nonzero(10);

    test_object_coders(symbols, symbol_size, multiplier);
}

