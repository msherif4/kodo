
/// @brief Doxygen dummy class specifying the API available at different
///        layers
class layer
{
public:

    /// @typedef field_type
    /// The finite field type used e.g. fifi::binary8 for the Finite
    /// Field 2^8
    typedef fifi_finite_field field_type;

    /// @typedef value_type
    /// The value type storing the field elements used for the
    /// specified field. E.g. the binary extension field 2^8 uses
    /// uint8_t as the value_type.
    typedef field::value_type value_type;

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

        /// @ingroup storage_api
        /// @return the maximum number of symbols in a block
        uint32_t max_symbols() const;

        /// @ingroup storage_api
        /// @return the maximum symbol size in bytes
        uint32_t max_symbol_size() const;

        /// @ingroup storage_api
        /// @return The maximum amount of data encoded / decoded in bytes.
        ///         This is calculated by multiplying the maximum number
        ///         of symbols encoded / decoded by the maximum size of
        ///         a symbol.
        uint32_t max_block_size() const;

        /// @ingroup codec_header_api
        /// @brief Can be reimplemented by a symbol header API layer to
        ///        ensure that enough space is available in the header for
        ///        some layer specific data.
        ///
        /// @note If you implement this function you most likely also have
        ///       to implement the layer::header_size() function.
        ///
        /// @return The size in bytes required for the symbol header buffer.
        uint32_t max_header_size();

        /// @ingroup symbol_id_api
        /// @brief Can be reimplemented by a Factory Symbol ID layer
        ///
        /// @note If you implement this function you most likely also have
        ///       to implement the layer::header_size() function.
        ///
        /// @return The maximum size in bytes required for a symbol id.
        /// @see factory_layer_api
        uint32_t max_id_size();

        /// @ingroup payload_codec_api
        ///
        /// @note If you implement this function you most likely also have
        ///       to implement the layer::payload_size() function.
        ///
        /// @return the maximum required payload buffer size in bytes
        uint32_t max_payload_size() const;

