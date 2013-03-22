// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_REED_SOLOMON_SYMBOL_ID_WRITER_HPP
#define KODO_RS_REED_SOLOMON_SYMBOL_ID_WRITER_HPP

#include <cstdint>

#include <fifi/fifi_utils.hpp>

#include "reed_solomon_symbol_id.hpp"

namespace kodo
{

    /// @todo make the coefficient** to a const uint8_t ** for the write_id
    /// @brief Uses the count of the currently encoded symbol to select the
    ///        proper row in the generator matrix. The row is selected using
    ///        the encoded symbol count.
    ///
    /// @ingroup symbol_id_layers
    template<class SuperCoder>
    class reed_solomon_symbol_id_writer
        : public reed_solomon_symbol_id<SuperCoder>
    {
    public:

        /// Type of actual SuperCoder
        typedef reed_solomon_symbol_id<SuperCoder> Super;

        /// @copydoc layer::field_type
        typedef typename Super::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename Super::value_type value_type;

    public:

        /// @copydoc layer::write_id(uint8_t*,uint8_t**)
        uint32_t write_id(uint8_t *symbol_id, uint8_t **symbol_coefficients)
            {
                assert(symbol_id != 0);
                assert(symbol_coefficients != 0);

                uint32_t symbol_index = Super::encode_symbol_count();

                // An Reed-Solomon code is not rate-less
                assert(symbol_index < field_type::order - 1);

                // Store the index as the symbol id
                sak::big_endian::put<value_type>(symbol_index, symbol_id);

                *symbol_coefficients = m_matrix->row(symbol_index);

                return sizeof(value_type);
            }

    protected:

        /// Access Reed-Solomon generator class
        using Super::m_matrix;

    };

}

#endif

