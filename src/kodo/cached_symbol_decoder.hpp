// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>
#include <iostream>
#include <vector>

#include <fifi/fifi_utils.hpp>

#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup codec_layers
    ///
    /// @brief This layer extracts the symbol coefficients and symbol data
    /// and makes it available for use. It does not perform any decoding on the
    /// coefficients or the symbol.
    ///
    /// An encoder may produce either systematic (i.e. uncoded) or coded
    /// symbols. The cached_symbol_decoder provides the cached_symbol_coded()
    /// function to differentiate between the two.
    /// If a symbol is systematic the cached_symbol_coded() will return false
    /// and you may use the cached_symbol_index() to determine which uncoded
    /// original symbol it corresponds to.
    /// On the other hand if cached_symbol_coded() returns true the symbol is
    /// some linear combination of the original symbols. The coefficients used
    /// to create the linear combination may be retrieved through the
    /// cached_symbol_coefficients() function.
    ///
    /// Finally the data of the symbol may be retried using the
    /// cached_symbol_data() function.
    ///
    /// You can check the example in use_cached_symbol_decoder.cpp to see an
    /// example of how to the use cached_symbol_decoder.
    ///
    template<class SuperCoder>
    class cached_symbol_decoder : public SuperCoder
    {
    public:

        /// @copydoc layer::construct(Factory&)
        template<class Factory>
        void construct(Factory& the_factory)
        {
            SuperCoder::construct(the_factory);

            m_data.resize(the_factory.max_symbol_size());
            m_coefficients.resize(the_factory.max_coefficients_size());
        }

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data,
                           uint8_t *symbol_coefficients)
        {
            assert(symbol_data != 0);
            assert(symbol_coefficients != 0);

            uint32_t symbol_size = SuperCoder::symbol_size();
            uint32_t coef_size = SuperCoder::coefficients_size();

            auto data_dest = sak::storage(m_data);
            auto coef_dest = sak::storage(m_coefficients);

            auto data_src = sak::storage(symbol_data, symbol_size);
            auto coef_src = sak::storage(symbol_coefficients, coef_size);

            sak::copy_storage(data_dest, data_src);
            sak::copy_storage(coef_dest, coef_src);

            m_symbol_coded = true;

            SuperCoder::decode_symbol(symbol_data, symbol_coefficients);
        }

        /// @copydoc layer::decode_symbol(uint8_t*,uint32_t)
        void decode_symbol(uint8_t *symbol_data, uint32_t symbol_index)
        {
            assert(symbol_data != 0);
            assert(symbol_index < SuperCoder::symbols());

            uint32_t symbol_size = SuperCoder::symbol_size();

            auto data_dest = sak::storage(m_data);
            auto data_src = sak::storage(symbol_data, symbol_size);

            sak::copy_storage(data_dest, data_src);

            m_symbol_index = symbol_index;
            m_symbol_coded =  false;

            SuperCoder::decode_symbol(symbol_data, symbol_index);
        }

        /// @return True if the previous symbol was uncoded in that case
        ///         its index can be retrieved by calling symbol_index()
        ///         otherwise the symbol was coded and its coefficients
        ///         can be retried by calling symbol_coefficients()
        bool cached_symbol_coded() const
        {
            return m_symbol_coded;
        }

        /// @return The index of the uncoded symbol
        uint32_t cached_symbol_index() const
        {
            assert(!m_symbol_coded);
            return m_symbol_index;
        }

        /// @return The data of the symbol. The size of the symbol in
        ///         bytes can be retried by calling layer::symbol_size()
        const uint8_t* cached_symbol_data() const
        {
            return &m_data[0];
        }

        /// @copydoc cached_symbol_data() const
        uint8_t* cached_symbol_data()
        {
            return &m_data[0];
        }

        /// @return The coding coefficients used to encode the symbol.
        ///         The size of the coefficients buffer in bytes can be
        ///         retried by calling the layer::coefficients_size()
        const uint8_t* cached_symbol_coefficients() const
        {
            assert(m_symbol_coded);
            return &m_coefficients[0];
        }

        /// @copydoc cached_symbol_coefficients() const
        uint8_t* cached_symbol_coefficients()
        {
            assert(m_symbol_coded);
            return &m_coefficients[0];
        }

    private:

        /// Tracks whether the recent decoding symbol was coded
        bool m_symbol_coded;

        /// If the symbol what uncoded what was the symbol index
        uint32_t m_symbol_index;

        /// Stores the data of the decoding symbol
        std::vector<uint8_t> m_data;

        /// If coded stores the coefficients of the decoding symbol
        std::vector<uint8_t> m_coefficients;

    };

}

