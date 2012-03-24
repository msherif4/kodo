// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_GENERATORS_BLOCK_CACHE_LOOKUP_H
#define KODO_GENERATORS_BLOCK_CACHE_LOOKUP_H

#include <stdint.h>
#include <map>

#include "block.h"
#include "block_seed.h"

namespace kodo
{
    
    // Caches the generated random data blocks to increase
    // performance.
    template<class SuperBlock>
    class block_cache_lookup : public SuperBlock
    {
    public:

        // The value type used inside the field
        typedef typename SuperBlock::value_type value_type;

        // The pointer returned by the factory
        typedef typename SuperBlock::pointer pointer;

        // Block cache factory
        class factory : public SuperBlock::factory
        {

        public:

            // @see block_final::factory(...)
            factory(uint32_t seed_value = 0)
                : SuperBlock::factory(seed_value)
                { }

            // @see block_final::factory(...)
            pointer build(uint32_t block_length)
                {
                    if(m_cache.find(block_length) == m_cache.end())
                    {
                        pointer p = SuperBlock::factory::build(block_length);
                        p->prepare(100);
                        m_cache[block_length] = p;
                        return p;
                    }
                    else
                    {
                        return m_cache[block_length];
                    }
                }
        private:
            
            // map for blocks
            std::map<uint32_t, pointer> m_cache;
        };

    public:
        
        // Look in the cache for the given block id, if it not found put it in
        // the cache, and then return a copy from the cache
        // @param block_id the id of the block
        // @param buffer where the coding vector will be put
        void fill(uint32_t block_id, value_type *buffer)
            {
                assert(buffer != 0);

                if(m_blocks.size() < block_id)
                {
                    SuperBlock::fill(block_id, buffer);
                }
                else
                {
                    std::vector<value_type> &block = m_blocks[block_id];
                    std::copy(block.begin(), block.end(), buffer);
                }
            }

        void prepare(uint32_t blocks)
            {
                m_blocks.resize(blocks);

                for(uint32_t i = 0; i < blocks; ++i)
                {
                    m_blocks[i].resize(SuperBlock::block_length());
                    SuperBlock::fill(i, &m_blocks[i][0]);
                }
            }
    private:

        // map for block_id vectors
        std::vector< std::vector<value_type> > m_blocks;
        
    };

    // A block with uniform random generator
    template<class ValueType>
    class block_cache_lookup_uniform
        : public block_cache<block<random_uniform,
                 block_final<block_cache_lookup_uniform<ValueType>, ValueType> 
                                 > >
    { }; 

    // A block with uniform random generator
    template<class ValueType>
    class block_cache_lookup_seed_uniform
        : public block_cache<block_seed<random_uniform,
                 block_final<block_cache_lookup_seed_uniform<ValueType>, ValueType> 
                                 > >
    { }; 
    
}

#endif

