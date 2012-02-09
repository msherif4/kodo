// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE_1_0.txt or
// http://www.steinwurf.dk/licensing

#ifndef KODO_GENERATORS_BLOCK_SEED_H
#define KODO_GENERATORS_BLOCK_SEED_H

#include <stdint.h>

#include "block_final.h"
#include "random_uniform.h"

namespace kodo
{

    // Generates full encoding vectors where the block id is used as seed
    // for the random generator
    template< template <class> class RandomGenerator, class SuperBlock>
    class block_seed : public SuperBlock
    {
    public:

        // The value type used inside the field
        typedef typename SuperBlock::value_type value_type;
        typedef RandomGenerator<value_type> random_generator;

    public:

        // @see block_final(...)
        void construct(uint32_t block_length, uint32_t seed_value)
            {
                SuperBlock::construct(block_length, seed_value);
                m_random_generator.seed(seed_value);
            }

        // Fills the vector buffer with the encoding vector defined by the block id
        // @param block_id the id of the block
        // @param buffer where the coding vector will be put
        void fill(uint32_t block_id, value_type *buffer)
            {
                assert(buffer != 0);
                
                m_random_generator.seed(block_id);
                m_random_generator.generate(buffer, SuperBlock::block_length());
            }

    private:        
        // The random generator
        random_generator m_random_generator;
    };

    // A block with uniform random generator
    template<class ValueType>
    class block_seed_uniform
        : public block<random_uniform,
                 block_final<block_seed_uniform<ValueType>, ValueType>
                       >
    { };    
}

#endif

