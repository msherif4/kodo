// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_UNIFORM_GENERATOR_HPP
#define KODO_UNIFORM_GENERATOR_HPP

#include <cstdint>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace kodo
{
     /// @ingroup coefficient_generator_layers
    /// Generates an random coefficient (from the chosen Finite Field)
    /// for every symbol.
    template<class SuperCoder>
    class uniform_generator : public SuperCoder
    {
    public:

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// @copydoc layer::seed_type
        typedef generator_type::result_type seed_type;

    public:

        /// @copydoc layer::generate(uint8_t*)
        void generate(uint8_t * symbol_coefficients)
            {
                assert(symbol_coefficients != 0);

                for(uint32_t i = 0; i < SuperCoder::coefficients_size(); ++i)
                {
                    symbol_coefficients[i] = m_distribution(m_random_generator);
                }
            }

        /// @copydoc layer::seed(seed_type)
        void seed(seed_type seed_value)
            {
                m_random_generator.seed(seed_value);
            }

    private:

        /// The distribution wrapping the random generator
        boost::random::uniform_int_distribution<uint8_t> m_distribution;

        /// The random generator
        boost::random::mt19937 m_random_generator;
    };
}

#endif

