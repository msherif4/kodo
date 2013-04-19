// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <cassert>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/bernoulli_distribution.hpp>

#include <fifi/is_binary.hpp>
#include <fifi/fifi_utils.hpp>

namespace kodo
{
    /// @ingroup coefficient_generator_layers
    /// @brief Generate uniformly distributed coefficients with a specific
    /// density
    template<class SuperCoder>
    class sparse_uniform_generator : public SuperCoder
    {
    public:

        /// @copydoc layer::value_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// The random generator used
        typedef boost::random::mt19937 generator_type;

        /// @copydoc layer::seed_type
        typedef generator_type::result_type seed_type;

    public:

        /// Constructor
        sparse_uniform_generator()
            : m_value_distribution(1, field_type::max_value)
        { }

        /// @copydoc layer::generate(uint8_t*)
        void generate(uint8_t *coefficients)
        {
            assert(coefficients != 0);

            for (uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {
                if (m_bernoulli(m_random_generator))
                {
                    if (fifi::is_binary<field_type>::value)
                    {
                        fifi::set_value<field_type>(coefficients, i, 1);
                    }
                    else
                    {
                        value_type coefficient =
                            m_value_distribution(m_random_generator);

                        fifi::set_value<field_type>(
                            coefficients, i, coefficient);
                    }
                }
            }
        }

        /// @copydoc layer::generate(uint8_t*)
        void generate_partial(uint8_t *coefficients)
        {
            assert(coefficients != 0);

            // Since we will not set all coefficients we should ensure
            // that the non specified ones are zero
            std::fill_n(
                coefficients, SuperCoder::coefficients_size(), 0);

            uint32_t symbols = SuperCoder::rank();

            for (uint32_t i = 0; i < symbols; ++i)
            {
                if (!SuperCoder::symbol_pivot(i))
                {
                    continue;
                }

                if (m_bernoulli(m_random_generator))
                {
                    if (fifi::is_binary<field_type>::value)
                    {
                        fifi::set_value<field_type>(coefficients, i, 1);
                    }
                    else
                    {
                        value_type coefficient =
                            m_value_distribution(m_random_generator);

                        fifi::set_value<field_type>(coefficients, i, coefficient);
                    }
                }

            }
        }

        /// @copydoc layer::seed(seed_type)
        void seed(seed_type seed_value)
        {
            m_random_generator.seed(seed_value);
        }

        /// Set the density of the coefficients generated
        /// @param density coefficients density
        void set_density(double density)
        {
            assert(density > 0);

            m_bernoulli = boost::random::bernoulli_distribution<>(density);
        }

        /// Get the density of the coefficients generated
        /// @return the density of the generator
        double get_density() const
        {
            return m_bernoulli.p();
        }

    private:

        /// The distribution controlling the density of the coefficients
        boost::random::bernoulli_distribution<> m_bernoulli;

        /// The type of the value_type distribution
        typedef boost::random::uniform_int_distribution<value_type>
            value_type_distribution;

        /// Distribution that generates random values from a finite field
        value_type_distribution m_value_distribution;

        /// The random generator
        boost::random::mt19937 m_random_generator;

    };
}

