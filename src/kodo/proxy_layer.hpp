// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_PROXY_LAYER_HPP
#define KODO_PROXY_LAYER_HPP

#include <cstdint>

namespace kodo
{

    /// @brief Special layer used to create parallel stacks with
    ///       some shared layers.
    template<class FinalType, class Proxy>
    class proxy_layer
    {
    public:

        /// @copydoc layer::field_type
        typedef typename Proxy::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename Proxy::value_type value_type;

        /// Pointer type to the constructed coder
        typedef boost::shared_ptr<FinalType> pointer;

    public:

        class factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                {
                    (void) max_symbols;
                    (void) max_symbol_size;
                }

            /// Sets the pointer to the proxy stack
            /// @param proxy The stack where calls should be forwarded.
            void set_proxy(typename Proxy::factory *proxy)
                {
                    assert(proxy != 0);
                    m_proxy_factory = proxy;
                }

            /// @copydoc layer::factory::build(uint32_t, uint32_t)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder = boost::make_shared<FinalType>();
                    return coder;
                }

            uint32_t max_symbols() const
                {
                    assert(m_proxy_factory);
                    return m_proxy_factory->max_symbols();
                }

            uint32_t max_symbol_size() const
                {
                    assert(m_proxy_factory);
                    return m_proxy_factory->max_symbol_size();
                }

            uint32_t max_block_size() const
                {
                    assert(m_proxy_factory);
                    return m_proxy_factory->max_block_size();
                }

            uint32_t max_header_size()
                {
                    assert(m_proxy_factory);
                    return m_proxy_factory->max_header_size();
                }

            uint32_t max_id_size()
                {
                    assert(m_proxy_factory);
                    return m_proxy_factory->max_id_size();
                }

            uint32_t max_payload_size() const
                {
                    assert(m_proxy_factory);
                    return m_proxy_factory->max_payload_size();
                }

            uint32_t max_coefficients_size() const
                {
                    assert(m_proxy_factory);
                    return m_proxy_factory->max_coefficients_size();
                }

        private:

            typename Proxy::factory *m_proxy_factory;

        };

    public:

        /// Constructor
        proxy_layer()
            : m_proxy(0)
            { }


        /// Sets the pointer to the proxy stack
        /// @param proxy The stack where calls should be forwarded.
        void set_proxy(Proxy *proxy)
            {
                assert(proxy != 0);
                m_proxy = proxy;
            }

        void construct(uint32_t max_symbols, uint32_t max_symbol_size)
            {
                (void) max_symbols;
                (void) max_symbol_size;
            }

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                (void) symbols;
                (void) symbol_size;
            }

        //------------------------------------------------------------------
        // SYMBOL STORAGE API
        //------------------------------------------------------------------

        void copy_symbols(const sak::mutable_storage &dest) const
            {
                assert(m_proxy);
                m_proxy->copy_symbols(dest);
            }


        void copy_symbol(uint32_t index,
                         const sak::mutable_storage &dest) const
            {
                assert(m_proxy);
                m_proxy->copy_symbol(index, dest);
            }

        uint8_t* symbol(uint32_t index)
            {
                assert(m_proxy);
                return m_proxy->symbol(index);
            }

        const uint8_t* symbol(uint32_t index) const
            {
                assert(m_proxy);
                return m_proxy->symbol(index);
            }

        value_type* symbol_value(uint32_t index)
            {
                assert(m_proxy);
                return m_proxy->symbol_value(index);
            }

        const value_type* symbol_value(uint32_t index) const
            {
                assert(m_proxy);
                return m_proxy->symbol_value(index);
            }

        uint32_t symbols() const
            {
                assert(m_proxy);
                return m_proxy->symbols();
            }

        uint32_t symbol_size() const
            {
                assert(m_proxy);
                return m_proxy->symbol_size();
            }

        uint32_t symbol_length() const
            {
                assert(m_proxy);
                return m_proxy->symbol_length();
            }

        uint32_t block_size() const
            {
                assert(m_proxy);
                return m_proxy->block_size();
            }

        /// @copydoc layer::symbol_exists(uint32_t) const
        bool symbol_exists(uint32_t index) const
            {
                assert(m_proxy);
                return m_proxy->symbol_exists(index);
            }

        /// @copydoc layer::symbol_count() const
        uint32_t symbol_count() const
            {
                assert(m_proxy);
                return m_proxy->symbol_count();
            }

        /// @copydoc layer::is_storage_full() const
        bool is_storage_full() const
            {
                assert(m_proxy);
                return m_proxy->is_storage_full();
            }

        //------------------------------------------------------------------
        // COEFFICIENT STORAGE API
        //------------------------------------------------------------------

        uint32_t coefficients_size() const
            {
                assert(m_proxy);
                return m_proxy->coefficients_size();
            }

        uint32_t coefficients_length() const
            {
                assert(m_proxy);
                return m_proxy->coefficients_length();
            }

        value_type* coefficients_value(uint32_t index)
            {
                assert(m_proxy);
                return m_proxy->coefficients_value(index);
            }

        const value_type* coefficients_value(uint32_t index) const
            {
                assert(m_proxy);
                return m_proxy->coefficients_value(index);
            }


        uint8_t* coefficients(uint32_t index)
            {
                assert(m_proxy);
                return m_proxy->coefficients(index);
            }

        const uint8_t* coefficients(uint32_t index) const
            {
                assert(m_proxy);
                return m_proxy->coefficients(index);
            }

        //------------------------------------------------------------------
        // MATH API
        //------------------------------------------------------------------


        void multiply(value_type *symbol_dest, value_type coefficient,
                      uint32_t symbol_length)
            {
                assert(m_proxy);
                m_proxy->multiply(symbol_dest, coefficient, symbol_length);
            }

        void multiply_add(
            value_type *symbol_dest, const value_type *symbol_src,
            value_type coefficient, uint32_t symbol_length)
            {
                assert(m_proxy);
                m_proxy->multiply_add(symbol_dest, symbol_src,
                                      coefficient, symbol_length);
            }

        void add(value_type *symbol_dest, const value_type *symbol_src,
                 uint32_t symbol_length)
            {
                assert(m_proxy);
                m_proxy->add(symbol_dest, symbol_src, symbol_length);
            }

        void multiply_subtract(
            value_type *symbol_dest, const value_type *symbol_src,
            value_type coefficient, uint32_t symbol_length)
            {
                assert(m_proxy);
                m_proxy->multiply_subtract(symbol_dest, symbol_src,
                                           coefficient, symbol_length);
            }

        void subtract(value_type *symbol_dest, const value_type *symbol_src,
                      uint32_t symbol_length)
            {
                assert(m_proxy);
                m_proxy->subtract(symbol_dest, symbol_src, symbol_length);
            }

        value_type invert(value_type value)
            {
                assert(m_proxy);
                return m_proxy->invert(value);
            }


    protected:

        Proxy *m_proxy;
    };

}

#endif

