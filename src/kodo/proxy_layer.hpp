// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

namespace kodo
{

    /// @brief Special layer used to create parallel stacks with
    ///       some shared layers.
    template<class FinalType, class MainStack>
    class proxy_layer
    {
    public:

        /// @copydoc layer::field_type
        typedef typename MainStack::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename MainStack::value_type value_type;

        /// Pointer type to the constructed coder
        typedef boost::shared_ptr<FinalType> pointer;

    public:

        /// @ingroup factory_layers
        /// Forwarding factory for the parallel proxy stack
        class factory
        {
        public:

            /// The factory type
            typedef typename FinalType::factory factory_type;

        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : m_factory_proxy(0),
                  m_stack_proxy(0)
            {
                (void) max_symbols;
                (void) max_symbol_size;
            }

            /// Sets the pointer to the proxy stack
            /// @param factory_proxy The stack where calls should be
            ///        forwarded.
            void set_factory_proxy(typename MainStack::factory* factory_proxy)
            {
                assert(factory_proxy != 0);
                assert(m_factory_proxy == 0);
                m_factory_proxy = factory_proxy;
            }

            void set_stack_proxy(MainStack* stack_proxy)
            {
                assert(stack_proxy != 0);
                m_stack_proxy = stack_proxy;
            }

            /// @copydoc layer::factory::build()
            pointer build()
            {
                assert(m_factory_proxy != 0);
                assert(m_stack_proxy != 0);

                pointer coder = boost::make_shared<FinalType>();

                coder->set_proxy(m_stack_proxy);

                factory_type *this_factory =
                    static_cast<factory_type*>(this);

                coder->construct(*this_factory);
                coder->initialize(*this_factory);

                return coder;
            }

            /// @copydoc layer::factory::max_symbols() const
            uint32_t max_symbols() const
            {
                assert(m_factory_proxy);
                return m_factory_proxy->max_symbols();
            }

            /// @copydoc layer::factory::max_symbol_size() const
            uint32_t max_symbol_size() const
            {
                assert(m_factory_proxy);
                return m_factory_proxy->max_symbol_size();
            }

            /// @copydoc layer::factory::max_block_size() const
            uint32_t max_block_size() const
            {
                assert(m_factory_proxy);
                return m_factory_proxy->max_block_size();
            }

            /// @copydoc layer::factory::max_header_size() const
            uint32_t max_header_size() const
            {
                assert(m_factory_proxy);
                return m_factory_proxy->max_header_size();
            }

            /// @copydoc layer::factory::max_id_size() const
            uint32_t max_id_size() const
            {
                assert(m_factory_proxy);
                return m_factory_proxy->max_id_size();
            }

            /// @copydoc layer::factory::max_payload_size() const
            uint32_t max_payload_size() const
            {
                assert(m_factory_proxy);
                return m_factory_proxy->max_payload_size();
            }

            /// @copydoc layer::factory::max_coefficients_size() const
            uint32_t max_coefficients_size() const
            {
                assert(m_factory_proxy);
                return m_factory_proxy->max_coefficients_size();
            }

            /// @copydoc layer::factory::symbols() const;
            uint32_t symbols() const
            {
                assert(m_factory_proxy);
                return m_factory_proxy->symbols();
            }

            /// @copydoc layer::factory::symbol_size() const;
            uint32_t symbol_size() const
            {
                assert(m_factory_proxy);
                return m_factory_proxy->symbol_size();
            }

        private:

            /// Pointer to the main stack's factory
            typename MainStack::factory* m_factory_proxy;

            /// Pointer to the main stack used during building a stack
            MainStack* m_stack_proxy;

        };

    public:

        /// Constructor
        proxy_layer()
            : m_proxy(0)
        { }


        /// Sets the pointer to the proxy stack
        /// @param proxy The stack where calls should be forwarded.
        void set_proxy(MainStack *proxy)
        {
            assert(proxy != 0);
            m_proxy = proxy;
        }

        /// @copydoc layer::construct(Factory&)
        template<class Factory>
        void construct(Factory &the_factory)
        {
            (void) the_factory;
        }

