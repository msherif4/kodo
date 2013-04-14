// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "systematic_vandermonde_matrix_base.hpp"
#include "vandermonde_matrix_base.hpp"
#include "transpose_vandermonde_matrix.hpp"

namespace kodo
{

    /// @copydoc systematic_vandermonde_matrix_base
    template<class SuperCoder>
    class systematic_vandermonde_matrix
        : public transpose_vandermonde_matrix<
                 systematic_vandermonde_matrix_base<
                 vandermonde_matrix_base<SuperCoder> > >
    { };

}


