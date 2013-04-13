// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <boost/shared_ptr.hpp>

/// @file
/// Sometimes it is useful to be able to write generic code
/// which may work using both encoders which have systematic
/// capabilities and encoders which do not. Using the functions
/// presented here this is possible.
///
/// @ingroup g_systematic_coding
/// @ingroup g_generic_api
///
/// @section Tests
///
/// see @ref test_systematic_operations.cpp

namespace kodo
{
    /// Catch all implementation if no specializations exist
    /// this function will be called.
    /// @return always return false
    inline bool is_systematic_encoder_dispatch(...)
    {
        return false;
    }

    /// Returns whether an encoder has systematic capabilities
    /// this is sometimes needed when writing generic code that
    /// works with many different encoder types. Different encoders
    /// should overload / specialize this function to return true
    /// if they support the systematic operations.
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    /// @return true if the encoder supports the systematic operations
    ///         otherwise false
    template<class Encoder>
    inline bool is_systematic_encoder(const Encoder & e)
    {
        return is_systematic_encoder_dispatch(&e);
    }

    /// Helper for unwrapping encoders stored in a shared_ptr
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    /// @return true if the encoder supports the systematic operations
    ///         otherwise false
    template<class Encoder>
    inline bool is_systematic_encoder(const boost::shared_ptr<Encoder> &e)
    {
        assert(e);
        return is_systematic_encoder(*e);
    }

    /// Catch all implementation if no specializations exist
    /// this function will be called.
    /// @return always return false
    inline bool is_systematic_on_dispatch(...)
    {
        // Catch all impl. should not be called, check using
        // is_systematic_encoder(...) whether an encoder supports
        // the systematic API
        assert(0);
        return false;
    }

    /// Returns whether the encoder is in the systematic mode i.e. it is
    /// generating un-coded symbols
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    /// @return true if the encoder is in the systematic state
    template<class Encoder>
    inline bool is_systematic_on(Encoder &e)
    {
        return is_systematic_on_dispatch(&e);
    }

    /// Helper for unwrapping encoders stored in a shared_ptr
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    /// @return true if the encoder is in the systematic state
    template<class Encoder>
    inline bool is_systematic_on(boost::shared_ptr<Encoder> &e)
    {
        assert(e);
        return is_systematic_on(*e);
    }

    /// Catch all implementation if no specializations exist
    /// this function will be called.
    /// @return always return false
    inline void set_systematic_off_dispatch(...)
    {
        // Catch all impl. should not be called, check using
        // is_systematic_encoder whether an encoder supports the systematic
        // API
        assert(0);
    }

    /// Switches the systematic encoding off - i.e. stop generating
    /// un-coded symbols.
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    template<class Encoder>
    inline void set_systematic_off(Encoder &e)
    {
        set_systematic_off_dispatch(&e);
    }

    /// Helper for unwrapping encoders stored in a shared_ptr
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    template<class Encoder>
    inline void set_systematic_off(boost::shared_ptr<Encoder> &e)
    {
        assert(e);
        set_systematic_off(*e);
    }

    /// Catch all implementation if no specializations exist
    /// this function will be called.
    /// @return always return false
    inline void set_systematic_on_dispatch(...)
    {
        // Catch all impl. should not be called, check using
        // is_systematic_encoder whether an encoder supports the systematic
        // API
        assert(0);
    }

    /// Switches the systematic encoding on - i.e. start generating
    /// un-coded symbols.
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    template<class Encoder>
    inline void set_systematic_on(Encoder &e)
    {
        set_systematic_on_dispatch(&e);
    }

    /// Helper for unwrapping encoders stored in a shared_ptr
    ///
    /// \ingroup g_systematic_coding
    /// \ingroup g_generic_api
    ///
    /// @param e the encoder
    template<class Encoder>
    inline void set_systematic_on(boost::shared_ptr<Encoder> &e)
    {
        assert(e);
        set_systematic_on(*e);
    }
}



