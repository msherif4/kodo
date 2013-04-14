// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <fifi/arithmetics.hpp>

#include "vandermonde_matrix.hpp"

namespace kodo
{

    /// @brief Computes a Vandermonde matrix to generate the coding
    ///        coefficients. The matrix code is
    template<class SuperCoder>
    class systematic_vandermonde_matrix_base : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// The generator matrix type
        typedef typename SuperCoder::generator_matrix generator_matrix;

    public:

        /// The factory layer associated with this coder. Maintains
        /// the block generator needed for the encoding vectors.
        class factory : public SuperCoder::factory
        {
        protected:

            /// Access to the finite field implementation used stored in
            /// the finite_field_math layer
            using SuperCoder::factory::m_field;

        public:

            /// @copydoc layer::factory::factory(uint32_t, uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size);

            /// Constructs systematic Vandermonde matrix.
            /// @param symbols The number of source symbols to encode
            /// @return The Vandermonde matrix
            boost::shared_ptr<generator_matrix> construct_matrix(
                uint32_t symbols);

        };

    };

    template<class SuperCoder>
    systematic_vandermonde_matrix_base<SuperCoder>::factory::factory(
        uint32_t max_symbols, uint32_t max_symbol_size)
        : SuperCoder::factory(max_symbols, max_symbol_size)
    {
        // A Reed-Solomon code cannot support more symbols
        // than 2^m - 1 where m is the size of the finite
        // field
        assert(max_symbols < field_type::order);
    }

    template<class SuperCoder>
    inline auto
    systematic_vandermonde_matrix_base<SuperCoder>::factory::construct_matrix(
        uint32_t symbols) -> boost::shared_ptr<generator_matrix>
    {
        assert(symbols > 0);
        assert(m_field);

        boost::shared_ptr<generator_matrix> m =
            SuperCoder::factory::construct_matrix(symbols);

        std::vector<value_type> temp_row(m->row_length());

        for(uint32_t i = 0; i < m->rows(); ++i)
        {
            value_type pivot = m->element(i,i);
            pivot = m_field->invert(pivot);

            fifi::multiply_constant(
                *m_field, pivot, m->row(i), m->row_length());

            for(uint32_t j = 0; j < m->rows(); ++j)
            {
                if(j == i)
                    continue;

                value_type scale = m->element(j, i);
                fifi::multiply_subtract(
                    *m_field, scale, m->row(j), m->row(i),
                    &temp_row[0], m->row_length());

            }
        }

        return m;

    }

}


