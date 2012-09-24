// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_REED_SOLOMON_DECODER_HPP
#define KODO_RS_REED_SOLOMON_DECODER_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include "../linear_block_decoder.hpp"

namespace kodo
{
    /// Basic Reed-Solomon decoder
    template<template <class> class GeneratorMatrix, class SuperCoder>
    class reed_solomon_decoder : public SuperCoder
    {
    public:

        /// The field we use
        typedef typename SuperCoder::field_type field_type;

        /// The value_type used to store the field elements
        typedef typename field_type::value_type value_type;

        /// The generator matrix type
        typedef GeneratorMatrix<field_type> generator_matrix;

        /// Pointer to coder produced by the factories
        typedef typename SuperCoder::pointer pointer;

    public:

        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        private:

            /// Access to the finite field implementation used
            /// stored in the finite_field_math layer
            using SuperCoder::factory::m_field;

        public:

            /// @see final_coder_factory::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                {
                    /// A Reed-Solomon code cannot support more symbols
                    /// than 2^m - 1 where m is the length of the finite
                    /// field elements in bytes
                    assert(max_symbols < field_type::order);
                }

            /// @see final_coder_factory::factory(..)
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

            /// @return the required symbol id buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    return sizeof(value_type);
                }

        private:

            /// map for blocks
            std::map<uint32_t, boost::shared_ptr<generator_matrix> > m_cache;
        };

    public:

        /// @see final_coder_factory(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                m_coefficients.resize(max_symbols);
            }

        /// The decode function which consumes the encoded symbol
        /// and symbol id
        /// @param symbol_data, the encoded symbol
        /// @param symbol_id, identifies how the symbol was encoded
        void decode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                value_type id = sak::big_endian::get<value_type>(symbol_id);

                /// Fetch the coding coefficients from the generator matrix
                assert(m_matrix);

                const value_type *coefficients = m_matrix->coefficients(id);

                /// Copy to a local buffer
                std::copy(coefficients, coefficients + m_matrix->symbols(),
                          &m_coefficients[0]);

                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                SuperCoder::decode_with_vector(&m_coefficients[0], symbol);
            }

        /// @return the required buffer needed for the symbol id
        uint32_t symbol_id_size() const
            {
                return sizeof(value_type);
            }

    protected:

        /// The temp coding coefficients buffer
        std::vector<value_type> m_coefficients;

        /// The generator matrix
        boost::shared_ptr<generator_matrix> m_matrix;
    };
}

#endif

