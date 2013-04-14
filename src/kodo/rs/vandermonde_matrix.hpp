// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "vandermonde_matrix_base.hpp"
#include "transpose_vandermonde_matrix.hpp"

namespace kodo
{

    /// Vandermonde matrix as described in RFC 5510. Only
    /// difference is that we transpose it, if used as
    /// suggested the k coefficients needed when producing
    /// a new encoded symbol would be located disjoint in
    /// memory. Memory access will be very inefficient if
    /// coefficients are not in major row order.
    /// @copydoc vandermonde_matrix_base
    template<class SuperCoder>
    class vandermonde_matrix
        : public transpose_vandermonde_matrix<
                 vandermonde_matrix_base<SuperCoder> >
    { };

}


