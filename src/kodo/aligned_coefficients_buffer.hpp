// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_ALIGNED_COEFFICIENTS_BUFFER_HPP
#define KODO_ALIGNED_COEFFICIENTS_BUFFER_HPP

#include <cstdint>

#include <boost/shared_ptr.hpp>

#include <sak/aligned_allocator.hpp>
#include <sak/is_aligned.hpp>

namespace kodo
{
    /// @brief Helper layer for layers that require a buffer for storing
    ///        symbol coefficients. The storage is uninitialized. The
    ///        buffer is guaranteed to be aligned (on a 16 byte boundary).
    template<class SuperCoder>
    class aligned_coefficients_buffer : public SuperCoder
    {
    public:

        /// @copydoc layer::pointer
        typedef typename SuperCoder::pointer pointer;

        /// Pointer to the type of this layer
        typedef boost::shared_ptr<
            aligned_coefficients_buffer<SuperCoder> > this_pointer;

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

            /// @copydoc layer::factory::build(uint32_t,uint32_t)
            pointer build(uint32_t symbols, uint32_t symbol_size)
                {
                    pointer coder =
                        SuperCoder::factory::build(symbols, symbol_size);

                    // Make sure we call the correct initialize_storage
                    // function, if there are several such function in
                    // the stack. First getting a pointer directly to this
                    // layer removes ambiguity.
                    this_pointer this_coder = coder;

                    this_coder->initialize_storage(
                        SuperCoder::factory::max_coefficients_size());

                    return coder;
                }
        };

    private:

        /// Initialize the coefficient storage
        /// @param max_coefficients_size The maximum size of the coding
        ///        coefficients in bytes
        void initialize_storage(uint32_t max_coefficients_size)
            {
                assert(max_coefficients_size > 0);

                // Note, that resize will not re-allocate anything
                // as long as the sizes are not larger than
                // previously. So this call should only have an
                // effect the first time this function is called.
                m_coefficients.resize(max_coefficients_size);
            }

    protected:

        /// The storage type
        typedef std::vector<uint8_t, sak::aligned_allocator<uint8_t> >
            aligned_vector;

        /// Temp symbol id (with aligned memory)
        aligned_vector m_coefficients;

    };
}

#endif

