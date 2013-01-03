// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_GENERATORS_BLOCK_SEED_HPP
#define KODO_GENERATORS_BLOCK_SEED_HPP

#include <stdint.h>

#include "block_final.hpp"
#include "random_uniform.hpp"

namespace kodo
{
    /// Generates full encoding vectors where the block id is used as seed
    /// for the random generator
    template< template <class> class RandomGenerator, class SuperBlock>
    class block_seed : public SuperBlock
    {
    public:

        /// The value type used inside the field
        typedef typename SuperBlock::value_type value_type;

        /// The random generator
        typedef RandomGenerator<value_type> random_generator;

    public:

        /// @copydoc block::construct()
        void construct(uint32_t block_length, uint32_t seed_value)
            {
                SuperBlock::construct(block_length, seed_value);
                m_random_generator.seed(seed_value);
            }

        /// @copydoc block::fill()
        void fill(uint32_t block_id, value_type *buffer)
            {
                assert(buffer != 0);

                m_random_generator.seed(block_id);
                m_random_generator.generate(buffer, SuperBlock::block_length());
            }

    private:
        /// The random generator
        random_generator m_random_generator;
    };

    /// A block with uniform random generator
    template<class ValueType>
    class block_seed_uniform
        : public block<random_uniform,
                 block_final<block_seed_uniform<ValueType>, ValueType>
                       >
    { };
}

#endif