        /// @copydoc layer::initialize(Factory&)
        template<class Factory>
        void initialize(Factory &the_factory)
        {
            (void) the_factory;
        }

        //------------------------------------------------------------------
        // SYMBOL STORAGE API
        //------------------------------------------------------------------

        /// @copydoc layer::copy_symbols(const sak::mutable_storage&) const
        void copy_symbols(const sak::mutable_storage &dest) const
        {
            assert(m_proxy);
            m_proxy->copy_symbols(dest);
        }

        /// @copydoc layer::copy_symbol(
        ///              uint32_t,const sak::mutable_storage&) const
        void copy_symbol(uint32_t index,
                         const sak::mutable_storage &dest) const
        {
            assert(m_proxy);
            m_proxy->copy_symbol(index, dest);
        }

        /// @copydoc layer::symbol(uint32_t)
        uint8_t* symbol(uint32_t index)
        {
            assert(m_proxy);
            return m_proxy->symbol(index);
        }

        /// @copydoc layer::symbol(uint32_t) const
        const uint8_t* symbol(uint32_t index) const
        {
            assert(m_proxy);
            return m_proxy->symbol(index);
        }

        /// @copydoc layer::symbol_value(uint32_t)
        value_type* symbol_value(uint32_t index)
        {
            assert(m_proxy);
            return m_proxy->symbol_value(index);
        }

        /// @copydoc layer::symbol_value(uint32_t) const
        const value_type* symbol_value(uint32_t index) const
        {
            assert(m_proxy);
            return m_proxy->symbol_value(index);
        }

        /// @copydoc layer::symbols() const
        uint32_t symbols() const
        {
            assert(m_proxy);
            return m_proxy->symbols();
        }

        /// @copydoc layer::symbol_size() const
        uint32_t symbol_size() const
        {
            assert(m_proxy);
            return m_proxy->symbol_size();
        }

        /// @copydoc layer::symbol_length() const
        uint32_t symbol_length() const
        {
            assert(m_proxy);
            return m_proxy->symbol_length();
        }

        /// @copydoc layer::block_size() const
        uint32_t block_size() const
        {
            assert(m_proxy);
            return m_proxy->block_size();
        }

        /// @copydoc layer::symbols_available() const
        uint32_t symbols_available() const
        {
            assert(m_proxy);
            return m_proxy->symbols_available();
        }

        /// @copydoc layer::symbols_initialized() const
        uint32_t symbols_initialized() const
        {
            assert(m_proxy);
            return m_proxy->symbols_initialized();
        }

        /// @copydoc layer::is_symbols_available() const
        bool is_symbols_available() const
        {
            assert(m_proxy);
            return m_proxy->is_symbols_available();
        }

        /// @copydoc layer::is_symbols_initialized() const
        bool is_symbols_initialized() const
        {
            assert(m_proxy);
            return m_proxy->is_symbols_initialized();
        }

        /// @copydoc layer::is_symbol_available(uint32_t) const
        bool is_symbol_available(uint32_t symbol_index) const
        {
            assert(m_proxy);
            return m_proxy->is_symbol_available(symbol_index);
        }

        /// @copydoc layer::is_symbol_initialized(uint32_t) const
        bool is_symbol_initialized(uint32_t symbol_index) const
        {
            assert(m_proxy);
            return m_proxy->is_symbol_initialized(symbol_index);
        }

        //------------------------------------------------------------------
        // COEFFICIENT STORAGE API
        //------------------------------------------------------------------

        /// @copydoc layer::coefficients_size() const
        uint32_t coefficients_size() const
        {
            assert(m_proxy);
            return m_proxy->coefficients_size();
        }

        /// @copydoc layer::coefficients_length() const
        uint32_t coefficients_length() const
        {
            assert(m_proxy);
            return m_proxy->coefficients_length();
        }

        /// @copydoc layer::coefficients_value(uint32_t)
        value_type* coefficients_value(uint32_t index)
        {
            assert(m_proxy);
            return m_proxy->coefficients_value(index);
        }

        /// @copydoc layer::coefficients_value(uint32_t) const
        const value_type* coefficients_value(uint32_t index) const
        {
            assert(m_proxy);
            return m_proxy->coefficients_value(index);
        }

