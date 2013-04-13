// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <map>

namespace kodo
{

    /// @ingroup symbol_id_layers
    ///
    /// @brief Base class for the Reed-Solomon symbol id reader and
    ///        writer. Contains the functionality to cache the
    ///        generator matrices.
    template<class SuperCoder>
    class reed_solomon_symbol_id : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename SuperCoder::value_type value_type;

        /// The generator matrix type
        typedef typename SuperCoder::generator_matrix generator_matrix;

        /// Pointer to coder produced by the factories
        typedef typename SuperCoder::pointer pointer;

        /// Pointer the type of this layer
        typedef boost::shared_ptr<
            reed_solomon_symbol_id<SuperCoder> > this_pointer;

    public:

        /// The factory layer associated with this coder. Maintains
        /// the block generator needed for the encoding vectors.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::factory(uint32_t,uint32_t)
            factory(uint32_t max_symbols, uint32_t max_symbol_size)
                : SuperCoder::factory(max_symbols, max_symbol_size)
            { }


            /// @copydoc layer::factory::build()
            pointer build()
            {
                pointer coder = SuperCoder::factory::build();

                uint32_t symbols = SuperCoder::factory::symbols();

                if(m_cache.find(symbols) == m_cache.end())
                {
                    m_cache[symbols] =
                        SuperCoder::factory::construct_matrix(symbols);
                }

                this_pointer this_coder = coder;
                this_coder->m_matrix = m_cache[symbols];

                // The row size of the generator matrix should fit
                // the expected coefficient buffer size
                assert(coder->coefficients_size() ==
                       this_coder->m_matrix->row_size());

                return coder;
            }

            /// @copydoc layer::max_id_size() const
            uint32_t max_id_size() const
            {
                return sizeof(value_type);
            }

        private:

            /// map for blocks
            std::map<uint32_t, boost::shared_ptr<generator_matrix> > m_cache;

        };


    public:

        /// @copydoc layer::id_size() const
        uint32_t id_size() const
        {
            return sizeof(value_type);
        }

    protected:

        /// The generator matrix
        boost::shared_ptr<generator_matrix> m_matrix;

    };

}


