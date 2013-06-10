// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <vector>

#include <fifi/fifi_utils.hpp>

#include <sak/convert_endian.hpp>
#include <sak/aligned_allocator.hpp>

#include "reed_solomon_symbol_id.hpp"

namespace kodo
{

    /// @ingroup symbol_id_layers
    ///
    /// @brief Uses the count of the currently encoded symbol to select the
    ///        proper row in the generator matrix. The row is selected using
    ///        the encoded symbol count.
    template<class SuperCoder>
    class reed_solomon_symbol_id_writer :
        public reed_solomon_symbol_id<SuperCoder>
    {
    public:

        /// Type of actual SuperCoder
        typedef reed_solomon_symbol_id<SuperCoder> Super;

        /// @copydoc layer::field_type
        typedef typename Super::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename Super::value_type value_type;

    public:

        /// @copydoc layer::initialize(factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            SuperCoder::initialize(the_factory);

            m_coefficients.resize(
                Super::coefficients_size(), 0);
        }

        /// @copydoc layer::write_id(uint8_t*, uint8_t**)
        uint32_t write_id(uint8_t *symbol_id, uint8_t **coefficients)
        {
            assert(symbol_id != 0);
            assert(coefficients != 0);

            uint32_t symbol_index = Super::encode_symbol_count();

            // An Reed-Solomon code is not rate-less
            assert(symbol_index < field_type::order - 1);

            // Store the index as the symbol id
            sak::big_endian::put<value_type>(symbol_index, symbol_id);

            sak::copy_storage(
                sak::storage(m_coefficients),
                sak::storage(m_matrix->row(symbol_index),
                             m_matrix->row_size()));

            *coefficients = &m_coefficients[0];

            return sizeof(value_type);
        }

    protected:

        /// Access Reed-Solomon generator class
        using Super::m_matrix;

        /// The storage type
        typedef std::vector<uint8_t, sak::aligned_allocator<uint8_t> >
            aligned_vector;

        /// Temp symbol id (with aligned memory)
        aligned_vector m_coefficients;

    };

}


