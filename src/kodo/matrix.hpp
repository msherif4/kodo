// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

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

            m_row_size = fifi::elements_to_size<field_type>(m_columns);
            m_row_length = fifi::size_to_length<field_type>(m_row_size);

            m_data.resize(rows * m_row_size, '\0');
        }

        /// Returns the element at the specific row and column in
        /// the matrix.
        /// @param row The row index
        /// @param column The column index
        /// @return The element stored.
        value_type element(uint32_t row, uint32_t column) const
        {
            assert(row < m_rows);
            assert(column < m_columns);

            const value_type *v = row_value(row);
            return fifi::get_value<field_type>(v, column);
        }

        /// Sets the element at the specific row and column in
        /// the matrix.
        /// @param row The row index
        /// @param column The column index
        /// @param value The element to store at the specified position
        void set_element(uint32_t row, uint32_t column, value_type &value)
        {
            assert(row < m_rows);
            assert(column < m_columns);

            value_type *r = row_value(row);
            fifi::set_value<field_type>(r, column, value);
        }

        /// @return The size of a row in bytes
        uint32_t row_size() const
        {
            return m_row_size;
        }

        /// @return The length of a row in the field's value_type
        uint32_t row_length() const
        {
            return m_row_length;
        }

        /// Return the bytes of a row at a specific index
        /// @param index The index of the row to return
        /// @return The byte corresponding to the selected row.
        uint8_t* row(uint32_t index)
        {
            assert(index < m_rows);
            return &m_data[index * m_row_size];
        }

        /// @copydoc row(uint32_t)
        const uint8_t* row(uint32_t index) const
        {
            assert(index < m_rows);
            return &m_data[index * m_row_size];
        }

        /// Return a value_type pointer to a row at a specific index
        /// @param index The index of the row to return
        /// @return The value_type pointer corresponding to the selected row.
        value_type* row_value(uint32_t index)
        {
            assert(index < m_rows);
            return reinterpret_cast<value_type*>(row(index));
        }

        /// @copydoc row_value(uint32_t)
        const value_type* row_value(uint32_t index) const
        {
            assert(index < m_rows);
            return reinterpret_cast<const value_type*>(row(index));
        }

        /// @return The number of rows
        uint32_t rows() const
        {
            return m_rows;
        }

        /// @return The number of columns
        uint32_t columns() const
        {
            return m_columns;
        }

        /// @return A transposed version of the matrix i.e. rows and
        ///         columns switched.
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

        /// Tracks the number of rows
        uint32_t m_rows;

        /// Tracks the number of columns
        uint32_t m_columns;

        /// The size of a row in bytes
        uint32_t m_row_size;

        /// The length of a row in value_type elements
        uint32_t m_row_length;

        /// The buffer storing the data of the matrix
        std::vector<uint8_t> m_data;

    };

}


