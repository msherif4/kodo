// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include <sak/storage.hpp>

#include <fifi/is_binary.hpp>
#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup codec_layers
    /// @brief This layer can be added to a decoding stack to track
    ///        when partial decoding is possible i.e. when symbols in
    ///        decoder are fully decoded even though the full data
    ///        block has not been sent.
    ///
    /// Keeping track when partial decoding is possible can be done
    /// in several ways. This layer will "monitor" the incoming
    /// coding coefficients, using the information stored in the
    /// it is possible to detect cases where all symbols are fully
    /// decoded.
    ///
    /// Imagine we have observed the following two coding vectors:
    ///
    ///  1: 1 0 0 0
    ///  2: 0 1 1 0
    ///         ^
    ///         |
    ///         +----+ Last non-zero position
    ///
    /// This matrix has a rank of 2 but since the last non-zero coefficient
    /// is found at index 2 we cannot yet decode (packet two is still coded).
    ///
    /// However if we receive on more packet such that the matrix becomes e.g.
    ///
    ///  1: 1 0 0 0
    ///  2: 0 1 1 0
    ///  3: 1 0 1 0
    ///         ^
    ///         |
    ///         +----+ Last non-zero position
    ///
    /// We see that the matrix has rank 3 and the last non-zero coefficient is
    /// still at index 2. We therefore must be able to decode the three packets.
    ///
    /// One word of caution, this does of course require that the decoder used
    /// will always decode all packets to the degree possible. Some decoders
    /// such as the linear_block_decoder_delayed will not perform backward
    /// substitution until the matrix reaches full rank. That type of decoder
    /// are therefore not compatible with this layer.
    template<class SuperCoder>
    class partial_decoding_tracker : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

        /// @copydoc layer::factory
        typedef typename SuperCoder::factory factory;

    public:

        /// Constructor
        partial_decoding_tracker()
            : m_largest_nonzero_index(0)
        { }

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            SuperCoder::initialize(the_factory);
            m_largest_nonzero_index = 0;
        }

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data,
                           uint8_t *symbol_coefficients)
        {
            assert(symbol_data != 0);
            assert(symbol_coefficients != 0);

            // Inspect the coding coefficients to find the maximum
            // position where we have a non-zero value
            value_type *coefficients
                = reinterpret_cast<value_type*>(symbol_coefficients);

            // We inspect the coefficients from the back i.e. if we
            // are given the following coding vector:
            //
            // 23 0 44 213 0 231 0
            //                ^
            //                |
            //                +-----+ max position of non-zero
            //                        coefficient
            //
            // Starting from the back will in many cases e.g. for uniform
            // coding vectors be better, since we are looking for the last
            // non-zero element.
            assert(SuperCoder::symbols() > m_largest_nonzero_index);

            for (uint32_t i = SuperCoder::symbols();
                 i --> m_largest_nonzero_index;)
            {
                value_type current_coefficient
                    = fifi::get_value<field_type>(coefficients, i);

                if (current_coefficient)
                {
                    m_largest_nonzero_index = i;
                    break;
                }
            }

            SuperCoder::decode_symbol(symbol_data, symbol_coefficients);
        }

        /// @copydoc layer::decode_symbol(uint8_t*, uint32_t)
        void decode_symbol(uint8_t *symbol_data,
                           uint32_t symbol_index)
        {
            assert(symbol_index < SuperCoder::symbols());
            assert(symbol_data != 0);

            m_largest_nonzero_index =
                std::max(symbol_index, m_largest_nonzero_index);

            SuperCoder::decode_symbol(symbol_data, symbol_index);
        }

        /// @return True if the matrix should be partially decoded.
        bool is_partial_complete() const
        {
            return SuperCoder::rank() == m_largest_nonzero_index + 1;
        }

        /// @return The position of the last seen non-zero coefficients
        uint32_t largest_nonzero_index() const
        {
            return m_largest_nonzero_index;
        }

    protected:

        /// The largest index seen with a nonzero coefficient
        uint32_t m_largest_nonzero_index;

    };

}

