
/// @brief Doxygen dummy class specifying the API available at different
///        layers
class layer
{
public:

    class factory
    {
    public:

        /// @ingroup factory_api
        /// @brief Factory constructor
        /// @param max_symbols the maximum symbols this coder can expect
        /// @param max_symbol_size the maximum size of a symbol in bytes
        factory(uint32_t max_symbols, uint32_t max_symbol_size);

        /// @ingroup factory_api
        /// @brief Builds the actual coder
        /// @param symbols the symbols this coder will use
        /// @param symbol_size the size of a symbol in bytes
        /// @return pointer to an instantiation of an encoder or decoder
        pointer build(uint32_t symbols, uint32_t symbol_size);

        /// @ingroup factory_api
        /// @return the maximum number of symbols in a block
        uint32_t max_symbols() const;

        /// @ingroup factory_api
        /// @return the maximum symbol size in bytes
        uint32_t max_symbol_size() const;

        /// @ingroup symbol_id_api
        /// @brief Can be reimplemented by a Factory Symbol ID layer
        /// @return The maximum size in bytes required for a symbol id.
        /// @see factory_layer_api
        uint32_t max_symbol_id_size();
    };

    /// @ingroup factory_api
    /// @brief Constructs the coder with the maximum parameters it will ever
    ///        see. A coder may only be constructed ONCE, but initialized many
    ///        times.
    /// @param max_symbols the maximum symbols this coder can expect
    /// @param max_symbol_size the maximum size of a symbol in bytes
    void construct(uint32_t max_symbols, uint32_t max_symbol_size);

    /// @ingroup factory_api
    /// @brief Initializes the coder
    /// @param symbols the number of symbols the coder should store
    /// @param symbol_size the size of each symbol in bytes
    void initialize(uint32_t symbols, uint32_t symbol_size);

    //
    // SYMBOL ID API
    //

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

    //
    // CODING_INFO API
    //

    /// @ingroup coding_info_api
    /// @return the number of symbols in this block coder
    uint32_t symbols() const;

    /// @ingroup coding_info_api
    /// @return the symbol size of a symbol in bytes
    uint32_t symbol_size() const;

    /// @ingroup coding_info_api
    /// @return the length of the symbol in value_type elements
    uint32_t symbol_length() const;

    /// @ingroup coding_info_api
    /// @concept block_size
    /// @return the block size i.e. the total size in bytes
    ///         that this coder operates on. Users may
    ///         use the bytes_used() function provided in the
    ///         symbol storage layers to see how many of those
    ///         bytes are then used.
    uint32_t block_size() const;

    /// @ingroup coding_info_api
    /// Sets the number of bytes used
    /// @param bytes_used number of bytes used of the total coders block size
    void set_bytes_used(uint32_t bytes_used);

    /// @ingroup coding_info_api
    /// @return the number of bytes used
    uint32_t bytes_used() const;

    /// @ingroup coding_info_api
    bool symbol_exists(uint32_t index);


};





