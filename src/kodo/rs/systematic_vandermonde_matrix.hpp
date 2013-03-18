// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_SYSTEMATIC_VANDERMONDE_MATRIX_HPP
#define KODO_RS_SYSTEMATIC_VANDERMONDE_MATRIX_HPP

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace kodo
{

    /// @ingroup user_layers
    /// @brief Computes a Vandermonde matrix to generate the coding
    ///        coefficients. The matrix code is
    template<class SuperCoder>
    class systematic_vandermonde_matrix
        : public vandermonde_matrix<SuperCoder>
    {
    public:

        /// The actual super
        typedef vandermonde_matrix<SuperCoder> Super;

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

        /// The generator matrix type
        typedef std::vector<value_type> generator_matrix;


    public:

        /// The factory layer associated with this coder. Maintains
        /// the block generator needed for the encoding vectors.
        class factory : public SuperCoder::factory
        {
        private:

            /// Access to the finite field implementation used stored in
            /// the finite_field_math layer
            using SuperCoder::factory::m_field;

        public:

            /// @copydoc layer::factory::factory(uint32_t, uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                {
                    // A Reed-Solomon code cannot support more symbols
                    // than 2^m - 1 where m is the size of the finite
                    // field
                    assert(max_symbols < field_type::order);
                }

            /// Constructs a basis Vandermonde matrix. The matrix
            /// is non-systematic (see RFC 5510).
            /// @param symbols The number of source symbols to encode
            /// @return The Vandermonde matrix
            boost::shared_ptr<generator_matrix> construct_matrix(
                uint32_t symbols)
                {
                    assert(symbols > 0);
                    assert(m_field);

                    /// The maximum number of encoding symbols
                    uint32_t max_symbols = field_type::order - 1;

                    // Create the Vandermonde matrix as suggested in
                    // RFC 5510. Excepts we transpose it, if used as
                    // suggested the k coefficients needed when producing
                    // a new encoded symbol would be located disjoint in
                    // memory. Memory access will be very inefficient if
                    // coefficients are not in major row order.
                    //
                    // a is the primitive element (alpha)
                    //
                    // [a^(0), a^(0)  , a^(0), ... , a^(0)      ]
                    // [a^(0), a^(1)  , a^(2), ... , a^(k-1)    ]
                    // [a^(0), a^(2)  , a^(4), ... , a^(2*(k-1))]
                    // [a^(0), a^(3)  , a^(6), ... , a^(3*(k-1))]
                    // [a^(0), a^(4)  , a^(8), ... , a^(4*(k-1))]
                    //                   .
                    //                   .
                    //                   .
                    // [a^(0), a^(n-1), a^(2*(n-1)), ... , a^((n-1)*(k-1))]
                    auto matrix = boost::make_shared<generator_matrix>();
                    matrix->resize(symbols * max_symbols, '\0');

                    // Follows the progress of alpha along the rows
                    value_type a_row = 1U;

                    for(uint32_t j = 0; j < max_symbols; ++j)
                    {
                        value_type a_column = 1U;

                        for(uint32_t i = 0; i < symbols; ++i)
                        {
                            uint32_t index = (j * symbols) + i;

                            matrix->at(index) = a_column;
                            a_column = m_field->multiply(a_row, a_column);
                        }

                        // Multiplying with 2U corresponds to multiplying
                        // with x
                        a_row = m_field->multiply(a_row, 2U);
                    }

                    return matrix;

                }

        };

    };
}

#endif

