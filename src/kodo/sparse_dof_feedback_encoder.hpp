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
    class sparse_dof_feedback_encoder : public SuperCoder
    {
    public:

        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size),
                  m_budget(2)
            { }

            /// @copydoc layer::factory::max_feedback_size() const
            uint32_t max_feedback_size() const
            {
                return 4U;
            }

            /// @todo
            void set_overhead_budget(uint32_t budget)
            {
                assert(budget > 0);
                m_budget = budget;
            }

            /// @todo
            uint32_t overhead_budget() const
            {
                return m_budget;
            }

        protected:

            uint32_t m_budget;

        };

    public:

        template<class Factory>
        void initialize(Factory& factory)
        {
            SuperCoder::set_density(calculate_initial_density());
            m_budget = factory.overhead_budget();
        }

        /// @copydoc layer::feedback_size() const
        uint32_t feedback_size() const
        {
            return 4U;
        }

        void read_feedback(uint8_t* feedback)
        {
            uint32_t dof = sak::big_endian::get<uint32_t>(feedback);

            double density = calculate_density(dof);
            SuperCoder::set_density(density);

        }

    protected:

        double calculate_density(uint32_t dof)
        {

        }

        double calculate_initial_density()
        {
            uint32_t symbols = SuperCoder::symbols();

            double step = std::ceil(std::log10(symbols));

            switch(step)
            {
            case 1.0: { return 0.3; }
            case 2.0: { return 0.2; }
            default:  { return 10.0 / symbols; }
            };

        }

    protected:

        uint32_t m_budget;

    };

}


