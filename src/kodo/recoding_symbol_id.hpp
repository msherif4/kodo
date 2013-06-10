// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup symbol_id_layers
    /// @brief Randomly recombines existing coding coefficients to
    ///        allow a decoder to produce recoded packets.
    template<class SuperCoder>
    class recoding_symbol_id : public SuperCoder
    {
    public:

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

    public:

        /// @ingroup factory_layers
        /// The factory layer associated with this coder.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
            { }

            /// @copydoc layer::factory::max_id_size() const
            uint32_t max_id_size() const
            {
                return SuperCoder::factory::max_coefficients_size();
            }

        };

    public:

        /// @copydoc layer::construct(factory &)
        template<class Factory>
        void construct(Factory &the_factory)
        {
            SuperCoder::construct(the_factory);

            m_coefficients.resize(the_factory.max_coefficients_size());
            m_recode_id.resize(the_factory.max_coefficients_size());
        }

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        template<class Factory>
        void initialize(Factory &the_factory)
        {
            SuperCoder::initialize(the_factory);

            m_id_size = SuperCoder::coefficients_size();
            assert(m_id_size > 0);
        }

        /// Will write the recoded encoding vector (symbol id) into the
        /// symbol_id buffer. The coding coefficients used to produce the
        /// recoded encoding vector will be available in the
        /// symbol_coefficient buffer.
        ///
        /// @copydoc layer::write_id(uint8_t*, uint8_t**)
        uint32_t write_id(uint8_t *symbol_id, uint8_t **coefficients)
        {
            assert(symbol_id != 0);
            assert(coefficients != 0);

            // Zero the symbol id
            std::fill_n(m_recode_id.begin(), m_id_size, 0);

            // Prepare the symbol id storage
            sak::mutable_storage id_storage =
                sak::storage(symbol_id, m_id_size);

            // Check the number of symbols stored
            uint32_t symbol_count = SuperCoder::rank();

            if(symbol_count == 0)
            {
                // Nothing we can do - we just return the zero'ed
                // symbol coefficients and id
                *coefficients = symbol_id;
                sak::copy_storage(
                    id_storage, sak::storage(&m_recode_id[0], m_id_size));

                return m_id_size;
            }
            else if(symbol_count < SuperCoder::symbols())
            {
                SuperCoder::generate_partial(&m_coefficients[0]);
            }
            else
            {
                SuperCoder::generate(&m_coefficients[0]);
            }

            // Create the recoded symbol id
            value_type *recode_id
                = reinterpret_cast<value_type*>(&m_recode_id[0]);

            value_type *recode_coefficients
                = reinterpret_cast<value_type*>(&m_coefficients[0]);

            for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
            {
                value_type c =
                    fifi::get_value<field_type>(recode_coefficients, i);

                if(!c)
                {
                    continue;
                }

                assert(SuperCoder::symbol_pivot(i));

                const value_type *source_id =
                    SuperCoder::coefficients_value( i );

                if(fifi::is_binary<field_type>::value)
                {
                    SuperCoder::add(
                        recode_id, source_id,
                        SuperCoder::coefficients_length());
                }
                else
                {
                    SuperCoder::multiply_add(
                        recode_id, source_id, c,
                        SuperCoder::coefficients_length());
                }
            }


            *coefficients = &m_coefficients[0];
            sak::copy_storage(
                id_storage, sak::storage(&m_recode_id[0], m_id_size));

            return m_id_size;
        }


        /// @copydoc layer::id_size()
        uint32_t id_size() const
        {
            return m_id_size;
        }

    protected:

        /// The number of bytes needed to store the symbol id
        /// coding coefficients
        uint32_t m_id_size;

        /// The storage type - we use aligned storage for both buffers
        /// since if the coefficients are multibyte data types was have
        /// to ensure the de-referencing the pointers are safe.
        typedef std::vector<uint8_t, sak::aligned_allocator<uint8_t> >
        aligned_vector;

        /// Buffer for the recoding coefficients
        aligned_vector m_coefficients;

        /// Buffer for the recoded id
        aligned_vector m_recode_id;
    };

}


