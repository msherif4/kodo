// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_NON_SYSTEMATIC_ENCODER_HPP
#define KODO_NON_SYSTEMATIC_ENCODER_HPP

#include <stdint.h>

#include <boost/type_traits/is_base_of.hpp>

#include <sak/convert_endian.hpp>

#include "systematic_base_coder.hpp"

namespace kodo
{
    /// Non-systematic encoding layer, this layer always
    /// forwards packets to lower layers without producing
    /// a systematic packet. It is useful in cases where
    /// a decoder expects an encoder to produce packets with
    /// the systematic flags in them, but where we never will
    /// produce a systematic packet (i.e. during recoding in
    /// RLNC schemes).
    template<class SuperCoder>
    class non_systematic_encoder : public SuperCoder
    {
    public:

        /// The flag type
        typedef typename systematic_base_coder::flag_type
            flag_type;

    public:

        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc final_coder_factory::factory::factory()
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @return the required symbol_id buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    return SuperCoder::factory::max_symbol_id_size() +
                        sizeof(flag_type);
                }
        };

    public:

        /// Iterates over the symbols stored in the encoding symbol id part
        /// of the payload id, and calls the encode_symbol function.
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                /// Flag non_systematic packet
                sak::big_endian::put<flag_type>(
                    systematic_base_coder::non_systematic_flag, symbol_id);

                uint32_t bytes_consumed =
                    SuperCoder::encode(symbol_data, symbol_id
                                       + sizeof(flag_type));

                return bytes_consumed + sizeof(flag_type);
            }

        /// @return the required symbol_id buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return SuperCoder::symbol_id_size() +
                    sizeof(flag_type);
            }
    };
}

#endif

