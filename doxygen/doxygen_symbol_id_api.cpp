
/// @brief Doxygen dummy class specifying the API available at different
///        layers
class layer
{
public:

    class factory
    {
    public:

        /// @ingroup symbol_id_api
        /// @brief Can be reimplemented by a Factory Symbol ID layer
        /// @return The maximum size in bytes required for a symbol id.
        /// @see factory_layer_api
        uint32_t max_symbol_id_size();
    };

    /// @ingroup symbol_id_api
    /// @brief Can be reimplemented by a Symbol ID layer
    /// @param symbol_id The buffer where the symbol id is written. This id
    ///                  should uniquely describe the coding coefficients
    ///                  used to generate an encoded symbol. It is therefore
    ///                  sufficient to transmit only the symbol id and not
    ///                  necessarily all coding coefficients to decoders.
    /// @param symbol_id_coefficient Pointer to a pointer of symbol id
    ///                  coefficients. After the call to write_id the
    ///                  symbol_id_coefficient pointer will point to memory
    ///                  storing all coding coefficients corresponding to
    ///                  the symbol id written to the symbol_id buffer.
    /// @return The number of bytes used from the symbol_id buffer.
    uint32_t write_id(uint8_t *symbol_id, uint8_t **symbol_id_coefficients);

    /// @ingroup symbol_id_api
    /// @brief Can be reimplemented by a Symbol ID layer
    /// @param symbol_id Contains the symbol id for an encoded symbol.
    /// @param symbol_id_coefficient Pointer to pointer of symbol id
    ///        coefficients. After the call to read_id the
    ///        symbol_id_coefficient pointer will point to memory storing
    ///        all coding coefficients corresponding to the symbol id
    ///        read from the symbol_id buffer.
    void read_id(uint8_t *symbol_id, uint8_t **symbol_id_coefficients);

    /// @ingroup symbol_id_api
    /// @brief Can be reimplemented by a Symbol ID layer
    /// @return the size in bytes required for the symbol id buffer.
    uint32_t symbol_id_size();

};





