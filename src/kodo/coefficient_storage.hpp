// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_COEFFICIENT_STORAGE_H
#define KODO_COEFFICIENT_STORAGE_H

#include <cstdint>
#include <fifi/fifi_utils.hpp>
#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup coefficient_storage_layers
    /// @brief Provides storage and access to the coding coefficients
    ///        used during encoding and decoding.
    template<class SuperCoder>
    class coefficient_storage : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

        /// Pointer to coder produced by the factories
        typedef typename SuperCoder::pointer pointer;

    public:

        /// @ingroup factory_layers
        /// The factory layer associated with this coder.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @copydoc layer::factory::build(uint32_t,uint32_t)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder =
                        SuperCoder::factory::build(symbols, symbol_size);

                    coder->Initialize_storage(
                        SuperCoder::factory::max_symbols(),
                        SuperCoder::factory::max_coefficients_size());

                    return coder;
                }
        };

    public:

        /// @copydoc layer::coefficients(uint32_t)
        uint8_t* coefficients(uint32_t index)
            {
                assert(index < SuperCoder::symbols());
                return &(m_coefficients_storage[index])[0];
            }

        /// @copydoc layer::coefficients(uint32_t) const
        const uint8_t* coefficients(uint32_t index) const
            {
                assert(index < SuperCoder::symbols());
                return &(m_coefficients_storage[index])[0];
            }

        /// @copydoc layer::coefficients_value(uint32_t)
        value_type* coefficients_value(uint32_t index)
            {
                return reinterpret_cast<value_type*>(
                    coefficients(index));
            }

        /// @copydoc layer::coefficients_value(uint32_t) const
        const value_type* coefficients_value(uint32_t index) const
            {
                return reinterpret_cast<const value_type*>(
                    coefficients(index));
            }

        /// @copydoc layer::set_coefficients(uint32_t,const sak::const_storage&)
        void set_coefficients(uint32_t index,
                              const sak::const_storage &storage)
            {
                assert(storage.m_size == SuperCoder::coefficients_size());
                assert(storage.m_data != 0);

                auto dest = sak::storage(
                    coefficients(index), SuperCoder::coefficients_size());

                sak::copy_storage(dest, storage);
            }


    private:

        /// Initialize the coefficient storage
        /// @param max_symbols The maximum symbols we support
        /// @param max_coefficients_size The maximum number of bytes needed
        ///        to store the coding coefficients.
        void Initialize_storage(uint32_t max_symbols,
                                uint32_t max_coefficients_size)
            {
                assert(max_symbols > 0);
                assert(max_coefficients_size > 0);

                // Note, that resize will not re-allocate anything
                // as long as the sizes are not larger than
                // previously. So this call should only have an
                // effect the first time this function is called.

                // Note, we have as many vectors as we have symbols
                // therefore the maximum number of vectors also
                // equal the maximum number of symbols
                m_coefficients_storage.resize(max_symbols);
                for(uint32_t i = 0; i < max_symbols; ++i)
                {
                    m_coefficients_storage[i].resize(max_coefficients_size);
                }
            }

    private:

        /// Stores the encoding vectors, we do a vector of vectors
        /// to maximize chances of proper alignment of individual
        /// coefficient vectors. This is needed when using SSE etc.
        /// instructions for fast computations with the coefficients
        std::vector< std::vector<uint8_t> > m_coefficients_storage;

    };
}

#endif

