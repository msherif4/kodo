// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_SEED_DECODER_HPP
#define KODO_RLNC_SEED_DECODER_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include <sak/convert_endian.hpp>

namespace kodo
{
    /// generates coding vector based on the seeds which is accompanying the
    /// coded symbols from the encoder. Note that this approach is currently
    /// incompatible with recoding
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

            /// @copydoc final_coder_factory::factory::factory()
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

        /// @copydoc final_coder_factory::construct()
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                uint32_t max_vector_length =
                    vector_type::length(max_symbols);

                m_symbol_id.resize(max_vector_length);
            }


        /// The decode function which consumes the payload
        /// @param symbol_data the input payload
        /// @param symbol_id the corresponding symbol id
        void decode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                seed_id id =
                    sak::big_endian::get<seed_id>(symbol_id);

                /// Fill the encoding vector
                SuperCoder::generate(id, &m_symbol_id[0]);

                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                SuperCoder::decode_with_vector(symbol, &m_symbol_id[0]);
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
        std::vector<value_type> m_symbol_id;

    };
}

#endif

