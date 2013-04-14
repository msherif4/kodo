// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <vector>

#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup symbol_storage_layers
    /// @brief The shallow storage implementation. In this context shallow
    /// means that the symbol storage only contains pointers to some
    /// external data structure.
    ///
    /// This is useful in cases where data to
    /// be encoded already has been read into memory or if a user requires
    /// incoming data to be directly decoded into a specific buffer.
    template<bool IsConst, class SuperCoder>
    class shallow_symbol_storage : public SuperCoder
    {
    public:

        /// The data pointer used will be either:
        /// IsConst = true -> const uint8_t*
        /// IsConst = false -> uint8_t*
        typedef typename std::conditional<IsConst,
                // If const
                typename std::add_pointer<
                typename std::add_const<uint8_t>::type >::type,
                // Else
                typename std::add_pointer<uint8_t>::type >::type data_ptr;

        /// The storage type used will be either:
        /// IsConst = true -> sak::const_storage
        /// IsConst = false -> sak::mutable_storage
        typedef typename std::conditional<IsConst,
                // If const
                sak::const_storage,
                // Else
                sak::mutable_storage>::type storage_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// @copydoc layer::factory
        typedef typename SuperCoder::factory factory;

    public:

        /// @copydoc layer::construct(factory&)
        void construct(factory &the_factory)
        {
            SuperCoder::construct(the_factory);

            m_data.resize(the_factory.max_symbols(), 0);
        }

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(factory &the_factory)
        {
            SuperCoder::initialize(the_factory);

            std::fill(m_data.begin(), m_data.end(), (data_ptr) 0);
            m_symbols_count = 0;
        }

        /// @copydoc layer::symbol(uint32_t) const
        const uint8_t* symbol(uint32_t index) const
        {
            assert(index < SuperCoder::symbols());

            // Did you forget to set the symbol
            assert(m_data[index]);

            return m_data[index];
        }

        /// @copydoc layer::symbol_value(uint32_t) const
        const value_type* symbol_value(uint32_t index) const
        {
            return reinterpret_cast<const value_type*>(symbol(index));
        }

        /// @copydoc layer::swap_symbols(std::vector<data_ptr>&)
        void swap_symbols(std::vector<data_ptr> &symbols)
        {
            assert(m_data.size() == symbols.size());
            m_data.swap(symbols);

            m_symbols_count = 0;

            for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {
                if(m_data[i] != 0)
                    ++m_symbols_count;
            }
        }

        /// @copydoc layer::set_symbols()
        void set_symbols(const storage_type &symbol_storage)
        {
            auto symbol_sequence = sak::split_storage(
                symbol_storage, SuperCoder::symbol_size());

            uint32_t sequence_size = symbol_sequence.size();
            assert(sequence_size == SuperCoder::symbols());

            for(uint32_t i = 0; i < sequence_size; ++i)
            {
                set_symbol(i, symbol_sequence[i]);
            }
        }

        /// @copydoc layer::set_symbol()
        void set_symbol(uint32_t index, const storage_type &symbol)
        {
            assert(symbol.m_data != 0);
            assert(symbol.m_size == SuperCoder::symbol_size());
            assert(index < SuperCoder::symbols());

            if(m_data[index] == 0)
            {
                ++m_symbols_count;
                assert(m_symbols_count <= SuperCoder::symbols());
            }

            // Assign the pointer
            m_data[index] = symbol.m_data;
        }

        /// @copydoc layer::copy_symbols(const sak::mutable_storage&)
        void copy_symbols(const sak::mutable_storage &dest) const
        {
            auto storage = dest;

            assert(storage.m_size > 0);
            assert(storage.m_data != 0);

            uint32_t data_to_copy =
                std::min(storage.m_size, SuperCoder::block_size());

            uint32_t symbol_index = 0;

            while(data_to_copy > 0)
            {
                uint32_t copy_size =
                    std::min(data_to_copy, SuperCoder::symbol_size());

                data_to_copy -= copy_size;

                sak::const_storage src_storage =
                    sak::storage(symbol(symbol_index), copy_size);

                sak::copy_storage(storage, src_storage);

                storage.m_size -= copy_size;
                storage.m_data += copy_size;

                ++symbol_index;

            }
        }

        /// @copydoc layer::copy_symbol(
        ///              uint32_t, const sak::mutable_storage&) const
        void copy_symbol(uint32_t index,
                         const sak::mutable_storage &dest) const
        {
            auto storage = dest;

            assert(storage.m_size > 0);
            assert(storage.m_data != 0);

            uint32_t data_to_copy =
                std::min(storage.m_size, SuperCoder::symbol_size());

            sak::const_storage src =
                sak::storage(symbol(index), data_to_copy);

            sak::copy_storage(storage, src);
        }

        /// @copydoc layer::symbol_exists(uint32_t) const
        bool symbol_exists(uint32_t index) const
        {
            assert(index < SuperCoder::symbols());
            return m_data[index] != 0;
        }

        /// @copydoc layer::symbol_count() const
        uint32_t symbol_count() const
        {
            return m_symbols_count;
        }

        /// @copydoc layer::is_storage_full() const
        bool is_storage_full() const
        {
            return m_symbols_count == SuperCoder::symbols();
        }

    protected:

        /// Symbol mapping
        std::vector<data_ptr> m_data;

        /// Symbols count
        uint32_t m_symbols_count;

    };

    /// @ingroup symbol_storage_layers
    /// @brief Defines a coding layer for 'const' symbol storage. Only useful
    /// for encoders since these to modify the buffers / data they
    /// operate on.
    template<class SuperCoder>
    class const_shallow_symbol_storage : public
        shallow_symbol_storage<true, SuperCoder>
    { };

    /// @ingroup symbol_storage_layers
    /// @brief Defines a coding layer for 'mutable' symbol storage. Allows
    /// the buffer data to be modified i.e. useful in decoders which need to
    /// access and modify the incoming symbols
    template<class SuperCoder>
    class mutable_shallow_symbol_storage : public
        shallow_symbol_storage<false, SuperCoder>
    {
    public:

        /// The actual SuperCoder type
        typedef shallow_symbol_storage<false, SuperCoder> Super;

        /// @copydoc layer::value_type
        typedef typename Super::value_type value_type;

    protected:

        /// Access to the symbol pointers
        using Super::m_data;

    public:

        /// @copydoc layer::symbol(uint32_t)
        uint8_t* symbol(uint32_t index)
        {
            assert(index < Super::symbols());

            // Did you forget to set the symbol
            assert(m_data[index]);

            return m_data[index];
        }

        /// @copydoc layer::symbol_value(uint32_t)
        value_type* symbol_value(uint32_t index)
        {
            return reinterpret_cast<value_type*>(symbol(index));
        }

    };

}


