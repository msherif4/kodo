// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_DECODER_HPP
#define KODO_RLNC_FULL_VECTOR_DECODER_HPP

#include <stdint.h>

#include <boost/optional.hpp>

#include "../linear_block_vector.hpp"

namespace kodo
{
    /// Implements a simple RLNC decoder. The implementation is quite
    /// simple since we rely on most of the functionality in the linear
    /// block decoder. We only have to provide the size functions.
    template<class SuperCoder>
    class full_vector_decoder : public SuperCoder
    {
    public:

        /// The field we use
        typedef typename SuperCoder::field_type field_type;

        /// The value_type used to store the field elements
        typedef typename field_type::value_type value_type;

        /// The encoding vector
        typedef typename SuperCoder::vector_type vector_type;

    public:

        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        public:

            /// @see final_coder_factory::factory(...)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @return the required symbol id buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    uint32_t max_vector_size =
                        vector_type::size( SuperCoder::factory::max_symbols() );

                    assert(max_vector_size > 0);

                    return  max_vector_size;
                }
        };

    public:

        /// @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return SuperCoder::vector_size();
            }
    };
}

#endif

