// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_REED_SOLOMON_GENERATOR_HPP
#define KODO_RS_REED_SOLOMON_GENERATOR_HPP

#include <cstdint>

namespace kodo
{

    /// @todo delete this
    /// Basic Reed-Solomon encoder
    template<template <class> class GeneratorMatrix, class SuperCoder>
    class reed_solomon_generator : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

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
            uint32_t max_id_size() const
                {
                    return sizeof(value_type);
                }

        private:

            /// map for blocks
            std::map<uint32_t, boost::shared_ptr<generator_matrix> > m_cache;
        };

    public:

        /// @copydoc layer::generate(uint8_t*)
        void generate(uint8_t *symbol_coefficients)
            {

            }

    protected:

        /// The generator matrix
        boost::shared_ptr<generator_matrix> m_matrix;
    };
}

#endif