        /// @copydoc layer::coefficients(uint32_t)
        uint8_t* coefficients(uint32_t index)
        {
            assert(m_proxy);
            return m_proxy->coefficients(index);
        }

        /// @copydoc layer::coefficients(uint32_t) const
        const uint8_t* coefficients(uint32_t index) const
        {
            assert(m_proxy);
            return m_proxy->coefficients(index);
        }

        //------------------------------------------------------------------
        // FINITE FIELD API
        //------------------------------------------------------------------

        /// @copydoc layer::multiply(value_type*,value_type,uint32_t)
        void multiply(value_type *symbol_dest, value_type coefficient,
                      uint32_t symbol_length)
        {
            assert(m_proxy);
            m_proxy->multiply(symbol_dest, coefficient, symbol_length);
        }

        /// @copydoc layer::multipy_add(value_type *, const value_type*,
        ///                             value_type, uint32_t)
        void multiply_add(
            value_type *symbol_dest, const value_type *symbol_src,
            value_type coefficient, uint32_t symbol_length)
        {
            assert(m_proxy);
            m_proxy->multiply_add(symbol_dest, symbol_src,
                                  coefficient, symbol_length);
        }

        /// @copydoc layer::add(value_type*, const value_type *, uint32_t)
        void add(value_type *symbol_dest, const value_type *symbol_src,
                 uint32_t symbol_length)
        {
            assert(m_proxy);
            m_proxy->add(symbol_dest, symbol_src, symbol_length);
        }

        /// @copydoc layer::multiply_subtract(value_type*,
        ///                                   const value_type*,
        ///                                   value_type, uint32_t)
        void multiply_subtract(
            value_type *symbol_dest, const value_type *symbol_src,
            value_type coefficient, uint32_t symbol_length)
        {
            assert(m_proxy);
            m_proxy->multiply_subtract(symbol_dest, symbol_src,
                                       coefficient, symbol_length);
        }

        /// @copydoc layer::subtract(
        ///              value_type*,const value_type*, uint32_t)
        void subtract(value_type *symbol_dest, const value_type *symbol_src,
                      uint32_t symbol_length)
        {
            assert(m_proxy);
            m_proxy->subtract(symbol_dest, symbol_src, symbol_length);
        }

        /// @copydoc layer::invert(value_type)
        value_type invert(value_type value)
        {
            assert(m_proxy);
            return m_proxy->invert(value);
        }

        //------------------------------------------------------------------
        // CODEC API
        //------------------------------------------------------------------

        /// @copydoc layer::encode_symbol(uint8_t*, uint8_t*)
        void encode_symbol(uint8_t *symbol_data, uint8_t *coefficients)
        {
            assert(m_proxy);
            m_proxy->encode_symbol(symbol_data, coefficients);
        }

        /// @copydoc layer::encode_symbol(uint8_t*,uint32_t)
        void encode_symbol(uint8_t *symbol_data, uint32_t symbol_index)
        {
            assert(m_proxy);
            m_proxy->encode_symbol(symbol_data, symbol_index);
        }

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data, uint8_t *coefficients)
        {
            assert(m_proxy);
            m_proxy->decode_symbol(symbol_data, coefficients);
        }

        /// @copydoc layer::decode_symbol(uint8_t*, uint32_t)
        void decode_symbol(uint8_t *symbol_data, uint32_t symbol_index)
        {
            assert(m_proxy);
            m_proxy->decode_symbol(symbol_data, symbol_index);
        }

        /// @copydoc layer::is_complete() const
        bool is_complete() const
        {
            assert(m_proxy);
            return m_proxy->is_complete();
        }

        /// @copydoc layer::rank() const
        uint32_t rank() const
        {
            assert(m_proxy);
            return m_proxy->rank();
        }

        /// @copydoc layer::symbol_pivot(uint32_t) const
        bool symbol_pivot(uint32_t index) const
        {
            assert(m_proxy);
            return m_proxy->symbol_pivot(index);
        }

    protected:

        /// Pointer to the main stack
        MainStack *m_proxy;

    };

}


