// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RS_VANDERMONDE_MATRIX_HPP
#define KODO_RS_VANDERMONDE_MATRIX_HPP

#include "vandermonde_matrix_base.hpp"
#include "transpose_vandermonde_matrix.hpp"

namespace kodo
{

    /// @copydoc vandermonde_matrix_base
    template<class SuperCoder>
    class vandermonde_matrix
        : public transpose_vandermonde_matrix<
                 vandermonde_matrix_base<SuperCoder> >
    { };

}

#endif

