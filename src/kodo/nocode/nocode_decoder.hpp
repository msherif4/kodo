// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_NOCODE_NOCODE_DECODER_HPP
#define KODO_NOCODE_NOCODE_DECODER_HPP

#include <vector>

#include <sak/storage.hpp>

namespace kodo
{

    /// @ingroup codec_layers
    ///
    /// Implements the functionality to consume an un-coded / systematic
    /// symbol. This can be useful in the following cases:
    /// 1) When implementing systematic codes
    /// 2) When creating a un-coded reference scheme for comparison with an
    ///    actual FEC scheme.
    /// 3) In unit tests
    template<class SuperCoder>
    class nocode_decoder : public SuperCoder
    {
    public:

        /// @copydoc layer::factory
        typedef typename SuperCoder::factory factory;

    public:

        /// @copydoc layer::construct(uint32_t, uint32_t)
        void construct(factory &the_factory)
            {
                SuperCoder::construct(the_factory);
                m_uncoded.resize(the_factory.max_symbols(), false);
            }

        /// @copydoc layer::initialize(uint32_t, uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                std::fill_n(m_uncoded.begin(), symbols, false);
                m_rank = 0;
            }

        /// @copydoc layer::decode_symbol(uint8_t*, uint32_t)
        void decode_symbol(uint8_t *symbol_data,
                           uint32_t symbol_index)
            {
                assert(symbol_index < SuperCoder::symbols());
                assert(symbol_data != 0);

                if(m_uncoded[symbol_index])
                {
                    return;
                }

                ++m_rank;
                m_uncoded[ symbol_index ] = true;

                sak::const_storage symbol_storage =
                    sak::storage(symbol_data, SuperCoder::symbol_size());

                SuperCoder::set_symbol(symbol_index, symbol_storage);

            }

        /// @copydoc layer::is_complete() const
        bool is_complete() const
            {
                return m_rank == SuperCoder::symbols();
            }

        /// @copydoc layer::rank() const
        uint32_t rank() const
            {
                return m_rank;
            }

    protected:

        /// The current rank of the decoder
        uint32_t m_rank;

        /// Tracks whether a symbol is contained which
        /// is fully decoded
        std::vector<bool> m_uncoded;

    };

}

#endif

