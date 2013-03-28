// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @example decode_simple.cpp
///
/// It may be that we want to implement a new decoding algorithm in Kodo or
/// simply just test an existing decoding implementation. In that case, it may
/// be preferred to reuse functionalities in already existing layers while
/// stripping unrequired functionalities for simplicity.
///
/// This example illustrates how the operations of a RLNC decoder can be
/// tested in a binary field without introducing unnecessary complexity.
/// To keep it simple, we want to enter the data to be decoded ourself instead
/// of relying on an encoder to generate the data.
/// The functionalities in layers above the "Codec API" is therefore not
/// required and has been stripped from the stack for simplicity. However,
/// the layers below the "Codec API" has been kept as they provide
/// functionalities that we require.

#include <vector>

#include <boost/utility/binary.hpp>

#include <kodo/rlnc/full_vector_codes.hpp>

namespace fifi
{
    /// Gets the number of field elements that can be stored in a certain number
    /// of bytes
    /// @param bytes number of bytes
    /// @return number of field elements that can be stored in bytes 
    template<class Field>
    inline uint32_t num_elements(uint32_t bytes)
    {
        return fifi::elements_needed<Field>(bytes);
    }

    /// Binary specialization to get the number of field elements that can be
    /// stored in a certain number of bytes
    /// @param bytes number of bytes
    /// @return number of field elements that can be stored in bytes 
    template<>
    inline uint32_t num_elements<fifi::binary>(uint32_t bytes)
    {
        assert(bytes > 0);

        return 8*bytes;
    }

    /// Set multiple elements in an array of fields
    /// @param elements elements to alter
    /// @param values vector of values to insert
    template <typename Field>
    inline void set_values(typename Field::value_ptr elements,
        std::vector<typename Field::value_type> values)
    {
        assert(elements != 0);

        uint32_t i = 0;
        for (auto value : values)
        {
            set_value<Field>(elements, i++, value);
        }
    }

    /// Initialize multiple elements in an array of fields
    /// @param elements elements to initialize
    /// @param values vector of vectors to insert
    template<typename Field>
    inline
    void initialize_values(typename Field::value_ptr elements,
        std::vector<std::vector<typename Field::value_type>> values)
    {
        assert(elements != 0);
        assert(values.size() > 0);
        // Maybe assert that all vectors are same size
        // (for now, we assume they are)

        uint32_t symbols = values.size();
        uint32_t symbol_size = bytes_needed<Field>(values[0].size());

        // Initialize data to zero
        std::fill_n(elements, symbols*symbol_size, 0);

        uint8_t* ptr = elements;
        for (auto symbol : values)
        {
            set_values<Field>((typename Field::value_type*)ptr, symbol);
            ptr += symbol_size;
        }
    }

    /// Print elements in an array of fields
    /// @param elements elements to alter
    /// @param symbols number of symbols to print
    /// @param symbols_length length of a symbol (# elements in a symbol)
    template <typename Field>
    inline void print(typename Field::value_ptr elements,
        uint32_t symbols, uint32_t symbol_length)
    {
        uint32_t symbol_size = bytes_needed<Field>(symbol_length);
        uint8_t* ptr_end = ((uint8_t*)elements)+symbols*symbol_size;

        // Loop through all symbols
        for (uint8_t* ptr = elements; ptr < ptr_end; ptr += symbol_size)
        {
            // Loop through all elements in a symbol
            for (uint32_t i = 0; i < symbol_length; ++i)
            {
                // Print element
                std::cout << (unsigned)fifi::get_value<Field>(
                         (typename Field::value_type*)ptr, i);
            }

            // Print new line after each symbol
            std::cout << std::endl;
        }
    }
}

namespace kodo
{
    // Simple RLNC decoder
    template<class Field>
    class rlnc_decoder
        : public // Codec API
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Storage API
                 symbol_storage_tracker<
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
                     > > > > > > > > > >
    {};
}


int main()
{
    typedef fifi::binary field_type;
    typedef field_type::value_type value_type;

    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    const uint32_t symbols = 3;
    const uint32_t symbol_size = 1;  // 8 bits in each symbol
    const uint32_t symbol_length = 5;

    // Typdefs for the decoder type we wish to use
    typedef kodo::rlnc_decoder<field_type> rlnc_decoder;

    // In the following we will make an decoder factory.
    // The factory is used to build actual decoders
    rlnc_decoder::factory decoder_factory(symbols, symbol_size);
    rlnc_decoder::pointer decoder = decoder_factory.build(symbols, symbol_size);
    

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
    //                          encoded_symbols using the symbol_coefficients
    //                          below.
    // encoded_symbols (X):     Symbols that has been encoded using
    //                          original_symbols using the symbol_coefficients
    //                          below.
    // symbol_coefficients (G): Coefficients used to encode/decode between
    //                          original_symbols and encoded_symbols.
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
    //        | 0 0 0 1 1 1 0 0 |   | 0 1 0 | | 0 0 0 0 1 1 0 1 |
    //        | 0 0 0 1 0 0 0 1 | = | 1 1 0 | | 0 0 0 1 1 1 0 0 |
    //        | 0 0 0 0 1 0 1 1 |   | 1 0 1 | | 0 0 0 0 0 1 1 0 |
    //
    // From the above matrix, the first encoded symbol is just the second
    // original symbol M_2. The second encoded symbol is M_1 bitwise xor M_2,
    // and the third encoded symbol is M_1 bitwise xor M_3.

    uint8_t encoded_symbols[symbols*symbol_size];
    uint8_t symbol_coefficients[symbols*symbol_size];
    uint8_t original_symbols[symbols*symbol_size];


    // Initialize data and add trailing zeros in each symbol automatically to
    // fit symbol_size
    fifi::initialize_values<field_type>((value_type*)encoded_symbols,
            { {1,1,1,0,0},
              {1,0,0,0,1},
              {0,1,0,1,1} } );

    fifi::initialize_values<field_type>((value_type*)symbol_coefficients,
            { {0,1,0},
              {1,1,0},
              {1,0,1} } );

    fifi::initialize_values<field_type>((value_type*)original_symbols,
            { {0,1,1,0,1},
              {1,1,1,0,0},
              {0,0,1,1,0} } );


    std::cout << std::endl << "Original symbols:" << std::endl;
    fifi::print<field_type>((value_type*)original_symbols,
            symbols, symbol_length);

    std::cout << std::endl << "Symbol Coefficients (before decoding):"
        << std::endl;
    fifi::print<field_type>((value_type*)symbol_coefficients,
            symbols, symbols);


    // Decode each symbol
    for (uint32_t i = 0; i < symbols; ++i)
    {
        decoder->decode_symbol(&encoded_symbols[i], &symbol_coefficients[i]);
    }


    // Copy decoded data into a vector
    std::vector<uint8_t> decoded_symbols(decoder->block_size());
    decoder->copy_symbols(sak::storage(decoded_symbols));


    std::cout << std::endl << "Symbol Coefficients (after decoding):"
        << std::endl;
    fifi::print<field_type>((value_type*)symbol_coefficients,
            symbols, symbols);

    std::cout << std::endl << "Encoded symbols:" << std::endl;
    fifi::print<field_type>((value_type*)encoded_symbols,
            symbols, symbol_length);

    std::cout << std::endl << "Decoded data:" << std::endl;
    fifi::print<field_type>((value_type*)&decoded_symbols[0],
            symbols, symbol_length);
    std::cout << std::endl;


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
