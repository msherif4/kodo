// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/bernoulli_distribution.hpp>

#include <fifi/is_binary.hpp>
#include <fifi/fifi_utils.hpp>

namespace kodo
{
    /// @ingroup coefficient_generator_layers
    /// @brief Generate uniformly distributed coefficients with a specific
    /// density
    template<class SuperCoder>
    class sparse_nonzero_uniform_generator : public SuperCoder
    {
    public:

        /// @copydoc layer::value_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// The random generator used
        typedef boost::random::mt19937 generator_type;

        /// @copydoc layer::seed_type
        typedef generator_type::result_type seed_type;

    public:

        /// Constructor
        sparse_nonzero_uniform_generator()
            : m_value_distribution(1U, field_type::max_value)
        { }


        /// @copydoc layer::initialize(Factory& factory)
        template<class Factory>
        void initialize(Factory& factory)
        {
            SuperCoder::initialize(factory);

            m_symbol_selection =
                symbol_selection_distribution(0, SuperCoder::symbols()-1);
        }

        /// @copydoc layer::generate(uint8_t*)
        void generate(uint8_t *coefficients)
        {
            SuperCoder::generate(coefficients);

            if(SuperCoder::nonzeros_generated() == 0)
            {
                generate_nonzero(coefficients);
            }

        }

        /// @copydoc layer::generate(uint8_t*)
        void generate_partial(uint8_t *coefficients)
        {
            assert(coefficients != 0);

            SuperCoder::generate_partial(coefficients);

            if(SuperCoder::nonzeros_generated() == 0)
            {
                generate_nonzero(coefficients);
            }
        }

        /// Generate a non-zero coefficient for a randomly chosen symbol
        /// @param coefficients The coefficients buffer
        void generate_nonzero(uint8_t *coefficients)
        {
            uint32_t index = m_symbol_selection(m_random_generator);

            if (fifi::is_binary<field_type>::value)
            {
                fifi::set_value<field_type>(coefficients, index, 1U);
            }
            else
            {
                value_type c = m_value_distribution(m_random_generator);
                fifi::set_value<field_type>(coefficients, index, c);
            }
        }

    private:

        /// The random generator
        boost::random::mt19937 m_random_generator;

        /// The distribution used to select a symbol at random if no non-zero
        /// coefficients were generated for he encoding vector
        typedef boost::random::uniform_int_distribution<uint32_t>
            symbol_selection_distribution;

        /// The symbol selection distribution
        symbol_selection_distribution m_symbol_selection;

        /// The type of the value_type distribution
        typedef boost::random::uniform_int_distribution<value_type>
            value_type_distribution;

        /// Distribution that generates random values from a finite field
        value_type_distribution m_value_distribution;

    };
}

