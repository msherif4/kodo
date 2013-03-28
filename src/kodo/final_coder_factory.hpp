// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_FINAL_CODER_FACTORY_HPP
#define KODO_FINAL_CODER_FACTORY_HPP

#include <cstdint>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup factory_layers
    /// Terminates the layered coder and contains the coder allocation
    /// policy
    template<class FinalType>
    class final_coder_factory : boost::noncopyable
    {
    public:

        /// Pointer type to the constructed coder
        typedef boost::shared_ptr<FinalType> pointer;

        /// The final factory
        class factory
        {
        public:

            /// The factory type
            typedef typename FinalType::factory factory_type;


            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : m_max_symbols(max_symbols),
                  m_max_symbol_size(max_symbol_size)
                {
                    assert(m_max_symbols > 0);
                    assert(m_max_symbol_size > 0);
                }

            /// @copydoc layer::factory::build(uint32_t,uint32_t)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    assert(symbols > 0);
                    assert(symbol_size > 0);

                    factory_type *this_factory = static_cast<factory_type*>(this);

                    pointer coder = boost::make_shared<FinalType>();

                    coder->test(*this_factory);
                    coder->construct(m_max_symbols, m_max_symbol_size);
                    coder->initialize(symbols, symbol_size);

                    return coder;
                }

        private:

            /// The maximum number of symbols
            uint32_t m_max_symbols;

            /// The maximum symbol size
            uint32_t m_max_symbol_size;
        };

    public:

        void test(factory &/*ok*/)
            {
            }

        /// @copydoc layer::construct(uint32_t,uint32_t)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                /// This is the final factory layer so we do nothing
                (void) max_symbols;
                (void) max_symbol_size;
            }

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                /// This is the final factory layer so we do nothing
                (void) symbols;
                (void) symbol_size;
            }
    };
}

#endif

