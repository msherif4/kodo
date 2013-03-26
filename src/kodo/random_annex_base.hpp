// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RANDOM_ANNEX_BASE_HPP
#define KODO_RANDOM_ANNEX_BASE_HPP

#include <stdint.h>

#include <set>
#include <vector>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/dynamic_bitset.hpp>

namespace kodo
{
    /// For a given selection of object partitioning parameters this function
    /// returns the maximum allowed annex size.
    ///
    /// The result returned from this function is guaranteed to work for any
    /// partitioning scheme as long as the maximum symbol size is not changed
    /// and maximum number of symbols per block is not increased.
    uint32_t max_annex_size(uint32_t max_symbols,
                            uint32_t max_symbol_size,
                            uint32_t object_size)
    {
        assert(max_symbols > 0);
        assert(max_symbol_size > 0);
        assert(object_size > 0);

        // The minimum number of total symbols we can have for the given
        // maximum_symbol_size and object_size ceil(x/y) = ((x - 1) / y) + 1
        uint32_t min_total_symbols =
            ((object_size - 1) / max_symbol_size) + 1;

        // Out of that we have a our maximum block size and since we
        // select the annex overlap randomly without replacement there
        // is no way we can have an annex bigger than the number of
        // available symbols in the surrounding blocks i.e. the total
        // number of surrounding symbols minus the maximum block
        if(max_symbols > min_total_symbols)
        {
            // Everything fits into one block
            return 0;
        }
        else
        {
            // We know how many symbols are outside the maximum block.
            // Additionally since the annex is contained within a single
            // block it cannot be larger than the max_symbols - 1. If
            // we allow the annex to be == max_symbols the blocks could
            // end up begin pure annex in which case no blocks contain
            // actual symbols.
            return std::min(min_total_symbols - max_symbols,
                            max_symbols - 1);
        }
    }

    /// @brief Annex info helper class
    struct annex_info
    {
        /// Constructor
        annex_info()
            : m_coder_id(0),
              m_symbol_id(0)
            { }

        /// Constructor
        /// @param coder_id The id of the coder
        /// @param symbol_id The id of the symbol
        annex_info(uint32_t coder_id, uint32_t symbol_id)
            : m_coder_id(coder_id),
              m_symbol_id(symbol_id)
            { }

        /// The coder id
        uint32_t m_coder_id;

        /// The symbol id
        uint32_t m_symbol_id;

    };

    /// Allows our annex info class to be used in e.g. a std::set
    inline bool operator<(const annex_info &a, const annex_info &b)
    {
        return a.m_coder_id < b.m_coder_id ||
            (!(b.m_coder_id < a.m_coder_id) &&
             a.m_symbol_id < b.m_symbol_id);
    }

    /// @brief Base class for the random annex encoder and decoder.
    template<class BlockPartitioning>
    class random_annex_base : boost::noncopyable
    {
    public:

        /// The block partitioning scheme used
        typedef BlockPartitioning block_partitioning;

        /// Convenient typedef to make the Standard Libary insert
        /// ugly'ness a bit more nicer
        typedef std::pair<std::set<annex_info>::iterator,bool> annex_return;

        /// The uniform int distribution
        typedef boost::random::uniform_int_distribution<uint32_t>
            uniform_int;

    public:

