// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RANK_CALLBACK_HPP
#define KODO_RANK_CALLBACK_HPP

#include <cstddef>
#include <cstdint>
#include <functional>

namespace kodo
{
    /// @ingroup codec_layers
    /// Callback on rank changed
    template<class SuperCoder>
    class rank_callback : public SuperCoder
    {
    public:

        /// Constructor
        rank_callback()
            : m_callback_func(nullptr)
            { }

        /// Reset rank changed callback function
        /// @copydoc layer::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                /// Reset callback function
                m_callback_func = nullptr;
            }

    public:

        /// Invoke rank changed callback
        /// @copydoc layer::decode_symbol()
        void decode_symbol(uint8_t *symbol_data,
                           uint8_t *symbol_coefficients)
            {
                /// Rank before decoding
                uint32_t rank = SuperCoder::rank();

                /// Decode symbol
                SuperCoder::decode_symbol(symbol_data, symbol_coefficients);
               
                /// If rank changed during decoding
                if (rank < SuperCoder::rank())
                {
                    /// Execute callback function if a callback function is set
                    if (m_callback_func)
                    {
                        m_callback_func(SuperCoder::rank());
                    }
                }
            }

        /// Invoke rank changed callback
        /// @copydoc layer::decode_symbol()
        void decode_symbol(const uint8_t *symbol_data,
                           uint32_t symbol_index)
            {
                /// Rank before decoding
                uint32_t rank = SuperCoder::rank();

                /// Decode symbol
                SuperCoder::decode_symbol(symbol_data, symbol_index);
               
                /// If rank changed during decoding
                if (rank < SuperCoder::rank())
                {
                    /// Execute callback function if set
                    if (m_callback_func)
                    {
                        m_callback_func(SuperCoder::rank());
                    }
                }
            }

        /// Set rank changed callback function
        void set_rank_changed_callback (
            std::function<void (uint32_t rank)> func )
            {
                assert(func != nullptr);

                m_callback_func = func;
            }

        /// Reset rank changed callback function
        void reset_rank_changed_callback()
            {
                m_callback_func = nullptr;
            }

        private:
            
            /// Rank changed callback function
            std::function<void (uint32_t rank)> m_callback_func;
    };
}

#endif

