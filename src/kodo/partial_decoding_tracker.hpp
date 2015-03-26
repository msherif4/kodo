// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include <sak/storage.hpp>

#include <fifi/is_binary.hpp>
#include <fifi/fifi_utils.hpp>

namespace kodo
{

    /// @ingroup payload_codec
    /// @brief This layer can be added to a decoding stack to track
    ///        when partial decoding is possible i.e. when symbols in
    ///        decoder are fully decoded even though the full data
    ///        block has not been sent.
    ///
    /// Detecting whether data has been partially decoded can be done by
    /// tracking whether encoder and decoder has the same rank. This of course
    /// means that we depend on the decoder performs full Gaussian elimination
    /// on the incoming symbols.
    ///
    /// To figure out which symbols have been partially decoded the decoder's
    /// layer::symbol_pivot(uint32_t) const function can be used.
    template<class SuperCoder>
    class partial_decoding_tracker : public SuperCoder
    {
    public:

        /// @copydoc layer::rank_type
        typedef typename SuperCoder::rank_type rank_type;

    public:

        /// @return True if the decoding matrix should be partially decoded.
        bool is_partial_complete() const
        {
            rank_type decoder_rank = SuperCoder::rank();
            rank_type encoder_rank = SuperCoder::encoder_rank();

            assert(decoder_rank <= encoder_rank);

            if(decoder_rank == 0)
            {
                return false;
            }
            else
            {
                return SuperCoder::rank() == SuperCoder::encoder_rank();
            }
        }

    };

}

