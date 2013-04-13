// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <vector>

#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup storage_layers
    /// @brief Implements the Symbol Storage API but only with empty
    ///        functions. Useful in unit tests.
    ///
    /// The fake class can be used to insert a dummy API to replace the
    /// use of a real stack.
    template<class SuperCoder>
    class fake_symbol_storage : public SuperCoder
    {
    public:

        /// @copydoc layer::set_symbols(const sak::const_storage&)
        void set_symbols(const sak::const_storage &symbol_storage)
        {
            (void) symbol_storage;
        }

        /// @copydoc layer::set_symbols(const sak::mutable_storage&)
        void set_symbols(const sak::mutable_storage &symbol_storage)
        {
            (void) symbol_storage;
        }

        /// @copydoc layer::set_symbol(uint32_t,const sak::mutable_storage&)
        void set_symbol(uint32_t index, const sak::mutable_storage &symbol)
        {
            (void) index;
            (void) symbol;
        }

        /// @copydoc layer::set_symbol(uint32_t, const sak::const_storage&)
        void set_symbol(uint32_t index, const sak::const_storage &symbol)
        {
            (void) index;
            (void) symbol;
        }

        /// @copydoc layer::swap_symbols(std::vector<const uint8_t*>&)
        void swap_symbols(std::vector<const uint8_t *> &symbols)
        {
            (void) symbols;
        }

        /// @copydoc layer::swap_symbols(std::vector<uint8_t*>&)
        void swap_symbols(std::vector<uint8_t *> &symbols)
        {
            (void) symbols;
        }

        /// @copydoc layer::swap_symbols(std::vector<uint8_t>&)
        void swap_symbols(std::vector<uint8_t> &symbols)
        {
            (void) symbols;
        }

    };

}


