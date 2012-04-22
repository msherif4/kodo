// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_SEED_DECODER_H
#define KODO_RLNC_SEED_DECODER_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <sak/convert_endian.h>

namespace kodo
{

    /// Implementes a simple uniform random encoding scheme
    /// where the payload_id carries all coding coefficients
    /// i.e. the "encoding vector"
    template<class SuperCoder>
    class seed_decoder : public SuperCoder
    {
    public:

        /// The field we use
        typedef typename SuperCoder::field_type field_type;

        /// The value_type used to store the field elements
        typedef typename SuperCoder::value_type value_type;

        /// The encoding vector
        typedef typename SuperCoder::vector_type vector_type;

        /// The seed_id type
        typedef uint32_t seed_id;

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

            /// @return the required payload buffer size in bytes
            uint32_t max_symbol_id_size() const
                {
                    return sizeof(seed_id);
                }

        };

    public:

        /// @see final_coder_factory(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                uint32_t max_vector_length =
                    vector_type::length(max_symbols);

                m_vector_data.resize(max_vector_length);
            }


        /// The decode function which consumes the payload
        /// @param payload, the input payload
        void decode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                seed_id id =
                    sak::big_endian::get<seed_id>(symbol_id);

                /// Fill the encoding vector
                SuperCoder::generate(id, &m_vector_data[0]);

                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                SuperCoder::decode_with_vector(&m_vector_data[0], symbol);
            }


        /// @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                /// We were thinking about returning a full
                /// vector here since that would allow
                /// recoding. However, decoding a recoded
                /// packet is not supported by seed decoder
                /// so it would be confusing an wrong atm.
                return sizeof(seed_id);
            }

    protected:

        /// The encoding vector buffer
        std::vector<value_type> m_vector_data;

    };
}

#endif

