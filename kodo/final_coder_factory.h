// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_FINAL_CODER_FACTORY_H
#define KODO_FINAL_CODER_FACTORY_H

#include <stdint.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <fifi/fifi_utils.h>

namespace kodo
{    
    /// Terminates the layered coder and contains the coder allocation
    /// policy
    template<class FINAL, class Field>
    class final_coder_factory : boost::noncopyable
    {
    public:

        /// Define the field type
        typedef Field field_type;

        /// The value type used i.e. the finite field elements
        /// are stored using this data type
        typedef typename field_type::value_type value_type;

        /// Pointer type to the constructed coder
        typedef boost::shared_ptr<FINAL> pointer;

        /// The final factory
        class factory
        {
        public:
            
            /// Constructor
            /// @param max_symbols the maximum symbols this coder can expect
            /// @param max_symbol_size the maximum size of a symbol in bytes
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : m_max_symbols(max_symbols),
                  m_max_symbol_size(max_symbol_size)
                {
                    assert(m_max_symbols > 0);
                    assert(m_max_symbol_size > 0);
                }
        
            /// Builds the actual coder
            /// @param symbols the symbols this coder will use
            /// @param symbol_size the size of a symbol in bytes
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    assert(symbols > 0);
                    assert(symbol_size > 0);
                    
                    pointer coder = boost::make_shared<FINAL>();
                    
                    coder->construct(m_max_symbols, m_max_symbol_size);
                    coder->initialize(symbols, symbol_size);
                    
                    return coder;
                }
            
            /// @return the maximum number of symbols in a block
            uint32_t max_symbols() const
                {
                    return m_max_symbols;
                }
            
            /// @return the maximum symbol size in bytes
            uint32_t max_symbol_size() const
                {
                    return m_max_symbol_size;
                }
            
        private:
            
            /// The maximum number of symbols
            uint32_t m_max_symbols;
            
            /// The maximum symbol size
            uint32_t m_max_symbol_size;
            
        };

    public:

        /// Constructs the coder with the maximum parameters it will ever
        /// see. A coder may only be constructed ONCE, but initialized many
        /// times.
        /// @param max_symbols the maximum symbols this coder can expect
        /// @param max_symbol_size the maximum size of a symbol in bytes
        void construct(uint32_t /*max_symbols*/, uint32_t /*max_symbol_size*/)
            {
                /// This is just the factory layer so we do nothing
            }

        /// Initializes the coder
        /// @param symbols the number of symbols the coder should store
        /// @param symbol_size the size of each symbol in bytes
        void initialize(uint32_t /*symbols*/, uint32_t /*symbol_size*/)
            {
                /// This is just the factory layer so we do nothing
            }     
    };
}

#endif

