// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace kodo
{

    /// @ingroup codec_layers
    /// @brief Allows a callback function to be invoked after rank changed
    ///
    /// This layer allows a callback function to be assigned to each decoder.
    /// After the rank has changed in a decoder, it will automatically invoke
    /// the assigned callback function.
    template<class SuperCoder>
    class rank_callback_decoder : public SuperCoder
    {
    public:

        /// The rank changed callback function. The callback is invoked
        /// whenever the rank changed and provides the current rank as a
        /// uint32_t
        typedef std::function<void (uint32_t)> rank_changed_callback;

    public:

        /// Constructor
        rank_callback_decoder()
            : m_callback_func(nullptr)
        { }

        /// Reset rank callback function
        /// @copydoc layer::initialize(factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            SuperCoder::initialize(the_factory);

            // Reset callback function
            m_callback_func = nullptr;
        }

    public:

        /// Invoke rank changed callback
        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data,
                           uint8_t *coefficients)
        {
            // Rank before decoding
            uint32_t rank = SuperCoder::rank();

            // Decode symbol
            SuperCoder::decode_symbol(symbol_data, coefficients);

            // Invoke callback function if rank changed
            check_current_rank(rank);
        }

        /// Invoke rank changed callback
        /// @copydoc layer::decode_symbol(uint8_t*,uint32_t)
        void decode_symbol(uint8_t *symbol_data,
                           uint32_t symbol_index)
        {
            // Rank before decoding
            uint32_t rank = SuperCoder::rank();

            // Decode symbol
            SuperCoder::decode_symbol(symbol_data, symbol_index);

            // Invoke callback function if rank changed
            check_current_rank(rank);
        }

        /// Set rank changed callback function
        /// @param callback rank changed callback function
        void set_rank_changed_callback(const rank_changed_callback &callback)
        {
            assert(callback);

            m_callback_func = callback;
        }

        /// Reset rank changed callback function
        void reset_rank_changed_callback()
        {
            m_callback_func = nullptr;
        }

    private:

        /// Invoke callback function if rank has changed
        /// @param old_rank rank before decoding
        void check_current_rank(uint32_t old_rank)
        {
            // If rank changed during decoding
            if (old_rank < SuperCoder::rank())
            {
                // Invoke callback function if set
                if (m_callback_func)
                {
                    m_callback_func(SuperCoder::rank());
                }
            }
        }

    private:

        /// Rank changed callback function
        rank_changed_callback m_callback_func;

    };

}


