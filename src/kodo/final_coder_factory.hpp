// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

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
    class final_coder_factory
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

        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size) :
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
                factory_type *this_factory =
                    static_cast<factory_type*>(this);

                pointer coder = boost::make_shared<FinalType>();

                coder->construct(*this_factory);
                coder->initialize(*this_factory);

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

            // /// @copydoc layer::factory::symbols() const;
            // uint32_t symbols() const
            // {
            //     return m_symbols;
            // }

            // /// @copydoc layer::factory::symbol_size() const;
            // uint32_t symbol_size() const
            // {
            //     return m_symbol_size;
            // }

            // /// @copydoc layer::factory::set_symbols(uint32_t)
            // void set_symbols(uint32_t symbols)
            // {
            //     assert(symbols > 0);
            //     assert(symbols <= m_max_symbols);

            //     m_symbols = symbols;
            // }

            // /// @copydoc layer::factory::set_symbol_size(uint32_t)
            // void set_symbol_size(uint32_t symbol_size)
            // {
            //     assert(symbol_size > 0);
            //     assert(symbol_size <= m_max_symbol_size);

            //     m_symbol_size = symbol_size;
            // }

        private: // Make non-copyable

            /// Copy constructor
            factory(const factory&);

            /// Copy assignment
            const factory& operator=(const factory&);


        private:

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

        /// @copydoc layer::construct(factory&)
        void construct(factory &the_factory)
        {
            /// This is the final factory layer so we do nothing
            (void) the_factory;
        }

        /// @copydoc layer::initialize(factory&)
        void initialize(factory &the_factory)
        {
            /// This is the final factory layer so we do nothing
            (void) the_factory;
        }

    protected:

        /// Constructor
        final_coder_factory()
        { }

        /// Destructor
        ~final_coder_factory()
        { }

    private: // Make non-copyable

        /// Copy constructor
        final_coder_factory(const final_coder_factory&);

        /// Copy assignment
        const final_coder_factory& operator=(
            const final_coder_factory&);
    };
}


