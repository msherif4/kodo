// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

namespace kodo
{

    /// @ingroup codec_layers
    /// @brief Implements the layer::rank() and layer::symbol_pivot()
    ///        functions based on the storage status.
    ///
    /// This kind of functionality is needed when an encoder should be
    /// able to support encoding before all symbols are ready. The
    /// coefficient generator layers uses this information to create
    /// partial coefficient vectors if needed.
    template<class SuperCoder>
    class storage_aware_encoder : public SuperCoder
    {
    public:

        /// @copydoc layer::rank() const
        uint32_t rank() const
        {
            return SuperCoder::symbols_initialized();
        }

        /// @copydoc layer::symbol_pivot(uint32_t) const
        bool symbol_pivot(uint32_t index) const
        {
            assert(index < SuperCoder::symbols());
            return SuperCoder::is_symbol_initialized(index);
        }

    };

}


