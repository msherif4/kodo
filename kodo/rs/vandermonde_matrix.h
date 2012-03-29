// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_VANDERMONDE_MATRIX_H
#define KODO_RS_VANDERMONDE_MATRIX_H

#include <fifi/arithmetics.h>
#include <boost/type_traits/is_same.hpp>

namespace kodo
{
    
    /// The full vector encoder 
    template<class Field>
    class vandermonde_matrix
    {
    public:

        /// The field type
        typedef Field field_type;

        /// The value type
        typedef typename field_type::value_type value_type;

    public:

        template<class FieldImpl>
        vandermonde_matrix(bool systematic, uint32_t symbols,
                           const boost::shared_ptr<FieldImpl> &field)
            : m_systematic(systematic),
              m_symbols(symbols)
            {
                assert(m_symbols > 0);
                assert(field);

                BOOST_STATIC_ASSERT((boost::is_same<field_type,
                                     typename FieldImpl::field_type>::value));

                /// The maximum number of encoding symbols
                uint32_t max_symbols = Field::order - 1;

                // Create the Vandermonde matrix as suggested in
                // RFC 5510.
                // Excepts we transpose it, if used as suggested the
                // k coefficients needed when producing a new encoded
                // symbol would be located disjoint in memory.
                // Memory access will be very inefficient if coefficients
                // are not in major row order.
                ///
                // a is the primitive element (alpha)
                ///
                // [a^(0), a^(0)  , a^(0), ... , a^(0)      ]
                // [a^(0), a^(1)  , a^(2), ... , a^(k-1)    ]
                // [a^(0), a^(2)  , a^(4), ... , a^(2*(k-1))]
                // [a^(0), a^(3)  , a^(6), ... , a^(3*(k-1))]
                // [a^(0), a^(4)  , a^(8), ... , a^(4*(k-1))]
                //                   .
                //                   .
                //                   .
                // [a^(0), a^(n-1), a^(2*(n-1)), ... , a^((n-1)*(k-1))]
                m_matrix.resize(symbols * max_symbols, '\0');
                
                // Follows the progress of alpha along the rows
                value_type a_row = 1U; 
                
                for(uint32_t j = 0; j < max_symbols; ++j)
                {
                    value_type a_column = 1U; 
                    
                    for(uint32_t i = 0; i < symbols; ++i)
                    {
                        uint32_t index = (j * symbols) + i;
                
                        m_matrix[index] = a_column;
                        a_column = field->multiply(a_row, a_column);
                    }

                    // Multiplying with 2U corresponds to multiplying with x
                    a_row = field->multiply(a_row, 2U);
                }

                // if(m_systematic)
                // {
                //     set_systematic();
                // }
            }

        /// @return true if the matrix is in systematic form
        bool is_systematic() const
            {
                return m_systematic;
            }

        /// @return the number of source symbols specified for this matrix
        uint32_t symbols() const
            {
                return m_symbols;
            }
        
        /// Returns the coefficients for a specific index
        /// i.e. corresponds to the column in the Vandermonde
        /// matrix.
        /// @return array of coefficients
        const value_type* coefficients(uint32_t index) const
            {
                assert(index < field_type::order);
                return &m_matrix[index * m_symbols];
            }
        
    private:

        value_type* matrix_row(uint32_t index)
            {
                assert(index < field_type::order);
                return &m_matrix[index * m_symbols];
            }

        // Performs row operations and puts the matrix on systematic
        // form
        // void set_systematic()
        //     {
                
        //     }

        // void forward_substitute()
        //     {
        //         for(uint32_t i = 0; i < m_symbols; ++i)
        //         {
        //             for(uint32_t j = 0; j < Field::order - 1; ++j)
        //             {
                        
        //             }
        //         }
                
        //     }

        // void normalize(uint32_t row_id, uint32_t pivot_id)
        //     {
        //         assert(row_id < m_symbols);
        //         assert(pivot_id < m_symbols);


                
        //         value_type *row = coefficients(row_id);
        //         assert(row != 0);
                
        //         value_type coefficient = row[pivot_id];
        //         assert(coefficient > 0);
                
        //         value_type inverted_coefficient = field->invert(coefficient);

        //         field->multiply(row, inverted_coefficient, m_symbols);
        //     }
                
    protected:

        /// True if the matrix is systematic
        bool m_systematic;
        
        /// The number of original source symbols
        uint32_t m_symbols;
                    
        /// The actual Vandermonde matrix
        std::vector<value_type> m_matrix;       
    };
}

#endif

