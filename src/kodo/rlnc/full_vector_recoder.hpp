// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_RECODER_HPP
#define KODO_RLNC_FULL_VECTOR_RECODER_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include "../linear_block_vector.hpp"
#include "../proxy_encoder.hpp"

namespace kodo
{
    /// Implements a simple recoding scheme for RLNC. Based on the
    /// random generator we re-combine previously received symbols
    /// to produce new re-encoded symbols.
    ///
    /// Note about sizes the proxy coder "bypasses" the existing
    /// coding layers. Thus to ensure correct behaviour the
    /// proxy coders symbol_id_size should be smaller than or
    /// equal to the "parent" coders. There is in principle nothing
    /// wrong with the proxy coder to need more space than the
    /// parent coder, but currently we do not support this.
    template<template <class> class ProxyCoder,
             template <class> class RandomGenerator,
             class SuperCoder>
    class full_vector_recoder : public SuperCoder
    {
    public:

        class proxy
            : public ProxyCoder<
                     proxy_encoder<
                     full_vector_recoder<ProxyCoder, RandomGenerator, SuperCoder
                         > > >
        {};

        /// The field we use
        typedef typename SuperCoder::field_type field_type;

        /// The value_type used to store the field elements
        typedef typename SuperCoder::value_type value_type;

        /// The encoding vector
        typedef typename SuperCoder::vector_type vector_type;

        /// The random generator used
        typedef RandomGenerator<value_type> random_generator;

        /// The rank of the current decoder
        using SuperCoder::m_rank;

    public:

        /// @see final_coder::construct(...)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                SuperCoder::construct(max_symbols, max_symbol_size);

                uint32_t max_vector_length = vector_type::length(max_symbols);
                m_recoding_vector.resize(max_vector_length);

                m_proxy.set_proxy(this);

                // Se note in class description
                assert(m_proxy.symbol_size() == SuperCoder::symbol_size());
                assert(m_proxy.symbol_id_size() <= SuperCoder::symbol_id_size());

                m_proxy.construct(max_symbols, max_symbol_size);
            }

        /// @see final_coder::initialize(...)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_proxy.initialize(symbols, symbol_size);
            }

        /// The recode functionality is special for Network Coding
        /// algorithms.
        uint32_t recode(uint8_t *s)
            {
                return m_proxy.encode(s);
            }

        uint32_t proxy_symbol_id_size()
            {
                return SuperCoder::vector_size();
            }

        uint32_t proxy_symbol_size()
            {
                return SuperCoder::symbol_size();
            }

        uint32_t proxy_encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                assert(symbol_data != 0);
                assert(symbol_id != 0);

                if(m_rank == 0)
                {
                    /// We do not have any symbols in our buffer
                    /// we have to simply output a zero encoding vector
                    /// and symbol
                    return SuperCoder::vector_size();
                }

                value_type *symbol
                    = reinterpret_cast<value_type*>(symbol_data);

                value_type *vector
                    = reinterpret_cast<value_type*>(symbol_id);

                uint32_t recode_vector_length = vector_type::length(m_rank);
                m_random.generate(&m_recoding_vector[0], recode_vector_length);

                uint32_t coefficient_index = 0;
                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    if(!SuperCoder::symbol_exists(i))
                        continue;

                    value_type coefficient =
                        vector_type::coefficient(coefficient_index,
                                                 &m_recoding_vector[0]);

                    if(coefficient)
                    {
                        value_type *vector_i = SuperCoder::vector( i );
                        value_type *symbol_i = reinterpret_cast<value_type*>(SuperCoder::symbol( i ));

                        if(fifi::is_binary<field_type>::value)
                        {
                            SuperCoder::add(vector, vector_i,
                                            SuperCoder::vector_length());

                            SuperCoder::add(symbol, symbol_i,
                                            SuperCoder::symbol_length());
                        }
                        else
                        {
                            SuperCoder::multiply_add(vector, vector_i,
                                                     coefficient,
                                                     SuperCoder::vector_length());

                            SuperCoder::multiply_add(symbol, symbol_i,
                                                     coefficient,
                                                     SuperCoder::symbol_length());
                        }
                    }

                    ++coefficient_index;
                    if(coefficient_index == m_rank)
                        break;

                }

                return SuperCoder::vector_size();

            }

    private:

        /// Buffer storing the RLNC "local encoding vector"
        std::vector<value_type> m_recoding_vector;

        /// Random generator producing coefficients for the local
        /// encoding vector
        random_generator m_random;

        /// The proxy
        proxy m_proxy;
    };
}

#endif

