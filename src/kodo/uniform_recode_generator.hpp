// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_UNIFORM_RECODE_GENERATOR_HPP
#define KODO_UNIFORM_RECODE_GENERATOR_HPP

#include <cstdint>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace kodo
{

    /// @todo delete this
    /// @ingroup coefficient_generator_layers
    /// This is a special generator which is often used in
    /// network codes. It produces the symbol coefficients
    /// which should be used to recode a symbol in the by recombining the
    template<class SuperCoder>
    class recode_generator : public SuperCoder
    {
    public:

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// The random generator used
        typedef boost::random::mt19937 generator_type;

        /// @copydoc layer::seed_type
        typedef generator_type::result_type seed_type;

    public:

        /// @copydoc layer::generate_partial(uint8_t*)
        void generate_partial(uint8_t *symbol_coefficients)
            {
                assert(symbol_coefficients != 0);

                for(uint32_t i = 0; i < SuperCoder::coefficients_size(); ++i)
                {
                    symbol_coefficients[i] =
                        m_distribution(m_random_generator);
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

