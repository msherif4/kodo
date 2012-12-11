// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_FINAL_CODER_FACTORY_POOL_HPP
#define KODO_FINAL_CODER_FACTORY_POOL_HPP

#include <stdint.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <sak/resource_pool.hpp>

#include <fifi/fifi_utils.hpp>

namespace kodo
{
    /// Terminates the layered coder and contains the coder final factory. The
    /// pool factory uses a memory pool to recycle encoders/decoders, and
    /// thereby minimize memory consumption.
    template<class FINAL, class Field>
    class final_coder_factory_pool : boost::noncopyable
    {
    public:

        /// Define the field type
        typedef Field field_type;

        /// The value type used i.e. the finite field elements are stored using
        /// this data type
        typedef typename field_type::value_type value_type;

        /// Pointer type to the constructed coder
        typedef boost::shared_ptr<FINAL> pointer;

        /// The final factory
        class factory
        {
        public:

            /// @copydoc final_coder_factory::factory::factory()
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : m_max_symbols(max_symbols),
                  m_max_symbol_size(max_symbol_size),
                  m_pool(boost::bind(&factory::make_coder, max_symbols,
                                     max_symbol_size))
                {
                    assert(m_max_symbols > 0);
                    assert(m_max_symbol_size > 0);
                }

            /// @copydoc final_coder_factory::factory::build()
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    assert(symbols > 0);
                    assert(symbol_size > 0);

                    pointer coder = m_pool.allocate();
                    coder->initialize(symbols, symbol_size);

                    return coder;
                }

            /// @copydoc final_coder_factory::factory::max_symbols()
            uint32_t max_symbols() const
                { return m_max_symbols; }

            /// @copydoc final_coder_factory::factory::max_symbol_size()
            uint32_t max_symbol_size() const
                { return m_max_symbol_size; }

        private:

            /// @copydoc final_coder_factory::factory::pointer()
            static pointer make_coder(uint32_t max_symbols,
                                      uint32_t max_symbol_size)
                {
                    assert(max_symbols > 0);
                    assert(max_symbol_size > 0);

                    pointer coder = boost::make_shared<FINAL>();
                    coder->construct(max_symbols, max_symbol_size);

                    return coder;
                }

        private:

            /// The maximum number of symbols
            uint32_t m_max_symbols;

            /// The maximum symbol size
            uint32_t m_max_symbol_size;

            /// Resource pool for the coders
            sak::resource_pool<FINAL> m_pool;

        };

    public:

        /// @copydoc final_coder_factory::construct()
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                // This is just the factory layer so we do nothing

                (void) max_symbols;
                (void) max_symbol_size;
            }

        /// @copydoc final_coder_factory::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                // This is just the factory layer so we do nothing

                (void) symbols;
                (void) symbol_size;
            }
    };
}

#endif

