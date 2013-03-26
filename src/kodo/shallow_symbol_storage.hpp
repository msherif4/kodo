// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_SHALLOW_SYMBOL_STORAGE_HPP
#define KODO_SHALLOW_SYMBOL_STORAGE_HPP

#include <cstdint>
#include <vector>

#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup storage_layers
    /// The shallow storage implementation. In this context shallow
    /// means that the symbol storage only contains pointers to some
    /// external data structure. This is useful in cases where data to
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

    public:

        /// @copydoc layer::construct()
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                m_data.resize(max_symbols, 0);
            }

        /// @copydoc layer::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                std::fill(m_data.begin(), m_data.end(), (data_ptr) 0);
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

        /// @copydoc layer::swap_symbols()
        void swap_symbols(std::vector<data_ptr> &symbols)
            {
                assert(m_data.size() == symbols.size());
                m_data.swap(symbols);
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

		// Assign the pointer
                m_data[index] = symbol.m_data;
            }

        /// @copydoc layer::copy_symbols()
        void copy_symbols(sak::mutable_storage dest) const
            {
                assert(dest.m_size > 0);
                assert(dest.m_data != 0);

                uint32_t data_to_copy =
                    std::min(dest.m_size, SuperCoder::block_size());

                uint32_t symbol_index = 0;

                while(data_to_copy > 0)
                {
                    uint32_t copy_size =
                        std::min(data_to_copy, SuperCoder::symbol_size());

                    data_to_copy -= copy_size;

                    sak::const_storage src_storage =
                        sak::storage(symbol(symbol_index), copy_size);

                    sak::copy_storage(dest, src_storage);

                    dest.m_size -= copy_size;
                    dest.m_data += copy_size;

                    ++symbol_index;

                }

            }

        /// @copydoc layer::copy_symbol()
        void copy_symbol(uint32_t index, sak::mutable_storage dest) const
            {
                assert(dest.m_size > 0);
                assert(dest.m_data != 0);

                uint32_t data_to_copy =
                    std::min(dest.m_size, SuperCoder::symbol_size());

                sak::const_storage src =
                    sak::storage(symbol(index), data_to_copy);

                sak::copy_storage(dest, src);
            }

    protected:

        /// Symbol mapping
        std::vector<data_ptr> m_data;

    };

    /// Defines a coding layer for 'const' symbol storage. Only useful
    /// for encoders since these to modify the buffers / data they
    /// operate on.
    template<class SuperCoder>
    class const_shallow_symbol_storage : public
        shallow_symbol_storage<true, SuperCoder>
    { };

    /// Defines a coding layer for 'mutable' symbol storage. Allows the
    /// buffer data to be modified i.e. useful in decoders which need to
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

#endif

