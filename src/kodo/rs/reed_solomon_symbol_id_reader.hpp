// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <fifi/fifi_utils.hpp>
#include <sak/convert_endian.hpp>

#include "../aligned_coefficients_buffer.hpp"
#include "reed_solomon_symbol_id.hpp"

namespace kodo
{

    /// @ingroup symbol_id_layers
    ///
    /// @brief Reads the row index of the Reed-Solomon generator matrix from
    ///        the symbol id buffer. This is then use to fetch the
    ///        corresponding coding coefficients.
    ///
    template<class SuperCoder>
    class reed_solomon_symbol_id_reader_base : public SuperCoder
    {
    public:

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

    public:

        /// @copydoc layer::read_id(uint8_t*, uint8_t**)
        void read_id(uint8_t *symbol_id, uint8_t **symbol_coefficients)
        {
            assert(symbol_id != 0);
            assert(symbol_coefficients != 0);

            value_type row_index =
                sak::big_endian::get<value_type>(symbol_id);

            sak::const_storage src =
                sak::storage(m_matrix->row(row_index),
                             m_matrix->row_size());

            sak::mutable_storage dest =
                sak::storage(m_coefficients);

            sak::copy_storage(dest, src);

            *symbol_coefficients = &m_coefficients[0];
        }

    private:

        /// Access Reed-Solomon generator class
        using SuperCoder::m_matrix;

        /// Access the buffer in the coefficients buffer
        /// layer used by the reed_solomon_symbol_id layer
        using SuperCoder::m_coefficients;

    };

    /// @copydoc reed_solomon_symbol_id_reader_base
    template<class SuperCoder>
    class reed_solomon_symbol_id_reader
        : public reed_solomon_symbol_id_reader_base<
        aligned_coefficients_buffer<
            reed_solomon_symbol_id<SuperCoder> > >
    { };


}


