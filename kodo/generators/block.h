// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_GENERATORS_BLOCK_H
#define KODO_GENERATORS_BLOCK_H

#include <stdint.h>

#include "block_final.h"
#include "random_uniform.h"

namespace kodo
{

    // Generates full encoding vectors where every coding coefficient
    // is chosen uniformly random.
    template< template <class> class RandomGenerator, class SuperBlock>
    class block : public SuperBlock
    {
    public:

        // The value type used inside the field
        typedef typename SuperBlock::value_type value_type;

        // The random generator used
        typedef RandomGenerator<value_type> random_generator;

    public:

        // Constructs a new block generator
        block()
            : m_position(0)
            { }

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
                
                position_generator(block_id);

                m_random_generator.generate(buffer, SuperBlock::block_length());
                ++m_position;
            }

    private:

        // Ensure that the random generator is positioned correctly
        // so that it will generate the same sequence of numbers for
        // same block_id.
        // @param block_id, the next block to generate
        void position_generator(uint32_t block_id)
            {
                if(block_id < m_position)
                {
                    m_random_generator.seed(SuperBlock::seed());
                    m_position = 0;
                }

                while(m_position < block_id)
                {
                    for(uint32_t i = 0; i < SuperBlock::block_length(); ++i)
                    {
                        m_random_generator.generate();
                    }

                    ++m_position;
                }
            }
        
    private:

        // The number of generated vectors from the initialized seed
        uint32_t m_position;
        
        // The random generator
        random_generator m_random_generator;
    };

    // A block with uniform random generator
    template<class ValueType>
    class block_uniform
        : public block<random_uniform,
                 block_final<block_uniform<ValueType>, ValueType>
                 >
    { };    
}

#endif

