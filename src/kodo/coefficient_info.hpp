// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup coefficient_storage_layers
    /// @brief Provides info about the coding coefficients
    ///        used during encoding and decoding.
    ///
    template<class SuperCoder>
    class coefficient_info : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

    public:

        /// @ingroup factory_layers
        /// The factory layer associated with this coder.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t, uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
            { }

            /// @copydoc layer::factory::max_coefficients_size() const
            uint32_t max_coefficients_size() const
            {
                return fifi::elements_to_size<field_type>(
                    SuperCoder::factory::max_symbols());
            }
        };

    public:

        /// Constructor
        coefficient_info()
            : m_coefficients_length(0),
              m_coefficients_size(0)
        { }

        /// @copydoc layer::initialize(factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            SuperCoder::initialize(the_factory);

            m_coefficients_length =
                fifi::elements_to_length<field_type>(the_factory.symbols());

            m_coefficients_size =
                fifi::elements_to_size<field_type>(the_factory.symbols());

            assert(m_coefficients_length > 0);
            assert(m_coefficients_size > 0);
        }

        /// @copydoc layer::coefficients_length() const
        uint32_t coefficients_length() const
        {
            assert(m_coefficients_length > 0);
            return m_coefficients_length;
        }

        /// @copydoc layer::coefficients_size() const
        uint32_t coefficients_size() const
        {
            assert(m_coefficients_size > 0);
            return m_coefficients_size;
        }

    private:

        /// The length of coefficients in value_type elements
        uint32_t m_coefficients_length;

        /// The size of coefficients in bytes
        uint32_t m_coefficients_size;
    };

}


