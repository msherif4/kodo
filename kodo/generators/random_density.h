// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_GENERATORS_RANDOM_UNIFORM_H
#define KODO_GENERATORS_RANDOM_UNIFORM_H

#include <stdint.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/bernoulli_distribution.hpp>

namespace kodo
{
    /// Generates sparse encoding vectors where every coding coefficient
    /// is chosen with the probability defined by the density.
    /// The default density is 0.5 (or 50%)
    template<class ValueType>
    class random_density
    {
    public:

        typedef ValueType value_type;

        /// Fills the vector buffer with random bytes
        void generate(value_type *buffer, uint32_t length)
            {
                assert(buffer != 0);
                assert(length >  0);
                
                for(uint32_t i = 0; i < length; ++i)
                {
                    if m_bernoulli(m_random_generator)
                        {
                            buffer[i] = m_distribution(m_random_generator);
                        }
                }
            }

        /// Set the density of the generator
        /// @ density the density to be used by the generator
        void set_density(double density)
            {
                assert( density > 0);
                assert( density <= 1);
                m_bernoulli = boost::random::bernoulli_distribution<>(density);
            }

        /// Get the density of the generator
        /// @return the density of the generator
        double get_density() const
            {
                return m_bernoulli.p();
            }

        /// Fills the vector buffer with random bytes
        value_type generate()
            {
                return m_distribution(m_random_generator);
            }

        void seed(uint32_t seed_value)
            {
                m_random_generator.seed(seed_value);
            }
        
    private:

        /// The distribution wrapping the random generator
        boost::random::uniform_int_distribution<value_type> m_distribution;

        /// The distribution controlling the density of the coding vector
        boost::random::bernoulli_distribution<> m_bernoulli;
        
        /// The random generator
        boost::random::mt19937 m_random_generator;
    };
}

#endif

