// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_SEED_ENCODER_HPP
#define KODO_RLNC_SEED_ENCODER_HPP

#include <stdint.h>

#include <sak/convert_endian.hpp>

namespace kodo
{
    /// The purpose of the seed encoder is to avoid sending a full
    /// encoding vector which for high fields may consume a significant
    /// number of bytes.
    /// The seed encoder instead only prepends the seed/id used to generate
    /// the encoding vector. A decoder may the utilize this seed/id to recreate
    /// the encoding vector on the receiving side.
    template<class SuperCoder>
    class seed_encoder : public SuperCoder
    {
    public:

        /// The field type
        typedef typename SuperCoder::field_type field_type;

        /// The value type
        typedef typename field_type::value_type value_type;

        /// The encoding vector
        typedef typename SuperCoder::vector_type vector_type;

        /// Pointer to coder produced by the factories
        typedef typename SuperCoder::pointer pointer;

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

                uint32_t max_vector_size =
                    vector_type::size(max_symbols);

                m_vector_data.resize(max_vector_size);
            }

        /// @see final_coder_factory::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                m_vector_size = vector_type::size( symbols );
                assert(m_vector_size > 0);

                m_seed_id = 0;
            }

        /// Iterates over the symbols stored in the encoding symbol id part
        /// of the payload id, and calls the encode_symbol function.
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                /// Put in the current seed_id
                sak::big_endian::put<seed_id>(m_seed_id, symbol_id);

                SuperCoder::generate(m_seed_id, reinterpret_cast<value_type*>(&m_vector_data[0]));

                ++m_seed_id;

                SuperCoder::encode(symbol_data, &m_vector_data[0]);

                return symbol_id_size();
            }

        
        /// @return the required payload buffer size in bytes
        uint32_t symbol_id_size() const
            {
                return sizeof(seed_id);
            }

    private:

        /// The encoding vector buffer
        std::vector<uint8_t> m_vector_data;

        /// The size of the encoding vector in bytes
        uint32_t m_vector_size;

        /// Keeping track of the number of packets sent
        seed_id m_seed_id;
    };
}

#endif

