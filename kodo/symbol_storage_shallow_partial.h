// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_SYMBOL_STORAGE_SHALLOW_PARTIAL_H
#define KODO_SYMBOL_STORAGE_SHALLOW_PARTIAL_H

#include <stdint.h>
#include <vector>

#include <boost/shared_array.hpp>

#include <fifi/fifi_utils.h>

#include "symbol_storage_shallow.h"

namespace kodo
{

    /// The shallow storage _partial_ implementation.
    /// Essentially the same as the shallow storage however the
    /// shallow storage only allows buffers which are exactly the
    /// size of symbols * symbol_size. The partial buffer allows
    /// blocks of data smaller. I.e. of a block is 10000 bytes and
    /// the data you want to encode is in a 9995 byte buffer you can
    /// use it with the partial symbol storage without having to move
    /// it to a 10000 byte temp. buffer.
    template<class SuperCoder>
    class symbol_storage_shallow_partial
        : public symbol_storage_shallow_const<SuperCoder>
    {
    public:

        /// Easy access to SuperCoder
        typedef symbol_storage_shallow_const<SuperCoder> Super;

        /// The field we are in
        typedef typename Super::field_type field_type;
        
        /// The value type used
        typedef typename Super::value_type value_type;

        /// Pointer to the value_type elements
        typedef typename Super::value_ptr value_ptr;

        /// The sequence of storage elements used
        typedef typename Super::storage_sequence_type
            storage_sequence_type;

        /// Pointer produced by the factory
        typedef typename Super::pointer pointer;
        
        /// Partial and zero symbol types
        typedef std::vector<value_type> symbol_type;

        /// Symbol data wrapped in smart ptr
        typedef boost::shared_ptr<symbol_type> symbol_ptr;

        /// Access to the SuperCoder mapping variable
        using Super::m_mapping;
        
    public:

        /// The factory layer associated with this coder. We create
        /// a zero symbol which may be shared with all the
        /// symbol storage partial
        class factory : public Super::factory
        {
        public:

            /// @see Super::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                {
                    uint32_t max_symbol_length =
                        fifi::bytes_to_elements<field_type>(max_symbol_size);

                    assert(max_symbol_length > 0);
                    
                    m_zero_symbol = boost::make_shared<symbol_type>();
                    m_zero_symbol->resize(max_symbol_length, 0);
                }

            /// @see Super::factory::build(...)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder = Super::factory::build(symbols, symbol_size);

                    coder->m_zero_symbol = m_zero_symbol;
                    return coder;
                }

        private:

            /// The zero symbol
            symbol_ptr m_zero_symbol;
            
        };

    public:

        /// @see SuperCoder::construct(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                Super::construct(max_symbols, max_symbol_size);

                uint32_t max_symbol_length =
                    fifi::bytes_to_elements<field_type>(max_symbol_size);
                
                m_partial_symbol = boost::make_shared<symbol_type>();
                m_partial_symbol->resize(max_symbol_length, 0);
            }

        /// @see SuperCoder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                Super::initialize(symbols, symbol_size);
                std::fill(m_partial_symbol->begin(), m_partial_symbol->end(), 0);
            }
                
        /// Sets the storage
        /// @param storage, a const storage container
        void set_symbols(const const_storage &symbol_storage)
            {
                storage_sequence_type symbol_sequence =
                    split_storage(symbol_storage, Super::symbol_size());

                uint32_t sequence_size = symbol_sequence.size(); 
                uint32_t last_index = sequence_size - 1;

                for(uint32_t i = 0; i < last_index; ++i)
                {
                    Super::set_symbol(i, symbol_sequence[i]);
                }

                const_storage last_symbol = symbol_sequence[last_index];

                if(last_symbol.m_size < Super::symbol_size())
                {
                    mutable_storage partial_symbol = storage(*m_partial_symbol);
                    copy_storage(partial_symbol, last_symbol);
                    Super::set_symbol(last_index, partial_symbol);
                }
                else
                {
                    Super::set_symbol(last_index, last_symbol);
                }

                const_storage zero_symbol = storage(*m_zero_symbol);
                
                for(uint32_t i = last_index + 1; i < Super::symbols(); ++i)
                {
                    Super::set_symbol(i, zero_symbol);
                }
            }

    protected:

        /// The zero symbol
        symbol_ptr m_zero_symbol;

        /// The partial symbol
        symbol_ptr m_partial_symbol;
                
    };        
}

#endif