        /// @ingroup coefficient_storage_api
        ///
        /// @note If you implement this function you most likely also have
        ///       to implement the layer::coefficients_size() function.
        ///
        /// @return The maximum required coefficients buffer size in bytes
        uint32_t max_coefficients_size() const;

    };

    /// @ingroup factory_api
    /// @brief Constructs the coder with the maximum parameters it will ever
    ///        see. A coder may only be constructed ONCE, but initialized
    ///        many times.
    /// @param max_symbols the maximum symbols this coder can expect
    /// @param max_symbol_size the maximum size of a symbol in bytes
    void construct(uint32_t max_symbols, uint32_t max_symbol_size);

    /// @ingroup factory_api
    /// @brief Initializes the coder
    /// @param symbols the number of symbols the coder should store
    /// @param symbol_size the size of each symbol in bytes
    void initialize(uint32_t symbols, uint32_t symbol_size);


    //
    // CODEC HEADER API
    //


    /// @ingroup codec_header_api
    /// @brief Writes the symbol header.
    /// @param symbol_data The destination buffer for the encoded symbol.
    /// @param symbol_header At this point the symbol header should be
    ///        initialized.
    /// @return The number of bytes used from symbol_header buffer.
    uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_header);

    /// @ingroup codec_header_api
    /// @brief Reads the symbol header.
    /// @param symbol_data The destination buffer for the encoded symbol.
    /// @param symbol_header At this point the symbol header should be
    ///        initialized.
    void decode(uint8_t *symbol_data, uint8_t *symbol_header);

    /// @ingroup codec_header_api
    /// @brief Can be reimplemented by a symbol header API layer to
    ///        ensure that enough space is available in the header for
    ///        some layer specific data.
    ///
    /// @note If you implement this function you most likely also have
    ///       to implement the layer::factory::max_header_size() function.
    ///
    /// @return The size in bytes required for the symbol header buffer.
    uint32_t header_size() const;


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
    /// @param symbol_coefficient Pointer to a pointer of symbol id
    ///                  coefficients. After the call to write_id the
    ///                  symbol_id_coefficient pointer will point to memory
    ///                  storing all coding coefficients corresponding to
    ///                  the symbol id written to the symbol_id buffer.
    /// @return The number of bytes used from the symbol_id buffer.
    uint32_t write_id(uint8_t *symbol_id, uint8_t **symbol_coefficients);

    /// @ingroup symbol_id_api
    /// @brief Can be reimplemented by a Symbol ID layer
    /// @param symbol_id Contains the symbol id for an encoded symbol.
    /// @param symbol_coefficient Pointer to pointer of symbol id
    ///        coefficients. After the call to read_id the
    ///        symbol_id_coefficient pointer will point to memory storing
    ///        all coding coefficients corresponding to the symbol id
    ///        read from the symbol_id buffer.
    void read_id(uint8_t *symbol_id, uint8_t **symbol_coefficients);

    /// @ingroup symbol_id_api
    /// @brief Can be reimplemented by a Symbol ID layer
    ///
    /// @note If you implement this function you most likely also have
    ///       to implement the layer::factory::max_id_size() function.
    ///
    /// @return the size in bytes required for the symbol id buffer.
    uint32_t id_size();


    //
    // CODEC API
    //
    // Here we could add linear block as a sub-group
    // under linear block we put Codec, Symbol IDs, Coding Coefficient Storage


    /// @ingroup codec_api
    /// Encodes a symbol according to the symbol coefficients
    ///
    /// @param symbol_data The destination buffer for the encoded symbol
    /// @param symbol_coefficients At this point the symbol id should be
    ///        initialized with the desired coding coefficients.
    void encode_symbol(uint8_t *symbol_data,
                       const uint8_t *symbol_coefficients);

    /// @ingroup codec_api
    /// Decodes an encoded symbol according to the coding coefficients
    /// stored in the corresponding symbol_id.
    ///
    /// @param symbol_data The encoded symbol
    /// @param symbol_coefficients The coding coefficients used to
    ///        create the encoded symbol
    void decode_symbol(uint8_t *symbol_data,
                       uint8_t *symbol_coefficients);

    /// @ingroup codec_api
    /// The decode function for systematic packets i.e. specific uncoded
    /// symbols.
    /// @param symbol_data The uncoded source symbol.
    /// @param symbol_index The index of this uncoded symbol in the data
    ///                     block.
    void decode_symbol(const uint8_t *symbol_data, uint32_t symbol_index);

    //
    // COEFFICIENT STORAGE API
    //

    /// @ingroup coefficient_storage_api
    ///
    /// @note If you implement this function you most likely also have
    ///       to implement the layer::factory::max_coefficients_size()
    ///       function.
    ///
    /// @return The number of bytes needed to store the symbol coefficients.
    uint32_t coefficients_size() const;

    /// @ingroup coefficient_storage_api
    /// @return The number of layer::value_type elements needed to store
    ///         the symbol coefficients.
    uint32_t coefficients_length() const;

    /// @ingroup coefficient_storage_api
    /// @param index the index in the vector
    /// @return the specified vector
    value_type* coefficients_value(uint32_t index);

    /// @ingroup coefficient_storage_api
    /// @param index the index in the vector
    /// @return the specified vector
    const value_type* coefficients_value(uint32_t index) const;

    /// @ingroup coefficient_storage_api
    /// @param index the index in the vector
    /// @return the specified vector
    uint8_t* coefficients(uint32_t index);

    /// @ingroup coefficient_storage_api
    /// @param index the index in the vector
    /// @return the specified vector
    const uint8_t* coefficients(uint32_t index) const;

    //
    // MATH API
    //

    /// @ingroup math_api
    /// Multiplies the symbol with the coefficient
    ///     symbol_dest = symbol_dest * coefficient
    ///
    /// @param symbol_dest the destination buffer for the source symbol
    /// @param coefficient the multiplicative constant
    /// @param symbol_length the length of the symbol in layer::value_type
    ///        elements
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
    /// @param value the finite field value to be inverted.
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
    void copy_symbols(const sak::mutable_storage &dest) const;

    /// @ingroup storage_api
    /// Copies an encoded / decoded symbols.
    /// @param index The index of the symbol to be copied
    /// @param dest The destination buffer where the symbols should be
    ///        copied. The function will copy symbol_size() bytes or until
    ///        the dest buffer is full.
    void copy_symbol(uint32_t index,
                     const sak::mutable_storage &dest) const;

    /// @ingroup storage_api
    /// @param index the index number of the symbol
    /// @return Returns a pointer to the symbol data. The size of
    ///         the symbol data is provided by the symbol_size() function.
    uint8_t* symbol(uint32_t index);

    /// @ingroup storage_api
    /// @param index the index number of the symbol
    /// @return Returns a const pointer to the symbol data. The size of
    ///         the symbol data is provided by the symbol_size() function.
    const uint8_t* symbol(uint32_t index) const;

    /// @ingroup storage_api
    /// @param index the index number of the symbol
    /// @return Returns a layer::value_type pointer to the symbol data.
    ///         The length of the symbol data is provided by the
    ///         symbol_length() function.
    value_type* symbol_value(uint32_t index);

    /// @ingroup storage_api
    /// @param index the index number of the symbol
    /// @return Returns a const layer::value_type pointer to the symbol
    ///         data. The length of the symbol data is provided by the
    ///         symbol_length() function.
    const value_type* symbol_value(uint32_t index) const;

    /// @ingroup storage_api
    /// Sets the storage for the source symbols
    /// @param symbol_storage A sak::const_storage container initialized
    ///        with the buffer to be use as encoding buffer.
    void set_symbols(const sak::const_storage &symbol_storage);

    /// @ingroup storage_api
    /// Sets the storage for the source symbols
    /// @param symbol_storage A sak::mutable_storage container initialized
    ///        with the buffer to be use as encoding / decoding buffer.
    void set_symbols(const sak::mutable_storage &symbol_storage);

    /// @ingroup storage_api
    /// Sets a symbol - by copying it into the right location in
    /// the buffer.
    /// @param index the index of the symbol into the coding block
    /// @param symbol the actual data of that symbol
    void set_symbol(uint32_t index, const sak::mutable_storage &symbol);

    /// @ingroup storage_api
    /// Sets a symbol - by copying it into the right location in
    /// the buffer.
    /// @param index the index of the symbol into the coding block
    /// @param symbol the actual data of that symbol
    void set_symbol(uint32_t index, const sak::const_storage &symbol);

    /// @ingroup storage_api
    /// @param symbols. A std::vector initialized with pointers to every
    ///        symbol
    void swap_symbols(std::vector<const uint8_t*> &symbols);

    /// @ingroup storage_api
    /// @param symbols. A std::vector initialized with pointers to every
    ///        symbol
    void swap_symbols(std::vector<uint8_t*> &symbols);

    /// @ingroup storage_api
    /// @param index the index number of the symbol
    void swap_symbols(std::vector<uint8_t> &symbols);

    /// @ingroup storage_api
    /// @return the number of symbols in this block coder
    uint32_t symbols() const;

    /// @ingroup storage_api
    /// @return the symbol size of a symbol in bytes
    uint32_t symbol_size() const;

    /// @ingroup storage_api
    /// @return the length of the symbol in layer::value_type elements
    uint32_t symbol_length() const;

    /// @ingroup storage_api
    /// @return the block size i.e. the total size in bytes
    ///         that this coder operates on. Users may
    ///         use the bytes_used() function provided in the
    ///         symbol storage layers to see how many of those
    ///         bytes are then used.
    uint32_t block_size() const;

    /// @ingroup storage_api
    /// Sets the number of bytes used
    /// @param bytes_used number of bytes used of the total coders
    ///        block size
    void set_bytes_used(uint32_t bytes_used);

    /// @ingroup storage_api
    /// @return the number of bytes used
    uint32_t bytes_used() const;

    /// @ingroup storage_api
    /// @param index The index of the symbol to check.
    /// @return true if the symbol has been initialized
    bool symbol_exists(uint32_t index) const;

    //
    // PAYLOAD API
    //

    /// @ingroup payload_codec_api
    /// Encodes a symbol into the provided buffer.
    /// @param payload. The buffer which should contain the encoded
    ///        symbol.
    /// @return the total bytes used from the payload buffer
    uint32_t encode(uint8_t *payload);

    /// @ingroup payload_codec_api
    /// Decodes an encoded symbol stored in the payload buffer
    /// @param payload. The buffer storing the payload of an encoded symbol
    void decode(uint8_t *payload);

    /// @ingroup payload_codec_api
    ///
    /// @note If you implement this function you most likely also have
    ///       to implement the layer::factory::max_header_size() function.
    ///
    /// @return the required payload buffer size in bytes
    uint32_t payload_size() const;

};





