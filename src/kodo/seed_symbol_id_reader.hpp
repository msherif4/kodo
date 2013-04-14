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

    /// @brief Reads the seed from the symbol_id buffer and uses it to seed
    ///        the generator layer, which produces the corresponding coding
    ///        coefficients.
    ///
    /// @ingroup symbol_id_layers
    template<class SuperCoder>
    class seed_symbol_id_reader : public seed_symbol_id<SuperCoder>
    {
    public:

        /// Type of SuperCoder with injected symbol_coefficient_buffer
        typedef seed_symbol_id<SuperCoder> Super;

        /// The seed type from the generator used
        typedef typename Super::seed_type seed_type;

    public:

        /// @copydoc layer::read_id(uint8_t*, uint8_t**)
        void read_id(uint8_t *symbol_id, uint8_t **symbol_coefficients)
        {
            assert(symbol_id != 0);
            assert(symbol_coefficients != 0);

            seed_type seed = sak::big_endian::get<seed_type>(symbol_id);

            Super::seed(seed);
            Super::generate(&m_coefficients[0]);

            *symbol_coefficients = &m_coefficients[0];
        }

    private:

        /// Access the buffer in the coefficients buffer
        /// layer used by the seed_symbol_id layer
        using Super::m_coefficients;

    };

}


