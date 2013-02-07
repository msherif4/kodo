
/// @brief Empty implementation of the symbol id API. Can be useful
///        for testing purposes.
/// @ingroup symbol_id_layer_api
class null_symbol_id_api
{
public:

    /// @param symbol_id The buffer where the symbol id is written. This id should
    ///        uniquely describe the coding coefficients used to generate an encoded
    ///        symbol. It is therefore sufficient to transmit only the symbol id and
    ///        not necessarily all coding coefficients to decoders.
    /// @param symbol_id_coefficient Pointer to a pointer of symbol id coefficients.
    ///        After the call to write_id the symbol_id_coefficient pointer will point
    ///        to memory storing all coding coefficients correspoinding to the symbol id
    ///        written to the symbol_id buffer.
    /// @return The number of bytes used from the symbol_id buffer.
    uint32_t write_id(uint8_t *symbol_id, uint8_t **symbol_id_coefficients)
        {
            (void) symbol_id;
            (void) symbol_id_coefficients;

            return 0;
        }
};

