// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @example decode_simple.cpp
///
/// It may be that we want to implement a new decoding algorithm in Kodo or
/// simply just test an existing decoding implementation. In that case, it may
/// be preferred to reuse functionalities in already existing layers while
/// stripping functionalities that are not required to keep it simple.
///
/// This example illustrates how the operations of a RLNC decoder can be
/// tested in a binary field without introducing unnecessary complexity.
/// To keep it simple, we want to enter the data to be decoded ourself instead
/// of relying on an encoder to generate the data.
/// The functionalities in layers above the "Codec API" is therefore not
/// required and has been stripped from the stack. However, the layers below
/// the "Codec API" has been kept as they provide functionalities that are
/// required.

#include <cstdint>
#include <cassert>
#include <iostream>
#include <vector>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/debug_linear_block_decoder.hpp>
#include <kodo/debug_coefficient_storage.hpp>
#include <kodo/debug_symbol_storage.hpp>

#include <fifi/fifi_utils.hpp>

namespace fifi
{
    /// Print field elements as a matrix
    /// @param elements elements to print
    /// @param symbols number of symbols to print
    /// @param symbol_elements number of field elements in a symbol
    template <typename Field>
    inline void print_matrix(typename Field::value_ptr elements,
        uint32_t symbols, uint32_t symbol_elements)
    {
        assert(elements != 0);

        uint32_t symbol_size =
            fifi::elements_to_size<Field>(symbol_elements);

        // Loop through all symbols
        for (uint32_t i = 0; i < symbols; ++i)
        {
            // Loop through all elements in a symbol
            for (uint32_t j = 0; j < symbol_elements; ++j)
            {
                // Print element
                std::cout << (uint32_t)fifi::get_value<Field>(
                        &elements[i*symbol_size], j) << " ";
            }

            // Print new line after each symbol
            std::cout << std::endl;
        }
    }

    /// Print field elements in a symbol or coefficients vector
    /// @param elements elements to print
    /// @param symbols number of symbols to print
    /// @param symbol_elements number of field elements in a symbol
    template <typename Field>
    inline void print_symbol(typename Field::value_type* elements,
        uint32_t symbol_elements)
    {
        assert(elements != 0);

        std::cout << "| ";
        for (uint32_t j = 0; j < symbol_elements; ++j)
        {
            // Print element
            std::cout << (uint32_t)fifi::get_value<Field>(
                    &elements[0], j) << " ";
        }
        std::cout << "|";
    }

