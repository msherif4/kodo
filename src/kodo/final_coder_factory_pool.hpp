// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <sak/resource_pool.hpp>

namespace kodo
{

    /// @ingroup factory_layers
    /// Terminates the layered coder and contains the coder final
    /// factory. The pool factory uses a memory pool to recycle
    /// encoders/decoders, and thereby minimize memory consumption.
    template<class FinalType>
    class final_coder_factory_pool
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
            factory(uint32_t max_symbols, uint32_t max_symbol_size) :
                m_pool(boost::bind(&factory::make_coder, this))
            {
                (void) max_symbols;
                (void) max_symbol_size;
            }

            /// @copydoc layer::factory::build()
            pointer build()
            {
                factory_type *this_factory =
                    static_cast<factory_type*>(this);

                pointer coder = m_pool.allocate();
                coder->initialize(*this_factory);

                return coder;
            }

            /// @return A reference to the internal resource pool
            const sak::resource_pool<FinalType>& pool() const
            {
                return m_pool;
            }

            /// @return A reference to the internal resource pool
            sak::resource_pool<FinalType>& pool()
            {
                return m_pool;
            }


        private: // Make non-copyable

            /// Copy constructor
            factory(const factory&);

            /// Copy assignment
            const factory& operator=(const factory&);

        private:

            /// Factory function used by the resource pool to
            /// build new coders if needed.
            /// @param max_symbols The maximum symbols that are supported
            /// @param max_symbol_size The maximum size of a symbol in
            ///        bytes
            static pointer make_coder(factory *f_ptr)
            {
                factory_type *this_factory =
                    static_cast<factory_type*>(f_ptr);

                pointer coder = boost::make_shared<FinalType>();
                coder->construct(*this_factory);

                return coder;
            }

        private:

            /// Resource pool for the coders
            sak::resource_pool<FinalType> m_pool;

        };

    public:

        /// @copydoc layer::construct(Factory&)
        template<class Factory>
        void construct(Factory& the_factory)
        {
            // This is the final factory layer so we do nothing
            (void) the_factory;
        }

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory& the_factory)
        {
            // This is the final factory layer so we do nothing
            (void) the_factory;
        }

    protected:

        /// Constructor
        final_coder_factory_pool()
        { }

        /// Destructor
        ~final_coder_factory_pool()
        { }

    private: // Make non-copyable

        /// Copy constructor
        final_coder_factory_pool(const final_coder_factory_pool&);

        /// Copy assignment
        const final_coder_factory_pool& operator=(
            const final_coder_factory_pool&);

    };
}


