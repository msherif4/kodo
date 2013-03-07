// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_DEBUG_LINEAR_BLOCK_DECODER_HPP
#define KODO_DEBUG_LINEAR_BLOCK_DECODER_HPP

#include <cstdint>

#include <fifi/fifi_utils.hpp>

namespace kodo
{
    // Debug layer which allows printing encoding vectors
    // of a RLNC decoder.
    template<class SuperCoder>
    class debug_linear_block_decoder : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

        /// Access to status about uncoded symbols in the linear block
        /// decoder.
        using SuperCoder::m_uncoded;

        /// Access to status about the coded symbols in the linear block
        /// decoder.
        using SuperCoder::m_coded;

    public:

        /// @copydoc layer::initialize(uint32_t,uint32_t)
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                m_symbol_data.resize(symbol_size);
                m_symbol_coefficients.resize(
                    SuperCoder::coefficients_size());
            }

        /// Prints the decoding matrix to the output stream
        /// @param out The output stream to print to
        void print_decoding_matrix(std::ostream &out)
            {
                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    if( m_uncoded[i] )
                    {
                        out << i << " U:\t";
                    }
                    else if( m_coded[i] )
                    {
                        out << i << " C:\t";
                    }
                    else
                    {
                        out << i << " ?:\t";
                    }

                    value_type *coefficients_i =
                        SuperCoder::coefficients_value(i);

                    for(uint32_t j = 0; j < SuperCoder::symbols(); ++j)
                    {

                        value_type value =
                            fifi::get_value<field_type>(coefficients_i, j );

                        out << (uint32_t)value << " ";
                    }

                    out << std::endl;
                }

                out << std::endl;
            }

        /// @copydoc layer::decode_symbol(uint8_t*,uint8_t*)
        void decode_symbol(uint8_t *symbol_data,
                           uint8_t *symbol_coefficients)
            {
                auto data_dest =
                    sak::storage(m_symbol_data);
                auto coefficients_dest =
                    sak::storage(m_symbol_coefficients);

                auto data_src =
                    sak::storage(symbol_data,
                                 SuperCoder::symbol_size());

                auto coefficients_src =
                    sak::storage(symbol_coefficients,
                                 SuperCoder::coefficients_size());

                sak::copy_storage(data_dest, data_src);
                sak::copy_storage(coefficients_dest, coefficients_src);

                m_symbol_coded = true;

                SuperCoder::decode_symbol(symbol_data, symbol_coefficients);
            }

        /// @copydoc layer::decode_symbol()
        void decode_symbol(const uint8_t *symbol_data,
                           uint32_t symbol_index)
            {
                auto data_dest =
                    sak::storage(m_symbol_data);

                auto data_src =
                    sak::storage(symbol_data, SuperCoder::symbol_size());

                sak::copy_storage(data_dest, data_src);

                m_symbol_coded =  false;
                m_symbol_index = symbol_index;

                // Has to be done before calling decode symbol since it
                // may manipulate the symbol data buffers
                SuperCoder::decode_symbol(symbol_data, symbol_index);
            }

        /// Prints the most recent symbol coefficients to enter the decoder
        /// @param out The output stream to print to
        void print_decoding_symbol(std::ostream &out)
            {
                if( m_symbol_coded )
                {
                    out << "Symbol C:" << std::endl<< "\t";

                    value_type *coefficients =
                        reinterpret_cast<value_type*>(
                            &m_symbol_coefficients[0]);

                    for(uint32_t j = 0; j < SuperCoder::symbols(); ++j)
                    {
                        value_type value =
                            fifi::get_value<field_type>(coefficients, j );

                        out << (uint32_t)value << " ";
                    }

                    out << std::endl;
                }
                else
                {
                    out << "Symbol U:" << std::endl;
                    out << "\tIndex: " << m_symbol_index << std::endl;
                }
            }

    private:

        /// Tracks whether the recent decoding symbol was coded
        bool m_symbol_coded;

        /// If the symbol what uncoded what was the symbol index
        uint32_t m_symbol_index;

        /// Stores the data of the decoding symbol
        std::vector<uint8_t> m_symbol_data;

        /// If coded stores the coefficients of the decoding symbol
        std::vector<uint8_t> m_symbol_coefficients;


    };

}

#endif