    /// Print matrix with encoded symbols, coefficients matrix, and matrix
    /// with original symbols as an equation
    /// @param original_data original data field elements to print
    /// @param coefficients_data coefficients field elements to print
    /// @param encoded_data encoded data field elements to print
    /// @param symbols number of symbols to print
    /// @param symbol_elements number of field elements in a symbol
    template <typename Field>
    inline void print_equation(typename Field::value_type* original_data,
        typename Field::value_type* coefficients_data,
        typename Field::value_type* encoded_data,
        uint32_t symbols, uint32_t symbol_elements)
    {
        assert(original_data != 0);
        assert(coefficients_data != 0);
        assert(encoded_data != 0);

        uint32_t symbol_size =
            fifi::elements_to_size<Field>(symbol_elements);

        uint32_t coef_size =
            fifi::elements_to_size<Field>(symbols);

        uint32_t middle_symbol = symbols/2;

        // Loop through all symbols
        for (uint32_t i = 0; i < symbols; ++i)
        {
            print_symbol<Field>(&original_data[i*symbol_size], symbol_elements);

            (i == middle_symbol) ? std::cout << " = " : std::cout << "   ";

            print_symbol<Field>(&coefficients_data[i*coef_size], symbols);

            std::cout << " ";

            print_symbol<Field>(&encoded_data[i*symbol_size], symbol_elements);

            // Print new line after each symbol
            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

}

namespace kodo
{
    // Simple RLNC decoder
    template<class Field>
    class rlnc_decoder
        : public // Codec API
                 debug_linear_block_decoder<
                 linear_block_decoder<
                 // Coefficient Storage API
                 debug_coefficient_storage<
                 coefficient_storage<
                 coefficient_info<
                 // Storage API
                 debug_symbol_storage<
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 rlnc_decoder<Field>
                     > > > > > > > > > > > >
    {};
}


int main()
{
    typedef fifi::binary field_type;
    typedef field_type::value_type value_type;

    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the number of elements in a symbol
    const uint32_t symbols = 3;
    const uint32_t symbol_elements = 5;

    // Get the size, in bytes, of a coefficient vector and a symbol
    const uint32_t coefficients_size = fifi::elements_to_size<field_type>(symbols);
    const uint32_t symbol_size = fifi::elements_to_size<field_type>(symbol_elements);

    // Typdefs for the decoder type we wish to use
    typedef kodo::rlnc_decoder<field_type> rlnc_decoder;

    // In the following we will make an decoder factory.
    // The factory is used to build actual decoders
    rlnc_decoder::factory decoder_factory(symbols, symbol_size);
    rlnc_decoder::pointer decoder = decoder_factory.build();
    

    // To illustrate decoding, random data has been filled into the
    // matrices below. It is crucial that the equation below is correct
    // if the purpose is to test if the decoder decodes correctly as this
    // example evaluates in the end of the example.
    //
    // For additional information, please see the article
    //
    //   Christina Fragouli, Jean-Yves Le Boudec, and Jörg Widmer.
    //   Network Coding: An Instant Primer.
    //   SIGCOMM Comput. Commun. Rev., 36(1):63-68, January 2006.
    //
    // from which the notation in the example is based on.
    //
    //
    //
    // original_symbols (M):    Symbols we expect to obtain from decoding
    //                          encoded_symbols using the symbol_coefficients.
    // symbol_coefficients (G): Coefficients used to encode/decode between
    //                          original_symbols and encoded_symbols.
    // encoded_symbols (X):     Symbols that has been encoded from
    //                          original_symbols using the symbol_coefficients.
    //
    //
    //                          X = G M
    //
    //                        X^j = sum_{i=1}^{n} g_i^j M^i
    //
    //                |   X^1   |   | g^1_1 g^1_2 g^1_3 | |   M^1   |
    //                |   X^2   | = | g^2_1 g^2_2 g^2_3 | |   M^2   |
    //                |   X^3   |   | g^3_1 g^3_2 g^3_3 | |   M^3   |
    //
    //       | encoded symbol 1 |   | encoding vect 1 | | original symbol 1 |
    //       | encoded symbol 2 | = | encoding vect 2 | | original symbol 2 |
    //       | encoded symbol 3 |   | encoding vect 3 | | original symbol 3 |
    //
    //              | 1 1 1 0 0 |   | 0 1 0 | | 0 1 1 0 1 |
    //              | 1 0 0 0 1 | = | 1 1 0 | | 1 1 1 0 0 |
    //              | 0 1 0 1 1 |   | 1 0 1 | | 0 0 1 1 0 |
    //
    // From the above matrix, the first encoded symbol is just the second
    // original symbol M_2. The second encoded symbol is M_1 bitwise xor M_2,
    // and the third encoded symbol is M_1 bitwise xor M_3.


    // The computer reads the bits in the opposite direction of how the
    // elements are written matematically in the matrices above.
    // Therefore, it may be easier to find the hex values, which we input into
    // the variables below, if the matrices above are rewritten with the bits
    // in the direction which they are stored in memory. This is shown below:
    //
    //              | 0 0 1 1 1 |   | 0 1 0 | | 1 0 1 1 0 |
    //              | 1 0 0 0 1 | = | 0 1 1 | | 0 0 1 1 1 |
    //              | 1 1 0 1 0 |   | 1 0 1 | | 0 1 1 0 0 |

    uint8_t original_symbols[] = { 0x16, 0x07, 0x0c };
    uint8_t symbol_coefficients[] = { 0x02, 0x03, 0x05 };
    uint8_t encoded_symbols[] = { 0x07, 0x11, 0x1a };

    std::cout << std::endl << "Original symbols:" << std::endl;
    fifi::print_matrix<field_type>((value_type*)original_symbols,
            symbols, symbol_elements);

    std::cout << std::endl << "Symbol coefficients:"
        << std::endl;
    fifi::print_matrix<field_type>((value_type*)symbol_coefficients,
            symbols, symbols);

    std::cout << std::endl << "Encoded symbols:" << std::endl;
    fifi::print_matrix<field_type>((value_type*)encoded_symbols,
            symbols, symbol_elements);

    std::cout << std::endl << "Equation:" << std::endl;
    fifi::print_equation<field_type>((value_type*)original_symbols,
        (value_type*)symbol_coefficients, (value_type*)encoded_symbols,
        symbols, symbol_elements);

    std::cout << "Start decoding..." << std::endl << std::endl;

    // Decode each symbol and print the decoding progress
    for (uint32_t i = 0; i < symbols; ++i)
    {
        // Pass the i'th symbol and coefficients to decoder
        decoder->decode_symbol(&encoded_symbols[i*symbol_size],
                &symbol_coefficients[i*coefficients_size]);

        decoder->print_latest_symbol_and_coefficients(std::cout, symbol_elements);

        std::cout << std::endl << "Decoding matrix:" << std::endl;
        decoder->print_decoding_matrix(std::cout);

        std::cout << "Symbol matrix:" << std::endl;
        decoder->print_symbol_matrix(std::cout, symbol_elements);
        std::cout << std::endl;
    }

    // Ensure that decoding was completed successfully.
    if (decoder->is_complete())
    {
        std::cout << "Decoding completed" << std::endl;
    }
    else
    {
        std::cout << 
            "Decoding incomplete - not all encoding vectors are independent" <<
            std::endl;
    }

    // Copy decoded data into a vector
    std::vector<uint8_t> decoded_symbols(decoder->block_size());
    decoder->copy_symbols(sak::storage(decoded_symbols));

    // Check that the original data is the same as the decoded data
    if (std::equal(decoded_symbols.begin(), decoded_symbols.end(),
        original_symbols))
    {
        std::cout << "Data decoded correctly";
    }
    else
    {
        std::cout << "Error: Decoded data differs from original data";
    }
    std::cout << std::endl << std::endl;

}
