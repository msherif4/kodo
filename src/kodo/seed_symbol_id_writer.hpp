// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <fifi/fifi_utils.hpp>

#include "seed_symbol_id.hpp"

namespace kodo
{

    /// @brief Writes a seed as the symbol id. The seed is used to seed the
    ///        generator layer before generating the coefficients.
    ///        The current symbol count is used as the seed.
    ///        Which allows the decoder to reproduce the coefficients used.
    ///
    /// @ingroup symbol_id_layers
    template<class SuperCoder>
    class seed_symbol_id_writer : public seed_symbol_id<SuperCoder>
    {
    public:

        /// Type of SuperCoder with injected symbol_coefficient_buffer
        typedef seed_symbol_id<SuperCoder> Super;

        /// The seed type from the generator used
        typedef typename Super::seed_type seed_type;

    public:

        /// @copydoc layer::write_id(uint8_t*, uint8_t**)
        uint32_t write_id(uint8_t *symbol_id, uint8_t **coefficients)
            {
                assert(symbol_id != 0);
                assert(coefficients != 0);

                seed_type seed =
                    (seed_type) Super::encode_symbol_count();

                Super::seed(seed);
                Super::generate(&m_coefficients[0]);

                /// Store the seed as the symbol id
                sak::big_endian::put<seed_type>(seed, symbol_id);
                *coefficients = &m_coefficients[0];

                return sizeof(seed_type);
            }

    private:

        /// Access the buffer in the coefficients buffer
        /// layer used by the seed_symbol_id layer
        using Super::m_coefficients;

    };

}


