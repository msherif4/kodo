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
    template<class FinalType>
    class final_coder_factory_pool : boost::noncopyable
    {
    public:

        /// Pointer type to the constructed coder
        typedef boost::shared_ptr<FinalType> pointer;



        /// @ingroup factory_layers
        /// The final factory
        class factory
        {
        public:

            /// The factory type
            typedef typename FinalType::factory factory_type;

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : m_pool(boost::bind(&factory::make_coder, max_symbols,
                                     max_symbol_size, this)),
                  m_max_symbols(max_symbols),
                  m_max_symbol_size(max_symbol_size),
                  m_symbols(max_symbols),
                  m_symbol_size(max_symbol_size)
                {
                    assert(m_max_symbols > 0);
                    assert(m_max_symbol_size > 0);
                    assert(m_symbols > 0);
                    assert(m_symbol_size > 0);
                }

            /// @copydoc layer::factory::build()
            pointer build()
                {
                    pointer coder = m_pool.allocate();
                    coder->initialize(m_symbols, m_symbol_size);

                    return coder;
                }

            /// @copydoc layer::factory::max_symbols() const;
            uint32_t max_symbols() const
                {
                    return m_max_symbols;
                }

            /// @copydoc layer::factory::max_symbol_size() const;
            uint32_t max_symbol_size() const
                {
                    return m_max_symbol_size;
                }

            /// @copydoc layer::factory::symbols() const;
            uint32_t symbols() const
                {
                    return m_symbols;
                }

            /// @copydoc layer::factory::symbol_size() const;
            uint32_t symbol_size() const
                {
                    return m_symbol_size;
                }

            /// @copydoc layer::factory::set_symbols(uint32_t)
            void set_symbols(uint32_t symbols)
                {
                    assert(symbols > 0);
                    assert(symbols < m_max_symbols);

                    m_symbols = symbols;
                }

            /// @copydoc layer::factory::set_symbol_size(uint32_t)
            void set_symbol_size(uint32_t symbol_size)
                {
                    assert(symbol_size > 0);
                    assert(symbol_size < m_max_symbol_size);

                    m_symbol_size = symbol_size;
                }

        private:

            /// Factory function used by the resource pool to
            /// build new coders if needed.
            /// @param max_symbols The maximum symbols that are supported
            /// @param max_symbol_size The maximum size of a symbol in
            ///        bytes
            static pointer make_coder(uint32_t max_symbols,
                                      uint32_t max_symbol_size,
                                      factory *f_ptr)
                {
                    assert(max_symbols > 0);
                    assert(max_symbol_size > 0);

                    factory_type *this_factory =
                        static_cast<factory_type*>(f_ptr);

                    pointer coder = boost::make_shared<FinalType>();
                    coder->construct(*this_factory);



                    return coder;
                }

        private:

            /// Resource pool for the coders
            sak::resource_pool<FinalType> m_pool;

            /// The maximum number of symbols
            uint32_t m_max_symbols;

            /// The maximum symbol size
            uint32_t m_max_symbol_size;

            /// The number of symbols used
            uint32_t m_symbols;

            /// The symbol size used
            uint32_t m_symbol_size;

        };

    public:


        /// @copydoc layer::construct(factory &)
        void construct(factory &the_factory)
            {
                // This is the final factory layer so we do nothing
                (void) the_factory;
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

