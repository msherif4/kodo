// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_GENERATORS_RANDOM_UNIFORM_H
#define KODO_GENERATORS_RANDOM_UNIFORM_H

#include <stdint.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace kodo
{
    // Generates full encoding vectors where every coding coefficient
    // is chosen uniformly random.
    template<class ValueType>
    class random_uniform
    {
    public:

        typedef ValueType value_type;

        // Fills the vector buffer with random bytes
        void generate(value_type *buffer, uint32_t length)
            {
                assert(buffer != 0);
                assert(length >  0);
                
                for(uint32_t i = 0; i < length; ++i)
                {
                    buffer[i] = m_distribution(m_random_generator);
                }
            }

        // Fills the vector buffer with random bytes
        value_type generate()
            {
                return m_distribution(m_random_generator);
            }

        void seed(uint32_t seed_value)
            {
                m_random_generator.seed(seed_value);
            }
        
    private:

        // The distribution wrapping the random generator
        boost::random::uniform_int_distribution<value_type> m_distribution;
        
        // The random generator
        boost::random::mt19937 m_random_generator;
    };
}

#endif

