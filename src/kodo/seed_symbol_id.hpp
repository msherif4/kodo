// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_SEED_SYMBOL_ID_HPP
#define KODO_SEED_SYMBOL_ID_HPP

#include <cstdint>

#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup symbol_id_layers
    /// @brief Base layer for seed symbol id reader and writers
    template<class SuperCoder>
    class seed_symbol_id
        : public aligned_coefficients_buffer<SuperCoder>
    {
    public:

        /// Type of SuperCoder with injected aligned_coefficient_buffer
        typedef aligned_coefficients_buffer<SuperCoder> Super;

        /// The seed type from the generator used
        typedef typename Super::seed_type seed_type;

        /// The seed should be integral
        static_assert(std::is_integral<seed_type>::value,
                      "Seed must have an integral type");

    public:

        /// @ingroup factory_layers
        /// The factory layer associated with this coder.
        class factory : public Super::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : Super::factory(max_symbols, max_symbol_size)
                { }

            /// @copydoc layer::factory::max_id_size() const
            uint32_t max_id_size() const
                {
                    return sizeof(seed_type);
                }

        };

    public:

        /// @copydoc layer::id_size() const
        uint32_t id_size() const
            {
                return sizeof(seed_type);
            }

    };

}

#endif

