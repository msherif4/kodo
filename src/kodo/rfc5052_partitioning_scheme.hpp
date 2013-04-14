// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RFC5052_PARTITIONING_SCHEME_HPP
#define KODO_RFC5052_PARTITIONING_SCHEME_HPP

#include <cstdint>

namespace kodo
{

    /// @ingroup block_partitioning_implementation
    /// @brief RFC5052 block partitioning scheme.
    ///
    /// Takes as input the number of symbols the symbol size
    /// and the total length of an object and returns the number
    /// the number blocks to use and the symbols and symbol size
    /// needed to encode/decode an object of the given size
    class rfc5052_partitioning_scheme
    {
    public:

        /// Create an uninitialized partitioning scheme
        rfc5052_partitioning_scheme();

        /// Constructor
        /// @param max_symbols the maximum number of symbols in a block
        /// @param max_symbol_size the size in bytes of a symbol
        /// @param object_size the size in bytes of the whole object
        rfc5052_partitioning_scheme(uint32_t max_symbols,
                                    uint32_t max_symbol_size,
                                    uint32_t object_size);

        /// @copydoc block_partitioning::symbols(uint32_t) const
        uint32_t symbols(uint32_t block_id) const;

        /// @copydoc block_partitioning::symbol_size(uint32_t) const
        uint32_t symbol_size(uint32_t block_id) const;

        /// @copydoc block_partitioning::block_size(uint32_t) const
        uint32_t block_size(uint32_t block_id) const;

        /// @copydoc block_partitioning::bytes_offset(uint32_t) const
        uint32_t byte_offset(uint32_t block_id) const;

        /// @copydoc block_partitioning::bytes_used(uint32_t) const
        uint32_t bytes_used(uint32_t block_id) const;

        /// @copydoc block_partitioning::blocks() const
        uint32_t blocks() const;

        /// @copydoc block_partitioning::object_size() const
        uint32_t object_size() const;

        /// @copydoc block_partitioning::total_symbols() const
        uint32_t total_symbols() const;

        /// @copydoc block_partitioning::total_block_size() const
        uint32_t total_block_size() const;

    private:

        /// The maximum number of symbols per block
        uint32_t m_max_symbols;

        /// The maximum size of a symbol in bytes
        uint32_t m_max_symbol_size;

        /// The size of the object to transfer in bytes
        uint32_t m_object_size;

        /// The total number of symbols in the object
        uint32_t m_total_symbols;

        /// The total number of blocks in the object
        uint32_t m_total_blocks;

        /// The number of large blocks in the object
        uint32_t m_large_blocks;

        /// The number of small blocks in the object
        uint32_t m_small_blocks;

        /// The number of symbols in a large block
        uint32_t m_large_block_symbols;

        /// The number of symbols in a small block
        uint32_t m_small_block_symbols;
    };

    inline rfc5052_partitioning_scheme::rfc5052_partitioning_scheme()
        : m_max_symbols(0),
          m_max_symbol_size(0),
          m_object_size(0),
          m_total_symbols(0),
          m_total_blocks(0),
          m_large_blocks(0),
          m_small_blocks(0),
          m_large_block_symbols(0),
          m_small_block_symbols(0)
    { }

    inline rfc5052_partitioning_scheme::rfc5052_partitioning_scheme(
        uint32_t max_symbols,
        uint32_t max_symbol_size,
        uint32_t object_size)
        : m_max_symbols(max_symbols),
          m_max_symbol_size(max_symbol_size),
          m_object_size(object_size)
    {
        assert(m_max_symbols > 0);
        assert(m_max_symbol_size > 0);
        assert(m_object_size > 0);

        // ceil(x/y) = ((x - 1) / y) + 1
        m_total_symbols = ((m_object_size - 1) / m_max_symbol_size) + 1;
        m_total_blocks  = ((m_total_symbols - 1) / m_max_symbols) + 1;

        m_large_block_symbols = ((m_total_symbols - 1) / m_total_blocks) + 1;
        m_small_block_symbols = m_total_symbols / m_total_blocks;

        m_large_blocks = m_total_symbols -
            (m_small_block_symbols * m_total_blocks);

        m_small_blocks = m_total_blocks - m_large_blocks;
    }

    inline uint32_t
    rfc5052_partitioning_scheme::symbols(uint32_t block_id) const
    {
        assert(block_id < m_total_blocks);
        return block_id < m_large_blocks ?
            m_large_block_symbols : m_small_block_symbols;
    }

    inline uint32_t
    rfc5052_partitioning_scheme::symbol_size(uint32_t /*block_id*/) const
    {
        return m_max_symbol_size;
    }

    inline uint32_t
    rfc5052_partitioning_scheme::block_size(uint32_t block_id) const
    {
        assert(block_id < m_total_blocks);
        return symbols(block_id) * symbol_size(block_id);
    }

    inline uint32_t
    rfc5052_partitioning_scheme::byte_offset(uint32_t block_id) const
    {
        assert(block_id < m_total_blocks);

        if(block_id < m_large_blocks)
        {
            return block_id * m_large_block_symbols * m_max_symbol_size;
        }

        // Calculating the largeblock offset
        uint32_t offset =
            m_large_blocks * m_large_block_symbols * m_max_symbol_size;

        // Calculating the smallblock offset
        offset += (block_id - m_large_blocks) *
            m_small_block_symbols * m_max_symbol_size;

        return offset;
    }

    inline uint32_t
    rfc5052_partitioning_scheme::bytes_used(uint32_t block_id) const
    {
        assert(block_id < m_total_blocks);

        uint32_t offset = byte_offset(block_id);

        assert(offset < m_object_size);
        uint32_t remaining =  m_object_size - offset;
        uint32_t the_block_size = block_size(block_id);

        return std::min(remaining, the_block_size);
    }

    inline uint32_t
    rfc5052_partitioning_scheme::blocks() const
    {
        assert(m_total_blocks > 0);
        return m_total_blocks;
    }

    inline uint32_t
    rfc5052_partitioning_scheme::object_size() const
    {
        assert(m_object_size > 0);
        return m_object_size;
    }

    inline uint32_t
    rfc5052_partitioning_scheme::total_symbols() const
    {
        assert(m_total_symbols > 0);
        return m_total_symbols;
    }

    inline uint32_t
    rfc5052_partitioning_scheme::total_block_size() const
    {
        return m_total_symbols * m_max_symbol_size;
    }

}

#endif

