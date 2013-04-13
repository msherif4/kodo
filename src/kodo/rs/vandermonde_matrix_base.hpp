// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "../matrix.hpp"
#include "transpose_vandermonde_matrix.hpp"

namespace kodo
{

    /// @brief Computes a Vandermonde matrix to generate the coding
    ///        coefficients. The matrix code is
    template<class SuperCoder>
    class vandermonde_matrix_base : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// The generator matrix
        typedef matrix<field_type> generator_matrix;

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

            /// Constructs a basis Vandermonde matrix. The matrix
            /// is non-systematic (see RFC 5510).
            /// @param symbols The number of source symbols to encode
            /// @return The Vandermonde matrix
            boost::shared_ptr<generator_matrix> construct_matrix(
                uint32_t symbols);

        };

    };

    template<class SuperCoder>
    vandermonde_matrix_base<SuperCoder>::factory::factory(
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
    vandermonde_matrix_base<SuperCoder>::factory::construct_matrix(
        uint32_t symbols)
        -> boost::shared_ptr<generator_matrix>
    {
        assert(symbols > 0);
        assert(m_field);

        /// The maximum number of encoding symbols
        uint32_t max_symbols = field_type::order - 1;

        // Create the Vandermonde matrix as suggested in
        // RFC 5510.
        //
        // a is the primitive element (alpha)
        //
        // [a^(0), a^(0)  , a^(0), ... , a^(0)      ]
        // [a^(0), a^(1)  , a^(2), ... , a^(k-1)    ]
        // [a^(0), a^(2)  , a^(4), ... , a^(2*(k-1))]
        // [a^(0), a^(3)  , a^(6), ... , a^(3*(k-1))]
        // [a^(0), a^(4)  , a^(8), ... , a^(4*(k-1))]
        //                   :
        //                   :
        //                   :
        // [a^(0), a^(n-1), a^(2*(n-1)), ... , a^((n-1)*(k-1))]
        auto m = boost::make_shared<generator_matrix>(symbols, max_symbols);

        // Follows the progress of alpha along the rows
        value_type a_row = 1U;

        for(uint32_t j = 0; j < symbols; ++j)
        {
            value_type a_column = 1U;

            for(uint32_t i = 0; i < max_symbols; ++i)
            {
                m->set_element(j, i, a_column);
                a_column = m_field->multiply(a_row, a_column);
            }

            // Multiplying with 2U corresponds to multiplying
            // with x
            a_row = m_field->multiply(a_row, 2U);
        }

        return m;
    }

}


