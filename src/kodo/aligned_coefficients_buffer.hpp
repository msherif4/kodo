// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

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

        /// The factory type
        typedef typename SuperCoder::factory factory;

    public:

        /// @copydoc layer::construct(factory &)
        void construct(factory &the_factory)
        {
            SuperCoder::construct(the_factory);
            m_coefficients.resize(the_factory.max_coefficients_size());
        }

    protected:

        /// The storage type
        typedef std::vector<uint8_t, sak::aligned_allocator<uint8_t> >
            aligned_vector;

        /// Temp symbol id (with aligned memory)
        aligned_vector m_coefficients;

    };
}


