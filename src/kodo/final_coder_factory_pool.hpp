// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_FINAL_CODER_FACTORY_POOL_HPP
#define KODO_FINAL_CODER_FACTORY_POOL_HPP

#include <cstdint>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <sak/resource_pool.hpp>

#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup factory_layers
    /// Terminates the layered coder and contains the coder final
    /// factory. The pool factory uses a memory pool to recycle
    /// encoders/decoders, and thereby minimize memory consumption.
    template<class FINAL>
    class final_coder_factory_pool : boost::noncopyable
    {
    public:

        /// Pointer type to the constructed coder
        typedef boost::shared_ptr<FINAL> pointer;

        /// The final factory
        class factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : m_pool(boost::bind(&factory::make_coder, max_symbols,
                                     max_symbol_size))
                {
                    assert(max_symbols > 0);
                    assert(max_symbol_size > 0);
                }

            /// @copydoc layer::factory::build(uint32_t,uint32_t)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    assert(symbols > 0);
                    assert(symbol_size > 0);

                    pointer coder = m_pool.allocate();
                    coder->initialize(symbols, symbol_size);

                    return coder;
                }

        private:

            /// Factory function used by the resource pool to
            /// build new coders if needed.
            /// @param max_symbols The maximum symbols that are supported
            /// @param max_symbol_size The maximum size of a symbol in
            ///        bytes
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

            /// Resource pool for the coders
            sak::resource_pool<FINAL> m_pool;

        };

    public:

        /// @copydoc layer::construct(uint32_t,uint32_t)
        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                // This is the final factory layer so we do nothing
                (void) max_symbols;
                (void) max_symbol_size;
            }

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                // This is the final factory layer so we do nothing
                (void) symbols;
                (void) symbol_size;
            }
    };
}

#endif

