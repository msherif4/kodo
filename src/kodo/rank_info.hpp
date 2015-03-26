// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <limits>

namespace kodo
{

    /// @ingroup codec_layers
    /// @brief provides the layer::rank_type typedef
    ///
    /// The data type that is used to store the rank of an encoder of
    /// decoder. The rank of an encoder refers to the number of
    /// symbols available to the encoder for encoding and the rank of
    /// the decoder refers to the number of pivot elements / partially
    /// decoded symbols currently seen.
    template<class SuperCoder>
    class rank_info : public SuperCoder
    {
    public:

        /// @copydoc layer::rank_type
        typedef uint32_t rank_type;

    public:

        /// @copydoc layer::construct(Factory&)
        template<class Factory>
        void construct(Factory& the_factory)
        {
            SuperCoder::construct(the_factory);

            assert(std::numeric_limits<rank_type>::max() >=
                   the_factory.max_symbols());
        }

    };

}


