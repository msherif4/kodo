// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <vector>

#include "shallow_symbol_storage.hpp"

namespace kodo
{

    /// @ingroup storage_layers
    /// @brief Same as a shallow storage but supports storage object
    ///        with a total size less than the coding block size.
    ///
    /// Essentially the same as the shallow storage however the
    /// shallow storage only allows buffers which are exactly the
    /// size of symbols * symbol_size. The partial buffer allows
    /// blocks of data smaller. I.e. of a block is 10000 bytes and
    /// the data you want to encode is in a 9995 byte buffer you can
    /// use it with the partial symbol storage without having to move
    /// it to a 10000 byte temp. buffer.
    template<class SuperCoder>
    class partial_shallow_symbol_storage
        : public const_shallow_symbol_storage<SuperCoder>
    {
    public:

        /// The actual SuperCoder type
        typedef const_shallow_symbol_storage<SuperCoder> Super;

        /// Pointer produced by the factory
        typedef typename Super::pointer pointer;

        /// Pointer to the type of this layer
        typedef boost::shared_ptr<
            partial_shallow_symbol_storage> this_pointer;

        /// Partial and zero symbol types
        typedef std::vector<uint8_t> symbol_type;

        /// Symbol data wrapped in smart ptr
        typedef boost::shared_ptr<symbol_type> symbol_ptr;

        /// Access to the SuperCoder mapping variable
        using Super::m_data;

    public:

        /// @ingroup factory_layers
        /// The factory layer associated with this coder. We create
        /// a zero symbol which may be shared with all the
        /// symbol storage partial
        class factory : public Super::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : Super::factory(max_symbols, max_symbol_size)
            {
                assert(max_symbol_size > 0);

                m_zero_symbol = boost::make_shared<symbol_type>();
                m_zero_symbol->resize(max_symbol_size, 0);
            }

        private:

            /// Grant the layer access
            friend class partial_shallow_symbol_storage;

            /// The zero symbol
            symbol_ptr m_zero_symbol;

        };

    public:

        /// @copydoc layer::construct(Factory&)
        template<class Factory>
        void construct(Factory &the_factory)
        {
            Super::construct(the_factory);

            assert(the_factory.max_symbol_size() > 0);

            m_zero_symbol = the_factory.m_zero_symbol;
            m_partial_symbol = boost::make_shared<symbol_type>();
            m_partial_symbol->resize(the_factory.max_symbol_size(), 0);
        }

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory &the_factory)
        {
            Super::initialize(the_factory);

            std::fill(m_partial_symbol->begin(),
                      m_partial_symbol->end(), 0);
        }

        /// Initializes the symbol storage layer so that the pointers to the
        /// symbol data are valid. Calling this function will work even
        /// without providing data enough to initialize all symbol pointers.
        /// @copydoc layer::set_symbols(const sak::const_storage &)
        void set_symbols(const sak::const_storage &symbol_storage)
        {
            auto symbol_sequence =
                sak::split_storage(symbol_storage, Super::symbol_size());

            uint32_t sequence_size = symbol_sequence.size();
            uint32_t last_index = sequence_size - 1;

            for(uint32_t i = 0; i < last_index; ++i)
            {
                Super::set_symbol(i, symbol_sequence[i]);
            }

            auto last_symbol =
                symbol_sequence[last_index];

            uint32_t symbol_size = Super::symbol_size();

            auto partial_symbol =
                sak::storage(&m_partial_symbol->at(0), symbol_size);

            auto zero_symbol =
                sak::storage(&m_zero_symbol->at(0), symbol_size);

            if(last_symbol.m_size < Super::symbol_size())
            {
                sak::copy_storage(partial_symbol, last_symbol);
                Super::set_symbol(last_index, partial_symbol);
            }
            else
            {
                Super::set_symbol(last_index, last_symbol);
            }

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


