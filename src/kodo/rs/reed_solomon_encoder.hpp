// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_REED_SOLOMON_ENCODER_HPP
#define KODO_RS_REED_SOLOMON_ENCODER_HPP

#include <stdint.h>

#include <sak/convert_endian.hpp>

namespace kodo
{
    /// Basic Reed-Solomon encoder
    template<template <class> class GeneratorMatrix, class SuperCoder>
    class reed_solomon_encoder : public SuperCoder
    {
    public:

        /// The field type
        typedef typename SuperCoder::field_type field_type;

        /// The value type
        typedef typename field_type::value_type value_type;

        /// The generator matrix type
        typedef GeneratorMatrix<field_type> generator_matrix;

        /// Pointer to coder produced by the factories
        typedef typename SuperCoder::pointer pointer;

    public:

        /// The factory layer associated with this coder. Maintains
        /// the block generator needed for the encoding vectors.
        class factory : public SuperCoder::factory
        {
        private:

            /// Access to the finite field implementation used stored in the
            /// finite_field_math layer
            using SuperCoder::factory::m_field;

        public:

            /// @copydoc final_coder_factory::factory::factory()
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                {
                    // A Reed-Solomon code cannot support more symbols
                    /// than 2^m - 1 where m is the length of the finite
                    // field elements in bytes
                    assert(max_symbols < field_type::order);
                }

            /// @copydoc final_coder_factory::factory::build()
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder =
                        SuperCoder::factory::build(symbols, symbol_size);

                    if(m_cache.find(symbols) == m_cache.end())
                    {
                        m_cache[symbols] =
                            boost::make_shared<generator_matrix>(
                                false, symbols, m_field);
                    }

                    coder->m_matrix = m_cache[symbols];
                    return coder;
                }

            /// @return the required payload buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    return sizeof(value_type);
                }

        private:

            /// map for blocks
            std::map<uint32_t, boost::shared_ptr<generator_matrix> > m_cache;
        };

    public:

        /// @copydoc final_coder_factory::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_count = 0;
            }

        /// Iterates over the symbols stored in the encoding symbol id parts
        /// of the payload id, and calls the encode_symbol function.
        /// @param symbol_data buffer where the encoded symbols should be stored
        /// @param symbol_id buffer where the symbol id is stored
        /// @return the number of bytes used to store the symbol id
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);
                assert(m_count < field_type::order);

                // Store the encoding id
                sak::big_endian::put<value_type>(m_count, symbol_id);

                // Get the corresponding coding coefficients
                const uint8_t *encoding_coefficients =
                    m_matrix->coefficients(m_count);

                SuperCoder::encode(symbol_data, encoding_coefficients);

                // Now increment the count so that we use other
                // coefficients next time
                ++m_count;

                return sizeof(value_type);
            }

        /// Resets the internal count - which will make the encoder start from
        /// the beginning of th generator matrix
        void reset_count()
            {
                m_count = 0;
            }

        /// Returns the coder 'count' i.e. the number of symbols already encoded
        uint32_t count() const
            {
                return m_count;
            }

        /// @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return sizeof(value_type);
            }


    protected:

        /// Keeping track of the number of packets sent
        uint32_t m_count;

        /// The generator matrix
        boost::shared_ptr<generator_matrix> m_matrix;
    };
}

#endif

