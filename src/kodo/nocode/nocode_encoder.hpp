// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_NOCODE_NOCODE_ENCODER_HPP
#define KODO_NOCODE_NOCODE_ENCODER_HPP

#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup codec_layers
    ///
    /// Implements the functionality to produce an un-coded / systematic
    /// symbol. See use-cases here @link nocode_decoder @endlink.
    template<class SuperCoder>
    class nocode_encoder : public SuperCoder
    {
    public:

        /// @copydoc layer::encode_symbol(uint8_t*, uint32_t)
        void encode_symbol(uint8_t *symbol_data, uint32_t symbol_index)
            {
                assert(symbol_data != 0);
                assert(symbol_index < SuperCoder::symbols());

                // Copy the symbol
                sak::mutable_storage dest =
                    sak::storage(symbol_data, SuperCoder::symbol_size());

                SuperCoder::copy_symbol(symbol_index, dest);
            }

    };
}

#endif

