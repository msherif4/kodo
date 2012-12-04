// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_LINEAR_BLOCK_VECTOR_GENERATOR_HPP
#define KODO_LINEAR_BLOCK_VECTOR_GENERATOR_HPP

#include <stdint.h>

#include "linear_block_vector.hpp"

namespace kodo
{
    /// A linear block vector generator - this generator
    /// produces linear block vector coding coefficients.
    /// See the layout of a linear_block_vector object in
    /// the corresponding header file.
    template<template <class> class GeneratorBlock, class SuperCoder>
    class linear_block_vector_generator : public SuperCoder
    {
    public:

        /// The field type
        typedef typename SuperCoder::field_type field_type;

        /// The value type
        typedef typename field_type::value_type value_type;

        /// The encoding vector
        typedef linear_block_vector<field_type> vector_type;

        /// The vector generator type
        typedef GeneratorBlock<value_type> generator_block;

        /// Pointer to coder produced by the factories
        typedef typename SuperCoder::pointer pointer;

    public:

        /// The factory layer associated with this coder.
        /// Maintains the block generator needed for the encoding
        /// vectors.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc final_coder_factory::factory::factory()
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @copydoc final_coder_factory::factory::build()
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder =
                        SuperCoder::factory::build(symbols, symbol_size);

                    uint32_t vector_length = vector_type::length( symbols );

                    typename generator_block::pointer block =
                        m_generator_factory.build( vector_length );

                    coder->m_generator = block;

                    return coder;
                }

            /// Set the see used by the generator block
            void set_seed(uint32_t seed)
                {
                    m_generator_factory.set_seed(seed);
                }

        private:

            typename generator_block::factory m_generator_factory;
        };


    public:

        /// Generates the coefficients for a linear block vector
        /// into the requested buffer
        void generate(uint32_t index, value_type *vector_buffer)
            {
                assert(vector_buffer != 0);

                // Fill the encoding vector
                assert(m_generator);

                m_generator->fill(index, vector_buffer);
            }

    protected:

        /// The linear block vector generator
        typename generator_block::pointer m_generator;
    };
}

#endif

