// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_REED_SOLOMON_SYMBOL_ID_HPP
#define KODO_RS_REED_SOLOMON_SYMBOL_ID_HPP

#include <cstdint>

namespace kodo
{

    /// @brief Base class for the Reed-Solomon symbol id reader and
    ///        writer. Contains the functionality to cache the
    ///        generator matrices.
    ///
    /// @ingroup symbol_id_layers
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

    public:

        /// The factory layer associated with this coder. Maintains
        /// the block generator needed for the encoding vectors.
        class factory : public SuperCoder::factory
        {
        public:

            /// @copydoc layer::factory::build(uint32_t, uint32_t)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder =
                        SuperCoder::factory::build(symbols, symbol_size);

                    if(m_cache.find(symbols) == m_cache.end())
                    {
                        m_cache[symbols] =
                            SuperCoder::factory::construct_matrix(symbols);
                    }

                    coder->m_matrix = m_cache[symbols];

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

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                assert(m_matrix);

                // The row size of the generator matrix should fit
                // the expected coefficient buffer size
                assert(SuperCoder::coefficients_size() ==
                       m_matrix->row_size());
            }


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

#endif

