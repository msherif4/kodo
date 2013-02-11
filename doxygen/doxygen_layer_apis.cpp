
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
    // SYMBOL HEADER API
    //

    /// @ingroup symbol_header_api
    /// Writes the symbol header.
    /// @param symbol_data The destination buffer for the encoded symbol.
    /// @param symbol_header At this point the symbol header should be
    ///        initialized.
    /// @return The number of bytes used from symbol_header buffer.
    uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_header);

    /// @ingroup symbol_header_api
    /// Reads the symbol header.
    /// @param symbol_data The destination buffer for the encoded symbol.
    /// @param symbol_header At this point the symbol header should be
    ///        initialized.
    /// @return The number of bytes used from symbol_header buffer.
    uint32_t decode(uint8_t *symbol_data, uint8_t *symbol_header)

    /// @ingroup symbol_header_api
    /// @brief Can be reimplemented by a symbol header API layer to ensure that
    ///        enough space is available in the header for some layer specific
    ///        data.
    /// @return The size in bytes required for the symbol header buffer.
    uint32_t symbol_header_size();


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
    // CODEC INFO API
    //

    /// @ingroup codec_info_api
    /// @return the number of symbols in this block coder
    uint32_t symbols() const;

    /// @ingroup codec_info_api
    /// @return the symbol size of a symbol in bytes
    uint32_t symbol_size() const;

    /// @ingroup codec_info_api
    /// @return the length of the symbol in value_type elements
    uint32_t symbol_length() const;

    /// @ingroup codec_info_api
    /// @return the block size i.e. the total size in bytes
    ///         that this coder operates on. Users may
    ///         use the bytes_used() function provided in the
    ///         symbol storage layers to see how many of those
    ///         bytes are then used.
    uint32_t block_size() const;

    /// @ingroup codec_info_api
    /// Sets the number of bytes used
    /// @param bytes_used number of bytes used of the total coders block size
    void set_bytes_used(uint32_t bytes_used);

    /// @ingroup codec_info_api
    /// @return the number of bytes used
    uint32_t bytes_used() const;

    /// @ingroup codec_info_api
    bool symbol_exists(uint32_t index);


    //
    // CODEC API
    //


    /// @ingroup codec_api
    /// Encodes a symbol according to the symbol id
    ///
    /// @param symbol_data The destination buffer for the encoded symbol
    /// @param symbol_id At this point the symbol id should be initialized
    ///        with the desired coding coefficients.
    void encode_symbol(uint8_t *symbol_data,
                       const uint8_t *symbol_id_coefficients);

    /// @ingroup codec_api
    /// Decodes an encoded symbol according to the coding coefficients
    /// stored in the corresponding symbol_id.
    ///
    /// @param symbol_data The encoded symbol
    /// @param symbol_id The coding coefficients used to create the encoded
    ///                  symbol
    void decode_symbol(uint8_t *symbol_data,
                       uint8_t *symbol_id_coefficients);


    /// @ingroup codec_api
    /// The decode function for systematic packets i.e. specific uncoded
    /// symbols.
    /// @param symbol_data The uncoded source symbol.
    /// @param symbol_index The index of this uncoded symbol in the data
    ///                     block.
    void decode_symbol(const uint8_t *symbol_data, uint32_t symbol_index);


    //
    // MATH API
    //


    /// @ingroup math_api
    /// Multiplies the symbol with the coefficient
    ///     symbol_dest = symbol_dest * coefficient
    ///
    /// @param symbol_dest the destination buffer for the source symbol
    /// @param coefficient the multiplicative constant
    /// @param symbol_length the length of the symbol in value_type elements
    void multiply(value_type *symbol_dest,
                  value_type coefficient,
                  uint32_t symbol_length);

    /// @ingroup math_api
    /// Multiplies the source symbol with the coefficient and adds it to the
    /// destination symbol i.e.:
    ///     symbol_dest = symbol_dest + symbol_src * coefficient
    ///
    /// @param symbol_dest the destination buffer for the source symbol
    /// @param symbol_src the source buffer for the
    /// @param coefficient the multiplicative constant
    /// @param symbol_length the length of the symbol in value_type elements
    void multiply_add(value_type *symbol_dest,
                      const value_type *symbol_src,
                      value_type coefficient,
                      uint32_t symbol_length);

    /// @ingroup math_api
    /// Adds the source symbol adds to the destination symbol i.e.:
    ///     symbol_dest = symbol_dest + symbol_src
    ///
    /// @param symbol_dest the destination buffer holding the resulting
    ///        symbol
    /// @param symbol_src the source symbol
    /// @param symbol_length the length of the symbol in value_type elements
    void add(value_type *symbol_dest,
             const value_type *symbol_src,
             uint32_t symbol_length);

    /// @ingroup math_api
    /// Multiplies the source symbol with the coefficient and subtracts
    /// it from the destination symbol i.e.:
    ///     symbol_dest = symbol_dest - (symbol_src * coefficient)
    ///
    /// @param symbol_dest the destination buffer for the source symbol
    /// @param symbol_src the source buffer for the
    /// @param coefficient the multiplicative constant
    /// @param symbol_length the length of the symbol in value_type elements
    void multiply_subtract(value_type *symbol_dest,
                           const value_type *symbol_src,
                           value_type coefficient,
                           uint32_t symbol_length);

    /// @ingroup math_api
    /// Subtracts the source symbol from the destination symbol i.e.:
    ///     symbol_dest = symbol_dest - symbol_src
    ///
    /// @param symbol_dest the destination buffer holding the resulting
    ///        symbol
    /// @param symbol_src the source symbol
    /// @param symbol_length the length of the symbol in value_type elements
    void subtract(value_type *symbol_dest,
                  const value_type *symbol_src,
                  uint32_t symbol_length);

    /// @ingroup math_api
    /// Inverts the field element
    /// @param value the finite field vale to be inverted.
    /// @return the inverse
    value_type invert(value_type value);

    //
    // SYMBOL STORAGE API
    //

    /// @ingroup storage_api
    /// Copies the encoded / decoded symbols.
    /// @param dest The destination buffer where the symbols should be
    ///        copied. The function will copy block_size() bytes or until
    ///        the dest buffer is full.
    void copy_symbols(sak::mutable_storage dest) const;

    /// @ingroup storage_api
    /// Copies an encoded / decoded symbols.
    /// @param index The index of the symbol to be copied
    /// @param dest The destination buffer where the symbols should be
    ///        copied. The function will copy symbol_size() bytes or until
    ///        the dest buffer is full.
    void copy_symbol(uint32_t index, sak::mutable_storage dest) const;


};





