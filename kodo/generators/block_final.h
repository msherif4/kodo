// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_GENERATORS_BLOCK_FINAL_H
#define KODO_GENERATORS_BLOCK_FINAL_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace kodo
{

    /// The final layer in block generators.
    /// The purpose of the block generators are to provide indexable
    /// random data. This was designed to serves two main purposes:
    /// 1) Random coefficients may be reused between coding blocks
    ///    e.g. for RLNC codes one set of random encoding vectors
    ///    can be used for all generations of the same size. This
    ///    does not affect performance of probability of linear
    ///    dependency since the generations are encoded / decoded
    ///    independently.
    /// 2) When state of the random generators needs to be synchronized
    ///    e.g. between a sender and receiver. Knowing that using the
    ///    same block id in the block generator will generate the same
    ///    sequence of random numbers is quite helpful. This is e.g.
    ///    utilized in the RNLC seed_encoder/seed_decoder to avoid
    ///    transmitting the entire encoding vector. 
    //
    template<class FINAL, class ValueType>
    class block_final
    {
    public:

        /// The value type used for the random data
        typedef ValueType value_type;

        /// The final block type
        typedef boost::shared_ptr<FINAL> pointer;

        /// Factory used to build the blocks
        class factory
        {
        public:

            /// Constructs a new block factory
            /// @param seed_value, the seed value used for
            ///        the underlying random generator
            factory(uint32_t seed_value = 0)
                : m_seed(seed_value)
                { }

            /// Build a factory for blocks of the defined length
            /// @param block_length the length of the block in value_type
            ///        elements
            pointer build(uint32_t block_length)
                {
                    assert(block_length > 0);
                    
                    pointer p = boost::make_shared<FINAL>();
                    p->construct(block_length, m_seed);

                    return p;
                }
            
        private:

            /// The seed to used to "reset" the random generator
            uint32_t m_seed;
        };
        
    public:

        /// Construct and store the parameters to the block generator
        void construct(uint32_t block_length, uint32_t seed_value)
            {
                assert(block_length > 0);
                
                m_block_length = block_length;
                m_seed = seed_value;
            }

        /// @return the block_length i.e. the length of a block in
        ///         value_type elements
        uint32_t block_length() const
            {
                return m_block_length;
            }

        /// @return the seed value used for the random generator
        uint32_t seed() const
            {
                return m_seed;
            }

    private:

        /// Store the block length - the length of a block in value_type
        /// elements
        uint32_t m_block_length;

        /// The seed specified
        uint32_t m_seed;
        
    };
}

#endif

