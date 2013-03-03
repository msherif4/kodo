// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_SYSTEMATIC_ENCODER_HPP
#define KODO_SYSTEMATIC_ENCODER_HPP

#include <stdint.h>

#include <sak/convert_endian.hpp>
#include <sak/storage.hpp>

#include <boost/type_traits/is_base_of.hpp>

#include "systematic_base_coder.hpp"
#include "systematic_operations.hpp"

namespace kodo
{

    /// @ingroup codec_header_layers
    /// @ingroup factory_layers
    /// @brief Systematic encoding layer.
    ///
    /// Specifying the SystematicOn template argument true will initialize the
    /// systematic encoder to produce systematic packets. If SystematicOn is
    /// false a user must first call the set_systematic_on() function to
    /// start producing systematic packets.
    template<bool SystematicOn, class SuperCoder>
    class base_systematic_encoder : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

        /// The symbol count type
        typedef typename systematic_base_coder::counter_type
            counter_type;

        /// The flag type
        typedef typename systematic_base_coder::flag_type
            flag_type;

    public:

        /// @ingroup factory_layers
        /// The factory layer associated with this coder.
        /// In this case only needed to provide the max_payload_size()
        /// function.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory()
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
                { }

            /// @copydoc layer::max_header_size()
            uint32_t max_header_size() const
                {
                    return SuperCoder::factory::max_header_size() +
                        sizeof(flag_type) + sizeof(counter_type);
                }
        };

    public:

        /// Constructor
        base_systematic_encoder()
            : m_count(0),
              m_systematic(SystematicOn)
            { }

        /// @copydoc layer::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                /// Reset the state
                m_count = 0;
                m_systematic = SystematicOn;
            }

        /// @copydoc layer::encode(uint8_t*, uint8_t*)
        uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_header)
            {
                assert(symbol_data != 0);
                assert(symbol_header != 0);

                if(m_systematic && ( m_count < SuperCoder::symbols() ))
                {
                    return encode_systematic(symbol_data,
                                             symbol_header);
                }
                else
                {
                    return encode_non_systematic(symbol_data,
                                                 symbol_header);
                }
            }

        /// @return, true if the encoder is in systematic mode
        bool is_systematic_on() const
            {
                return m_systematic;
            }

        /// Set the encoder in systematic mode
        void set_systematic_on()
            {
                m_systematic = true;
            }

        /// Turns off systematic mode
        void set_systematic_off()
            {
                m_systematic = false;
            }

        /// @copydoc layer::header_size()
        uint32_t header_size() const
            {
                return SuperCoder::header_size() +
                    sizeof(flag_type) + sizeof(counter_type);
            }

        /// @return the number of systematic coded packets
        uint32_t systematic_count() const
            {
                return m_count;
            }

    protected:

        /// Encodes a systematic packet
        /// @copydoc linear_block_encode::encode_with_vector()
        uint32_t encode_systematic(uint8_t *symbol_data,
                                   uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                /// Flag systematic packet
                sak::big_endian::put<flag_type>(
                    systematic_base_coder::systematic_flag, symbol_id);

                /// Set the symbol id
                sak::big_endian::put<counter_type>(
                    m_count, symbol_id + sizeof(flag_type));

                /// Copy the symbol
                assert(m_count < SuperCoder::symbols());

                sak::mutable_storage dest =
                    sak::storage(symbol_data, SuperCoder::symbol_size());

                SuperCoder::copy_symbol(m_count, dest);

                ++m_count;

                return sizeof(flag_type) + sizeof(counter_type);
            }

        /// Encodes a non-systematic packets
        /// @copydoc linear_block_encode::encode_with_vector()
        uint32_t encode_non_systematic(uint8_t *symbol_data,
                                       uint8_t *symbol_id)
            {
                /// Flag non_systematic packet
                sak::big_endian::put<flag_type>(
                    systematic_base_coder::non_systematic_flag, symbol_id);

                uint32_t bytes_consumed = SuperCoder::encode(
                    symbol_data, symbol_id + sizeof(flag_type));

                return bytes_consumed + sizeof(flag_type);
            }

    protected:

        /// Keeps track of the number of symbol sent allows us to switch to
        /// non-systematic encoding after sending all source symbols
        /// systematically
        counter_type m_count;

        /// Allows the systematic mode to be disabled at runtime
        bool m_systematic;

    };

    template<class SuperCoder>
    class systematic_encoder :
        public base_systematic_encoder<true, SuperCoder>
    {};

    template<class SuperCoder>
    class non_systematic_encoder :
        public base_systematic_encoder<false, SuperCoder>
    {};


    /// Overload for the generic is_systematic_encoder_dispatch(...) function
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    /// @return true since this is an systematic encoder
    template<class SuperCoder>
    inline bool
    is_systematic_encoder_dispatch(const systematic_encoder<SuperCoder> *)
    {
        return true;
    }

    /// Overload for the generic is_systematic_on_dispatch(...) function
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    /// @return true if the encoder currently produces systematic symbols
    template<class SuperCoder>
    inline bool
    is_systematic_on_dispatch(systematic_encoder<SuperCoder> *e)
    {
        assert(e != 0);
        return e->is_systematic_on();
    }

    /// Overload for the generic set_systematic_off_dispatch(...) function
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    template<class SuperCoder>
    inline void
    set_systematic_off_dispatch(systematic_encoder<SuperCoder> *e)
    {
        assert(e != 0);
        e->set_systematic_off();
    }

    /// Overload for the generic set_systematic_on_dispatch(...) function
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    template<class SuperCoder>
    inline void
    set_systematic_on_dispatch(systematic_encoder<SuperCoder> *e)
    {
        assert(e != 0);
        e->set_systematic_on();
    }
}

#endif

