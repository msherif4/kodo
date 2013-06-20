// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cmath>

#include <sak/convert_endian.hpp>

namespace kodo
{

    /// @todo: fix docs
    template<class SuperCoder>
    class sparse_staircase_encoder : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

    public:

        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
            { }

            /// @copydoc layer::factory::max_feedback_size() const
            uint32_t max_feedback_size() const
            {
                return 4U;
            }

        };

    public:

        template<class Factory>
        void initialize(Factory& factory)
        {
            SuperCoder::initialize(factory);
            SuperCoder::set_density(calculate_initial_density());
        }

        void handle_feedback()
        {
            if(fifi::is_binary<field_type>::value)
            {
                SuperCoder::set_density(0.5);
            }
            else
            {
                SuperCoder::set_density(0.03);
            }
        }

        /// @copydoc layer::feedback_size() const
        uint32_t feedback_size() const
        {
            return 4U;
        }

    protected:

        double calculate_initial_density()
        {
            uint32_t symbols = SuperCoder::symbols();
            assert(symbols > 1);

            uint32_t step = (uint32_t)std::ceil(std::log10(symbols));

            switch(step)
            {
            case 1U: { return 2.0 / symbols; }
            case 2U: { return 3.0 / symbols; }
            default:  { return 10.0 / symbols; }
            };

        }

    };

}


