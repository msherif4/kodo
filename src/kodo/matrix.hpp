// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_MATRIX_HPP
#define KODO_MATRIX_HPP

#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @brief Simple storage class with a matrix API for finite
    ///        field elements.
    template<class Field>
    class matrix
    {
    public:

        /// The finite field type used
        typedef Field field_type;

        /// The value type used in the finite field
        typedef typename field_type::value_type value_type;

    public:

        /// Constructor
        /// @param rows The number of rows in the matrix
        /// @param columns The number of columns in the matrix
        matrix(uint32_t rows, uint32_t columns)
            : m_rows(rows),
              m_columns(columns)
            {
                assert(m_rows > 0);
                assert(m_columns > 0);

                m_row_size = fifi::bytes_needed<field_type>(m_columns);
                m_row_length = fifi::elements_needed<field_type>(m_row_size);

                m_data.resize(rows * m_row_size, '\0');
            }

        value_type element(uint32_t row, uint32_t column) const
            {
                assert(row < m_rows);
                assert(column < m_columns);

                const value_type *v = row_value(row);
                return fifi::get_value<field_type>(v, column);
            }

        void set_element(uint32_t row, uint32_t column, value_type &value)
            {
                assert(row < m_rows);
                assert(column < m_columns);

                value_type *r = row_value(row);
                fifi::set_value<field_type>(r, column, value);
            }

        uint32_t row_size() const
            {
                return m_row_size;
            }

        uint32_t row_length() const
            {
                return m_row_length;
            }

        uint8_t* row(uint32_t index)
            {
                assert(index < m_rows);
                return &m_data[index * m_row_size];
            }

        const uint8_t* row(uint32_t index) const
            {
                assert(index < m_rows);
                return &m_data[index * m_row_size];
            }

        value_type* row_value(uint32_t index)
            {
                assert(index < m_rows);
                return reinterpret_cast<value_type*>(row(index));
            }

        const value_type* row_value(uint32_t index) const
            {
                assert(index < m_rows);
                return reinterpret_cast<const value_type*>(row(index));
            }

        uint32_t rows() const
            {
                return m_rows;
            }

        uint32_t columns() const
            {
                return m_columns;
            }

        matrix transpose() const
            {
                matrix m(m_columns, m_rows);

                for(uint32_t i = 0; i < m_rows; ++i)
                {
                    for(uint32_t j = 0; j < m_columns; ++j)
                    {
                        value_type v = element(i,j);
                        m.set_element(j,i,v);
                    }
                }

                return m;
            }

    private:

        uint32_t m_rows;
        uint32_t m_columns;
        uint32_t m_row_size;
        uint32_t m_row_length;

        std::vector<uint8_t> m_data;

    };

}

#endif