        /// Builds the random annex according to the given annex size and
        /// partitioning scheme
        /// @param annex_size the size of the annex
        /// @param partitioning the partitioning scheme used
        void build_annex(uint32_t annex_size,
                         const block_partitioning &partitioning)
            {
                // Get the number of blocks for this object
                uint32_t blocks = partitioning.blocks();

                // Prepare annex
                m_annex.resize(blocks);

                // Prepare reverse annex
                m_reverse_annex.resize(blocks);

                if(annex_size == 0 || blocks < 2)
                {
                    // No need to continue - we leave the resize since
                    // then the algorithms operating on the data
                    // structures (m_annex or m_reverse_annex) does not
                    // have to perform the check for
                    // annex_size == 0 or blocks < 2
                    return;
                }

                for(uint32_t i = 0; i < blocks; ++i)
                    m_reverse_annex[i].resize(blocks);

                // For 5 blocks generate between [0,..,3] since 1 block is
                // always excluded only 4 values are suitable. When
                // block 1 is excluded we select between {0, 2, 3, 4}
                // and so forth.
                m_block_distribution
                    = uniform_int(0, blocks - 2);

                // std::cout << "Total blocks " << blocks << std::endl;
                // std::cout << "Annex " << annex_size << std::endl;
                // for(uint32_t i = 0; i < blocks; ++i)
                // {
                //     std::cout << "Block " << i << " symbols "
                //               << partitioning.symbols(i) << std::endl;
                // }

                for(uint32_t i = 0; i < blocks; ++i)
                {
                    //std::cout << "Block " << i << " has "
                    //<< partitioning.symbols(i) << " symbols" << std::endl;

                    // Safety check -- since we select the annex overlap
                    //randomly without replacement there is no way we
                    // can have an annex bigger than the number of
                    // available symbols in the surrounding blocks
                    assert(annex_size < (partitioning.total_symbols() -
                                         partitioning.symbols(i)));

                    while(m_annex[i].size() < annex_size)
                    {
                        uint32_t block_id = select_block(i);

                        uint32_t symbol_id =
                            select_symbol(partitioning.symbols(block_id));

                        annex_info annex(block_id, symbol_id);

                        // Here we rely on the fact that the set will
                        // not insert the element if it already exists
                        annex_return ret = m_annex[i].insert(annex);

                        if(ret.second == true)
                        {
                            m_reverse_annex[block_id][i] = 1;
                        }
                        else
                        {
                            //std::cout << "Could not insert " << std::endl;
                        }
                    }
                }

                // for(uint32_t i = 0; i < blocks; ++i)
                // {
                //     std::set<annex_info>::iterator it;
                //     for(it = m_annex[i].begin(); it != m_annex[i].end(); ++it)
                //     {
                //         annex_info annex = *it;

                //         std::cout << "For block " << i << " select block "
                //                   << annex.first << " and symbol id " << annex.second
                //                   << std::endl;
                //     }

                //     std::cout << "For block " << i << " reverse annex: ";
                //     for(uint32_t j = 0; j < blocks; ++j)
                //     {
                //         std::cout << m_reverse_annex[i][j] << " ";
                //     }
                //     std::cout << std::endl;
                // }
            }

    protected:

        /// Selects a block from the block distribution, however
        /// with a certain block excluded
        /// @param exclude_block block excluded from the random pick
        /// @return the selected block
        uint32_t select_block(uint32_t exclude_block)
            {
                uint32_t block_id = m_block_distribution(m_random_generator);

                if(block_id >= exclude_block)
                {
                    return block_id + 1;
                }
                else
                {
                    return block_id;
                }
            }

        /// Selects a symbol id
        /// @param block_symbols the number of symbols in a block
        /// @return the selected block index
        uint32_t select_symbol(uint32_t block_symbols)
            {
                // Looking at the boost::random::uniform_int_distribution
                // constructor it seems quite cheap to construct. So
                // for simplicity we create one here base on the
                // block symbols parameter. If this can be shown to be
                // too inefficient we should switch to another approach
                // e.g. caching the distributions for reuse or something
                uniform_int symbol_generator(0, block_symbols - 1);

                return symbol_generator(m_random_generator);
            }

    protected:

        /// The distribution wrapping the random generator
        uniform_int m_block_distribution;

        /// The random generator
        boost::random::mt19937 m_random_generator;

        /// Stores the annex for every block
        std::vector< std::set<annex_info> > m_annex;

        /// Stores the reverse annex
        std::vector< boost::dynamic_bitset<> > m_reverse_annex;
    };

}

#endif

